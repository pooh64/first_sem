#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "tree.h"

#include "translate.h"

#define IED(node, arg) (((node) -> type == TYPE_OPERATOR) && ((node) -> data == arg))
#define ISOP(n) (n -> type == TYPE_OPERATOR)
#define ISNUM(n) (n -> type == TYPE_NUMBER)
#define DATA node -> data
#define IS(arg) ((node -> type == TYPE_OPERATOR) && (node -> data == arg))

#define TL translate(node -> left, out)
#define TR translate(node -> right, out)

namespace translateN
{
    int Num_of_cur_if_label = 1;
    int Cur_move_of_ax = 0;
}

int translate(node_t *node, FILE *out)
{
    assert(node);
    assert(out);
    int tmp = 0;
    //////////////////////////////////////////////////////////////

    if(node -> type == TYPE_FUNCTION)
    {
        fprintf(out, "LABEL F%d\n\n", (int) node -> data); //preparing memory
        translateN :: Num_of_cur_if_label = node -> right -> data;
        TL;
    }

    else if(node -> type == TYPE_FUNCCALL)
    {
        int arg_count = node -> right -> data;
        TL; //pushing args in stack

        fprintf(out, "PUSH AX\n"); //preparing memory
        fprintf(out, "PUSH %d\n", translateN :: Num_of_cur_if_label);
        fprintf(out, "ADD\n");
        fprintf(out, "POP AX\n");

        for(int i = arg_count - 1; i > (- 1); i--) //moving args
        {
            fprintf(out, "PUSH AX\n");
            fprintf(out, "PUSH %d\n", i);
            fprintf(out, "ADD\n");
            fprintf(out, "POP BX\n");
            fprintf(out, "POP [BX]\n");
        }

        fprintf(out, "CALL F%d\n", (int) node -> data); //run
        //fprintf(out, "PUSH CX\n"); //return value

        //memory_back
        fprintf(out, "PUSH %d\n", translateN :: Num_of_cur_if_label);
        fprintf(out, "PUSH AX\n");
        fprintf(out, "SUB\n");
        fprintf(out, "POP AX\n");

    }

    else if(IED(node, OPERATOR_RETURN))
    {
        TL; // pushing return_value in stack
        //fprintf(out, "POP CX\n");
        fprintf(out, "RET\n\n");
    }

    //////////////////////////////////////////////////////////////

    if(node -> type == TYPE_VARIABLE)
    {
        fprintf(out, "PUSH %d\n", (int) node -> data);
        fprintf(out, "PUSH AX\n");
        fprintf(out, "ADD\n");
        fprintf(out, "POP BX\n");
        fprintf(out, "PUSH [BX]\n");
    }

    if(ISNUM(node))
    {
        fprintf(out, "PUSH %lg\n", node -> data);
        return 0;
    }

    else if(IS(OPERATOR_RAM)) //dangerous, it pushes data of ram[i] in stack
    {
        if(node -> left -> type == TYPE_NUMBER)
        {
            fprintf(out, "PUSH [%d]\n", (int) node -> left -> data);
        }
        else
        {
            TL;
            fprintf(out, "POP BX\nPUSH [BX]\n");
        }
    }

    else if(IS(OPERATOR_NEWLINE))
    {
        if(node -> left != NULL)
            TL;
        if(node -> right != NULL)
            TR;
    }
    ///////////////////////////////////////////////////////////////

    else if(IS(OPERATOR_OUT))
    {
        TL;
        fprintf(out, "OUT\n");
    }

    else if(IS(OPERATOR_IN))
    {
        if(IED(node -> left, OPERATOR_RAM))
        {
            if(node -> left -> left -> type == TYPE_NUMBER)
            {
                fprintf(out, "IN\nPOP [%d]\n", (int) node -> left -> left -> data);
            }
            else
            {
                translate(node -> left -> left, out);
                fprintf(out, "POP BX\nIN\nPUSH [BX]\n");
            }
        }

        else if(node -> left -> type == TYPE_VARIABLE)
        {
            fprintf(out, "IN\n");
            fprintf(out, "PUSH %d\n", (int) node -> left -> data);
            fprintf(out, "PUSH AX\n");
            fprintf(out, "ADD\n");
            fprintf(out, "POP BX\n");
            fprintf(out, "POP [BX]\n");
        }
    }

    ///////////////////////////////////////////////////////////////
    #define DEF_CMP(op, cmd) \
    else if(IS(OPERATOR_##op)) \
    { \
        TL; \
        TR; \
        fprintf(out, "PUSH 1\n"); \
        fprintf(out, "POP BX\n"); \
        fprintf(out, "%s l%d\n", #cmd, translateN :: Num_of_cur_if_label); \
        fprintf(out, "PUSH 0\n"); \
        fprintf(out, "POP BX\n"); \
        fprintf(out, "LABEL l%d\n", translateN :: Num_of_cur_if_label); \
        fprintf(out, "PUSH BX\n"); \
        translateN :: Num_of_cur_if_label++; \
    }

    DEF_CMP(IE, JE)
    DEF_CMP(INE, JNE)
    DEF_CMP(IA, JA)
    DEF_CMP(IB, JB)
    ///////////////////////////////////////////////////////////////

    else if(IS(OPERATOR_ASSIGNMENT)) // assign uses bx -> bx banned //add REGS here
    {
        TR; //result is val in stack

        if(IED(node -> left, OPERATOR_RAM))
        {
            if(node -> left -> left -> type == TYPE_NUMBER)
            {
                fprintf(out, "POP [%d]\n", (int) node -> left -> left -> data);
            }
            else
            {
                translate(node -> left -> left, out); //result is val in stack
                fprintf(out, "POP BX\n");
                fprintf(out, "POP [BX]\n");
            }
        }

        if(node -> left -> type == TYPE_VARIABLE)
        {
            fprintf(out, "PUSH %d\n", (int) node -> left -> data);
            fprintf(out, "PUSH AX\n");
            fprintf(out, "ADD\n");
            fprintf(out, "POP BX\n");
            fprintf(out, "POP [BX]\n");
        }
        //regs
    }

    #define ARITHM(name) \
    else if(IS(OPERATOR_##name)) \
    { \
        TR; \
        TL; \
        fprintf(out, #name); \
        fprintf(out, "\n"); \
    }

    ARITHM(ADD)
    ARITHM(SUB)
    ARITHM(MUL)
    ARITHM(DIV)

///////////////////////////////////////////////////////////

    else if(IS(OPERATOR_IF))
    {
        tmp = translateN :: Num_of_cur_if_label;
        translateN :: Num_of_cur_if_label++;
        TL;
        fprintf(out, "PUSH 0\nJE l%d\n", tmp);
        TR;
        fprintf(out, "LABEL l%d\n", tmp);
    }

    else if(IS(OPERATOR_WHILE))
    {
        assert(node -> right);
        assert(node -> left);
        tmp = translateN :: Num_of_cur_if_label;
        translateN :: Num_of_cur_if_label++;
        fprintf(out, "LABEL l%d\n", tmp);
        TR;
        TL;
        fprintf(out, "PUSH 0\nJNE l%d\n", tmp);
    }

///////////////////////////////////////////////////////////

    return 0;
}

int translate_start(node_t *node, FILE *out)
{
    assert(node);
    assert(out);
    fprintf(out, "CALL F0\nEND\n\n");
    translate(node, out);
    return 0;
}
