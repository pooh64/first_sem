#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<time.h>
#include<math.h>
#include"diff.h"
#include"tree.h"

int fixed = 0;
int depth = 0;

extern FILE* tex;

int StupidReport(FILE* output)
{
    int n = rand()%8;
    char rep[10][100] = {"through difficult transformations:", "making a few substitutions, we get:", \
    "in a few hours of hard work:", "thus it is obvious that:", "after five cups of coffee:", "after hard calculations:", \
    "that was easy:", "by asking the teacher"};
    fprintf(output, "\n%s\n", rep[n]);
    return 0;
}

int PrintTreeTex(tree_node_t* node, FILE* output)
{
    assert(output);
    fprintf(output, "\\[\n");
    PrintTreeTexRec(node, output);
    fprintf(output, "\n\\]\n");
    return 0;
}

int PrintTexInit(FILE* output)
{
    assert(output);
    fprintf(output, "\\documentclass[12pt]{article} \n\
    \\usepackage[cp1251]{inputenc} \n\
    \\usepackage[russian]{babel} \n\
    \\begin{document}\n");
    return 0;
}

int PrintTexEnd(FILE* output)
{
    assert(output);
    fprintf(output, "\n\\end{document}");
    return 0;
}

int GetTexPdf()
{
    system("pdflatex tex.tex");
    system("xdg-open tex.pdf");
    return 0;
}

int PrintTreeTexRec(tree_node_t* node, FILE* output)
{
    #define OPERATOR(str, name, code, print) \
    else if(TreeGetNodeData(node) == OPERATOR_##name) \
        PrintNodeOperator(node, print, output);

    assert(node);

    if(IS_NUM(node))
    {
        if(DATA(node) >= 0)
            fprintf(output, "%lg", DATA(node));
        else
            fprintf(output, "(%lg)", DATA(node));
    }
    else if(IS_VAR(node))
        fprintf(output, "%s", DerivateGetVariableStr(node -> data));

    #include"operators.h"
    #undef OPERATOR
    return 0;
}

int PrintNodeOperator(tree_node_t* node, int method, FILE* output) //priority
{
    assert(node);
    if(method == 0)
    {
        if((DATA(node) == OPERATOR_MULT) || (DATA(node) == OPERATOR_DIV)) // ...*...
        {
            if(IS_OP(LEFT(node)))
            {
                if((DATA(LEFT(node)) == OPERATOR_PLUS) || (DATA(LEFT(node)) == OPERATOR_MINUS)) // (1+2)*...
                {
                    fprintf(output, "\\left(");
                    PrintTreeTexRec(node -> left, output);
                    fprintf(output, "\\right)");
                }
                else
                    PrintTreeTexRec(node -> left, output);
            }
            else
                PrintTreeTexRec(node -> left, output);

            fprintf(output, "%s", DerivateGetOperatorStr(node -> data));

            if(IS_OP(RIGHT(node)))
            {
                if((DATA(RIGHT(node)) == OPERATOR_PLUS) || (DATA(RIGHT(node)) == OPERATOR_MINUS)) // ...*(1+2)
                {
                    fprintf(output, "\\left(");
                    PrintTreeTexRec(node -> right, output);
                    fprintf(output, "\\right)");
                }
                else
                    PrintTreeTexRec(node -> right, output);
            }
            else
                PrintTreeTexRec(node -> right, output);
        }
        else
        {
            PrintTreeTexRec(node -> left, output);
            fprintf(output, "%s", DerivateGetOperatorStr(node -> data));
            PrintTreeTexRec(node -> right, output);
        }
    }

    else if(method == 1)
    {
        fprintf(output, "%s\\left(", DerivateGetOperatorStr(node -> data));
        PrintTreeTexRec(node -> left, output);
        fprintf(output, "\\right)");
    }

    else if(method == -1)
    {
        if(DATA(node) == OPERATOR_POW)
        {
            fprintf(output, "{\\left(");
            PrintTreeTexRec(node -> left, output);
            fprintf(output, "\\right)}%s{", DerivateGetOperatorStr(DATA(node)));
            PrintTreeTexRec(node -> right, output);
            fprintf(output, "}");
        }
        else if(DATA(node) == OPERATOR_DIV)
        {
            fprintf(output, "\\frac{");
            PrintTreeTexRec(node -> left, output);
            fprintf(output, "}{");
            PrintTreeTexRec(node -> right, output);
            fprintf(output, "}");
        }
        else if(DATA(node) == OPERATOR_EXP)
        {
            fprintf(output, "{\\left(e\\right)}^{");
            PrintTreeTexRec(node -> left, output);
            fprintf(output, "}");
        }
        else if(DATA(node) == OPERATOR_MULT)
        {
            PrintTreeTexRec(node -> left, output);
            fprintf(output, " \\cdot ");
            PrintTreeTexRec(node -> right, output);
        }
        else
            printf("\nPrintNodeOperator fail\n\n");
    }

    return 0;
}

int DerivateGetOperatorInt(char* data)
{
    assert(data);
    #define OPERATOR(str, name, code, print) \
    if(!strcmp(#str, data)) \
        return OPERATOR_##name;
    #include"operators.h"
    #undef OPERATOR
    return 0;
}

char* DerivateGetOperatorStr(double data)
{
    #define OPERATOR(str, name, code, print) \
    if(data == OPERATOR_##name) \
        return #str;
    #include"operators.h"
    #undef OPERATOR
    return 0;
}

int DerivateGetVariableInt(char* data)
{
    assert(data);
    #define VARIABLE(str) \
    if(!strcmp(#str, data)) \
        return VARIABLE_##str;
    #include"variables.h"
    #undef VARIABLE
    return 0;
}

char* DerivateGetVariableStr(double data)
{
    #define VARIABLE(str) \
    if(data == VARIABLE_##str) \
        return #str;
    #include"variables.h"
    #undef VARIABLE
    return 0;
}


tree_node_t* Slozhna(tree_node_t* node, tree_node_t* (diff)(tree_node_t* node))
{
    assert(node);
    return MULT(diff(node), DL);
}

tree_node_t* DerivateEXP(tree_node_t* node)
{
    assert(node);
    return EXP(node);
}

tree_node_t* DerivateTranslatePOW(tree_node_t* node) //dont forget to free
{
    assert(node);
    //tree_node_t* newnode = EXP(MULT(LN(TreeGetLeftNode(node)), TreeGetRightNode(node)));
    tree_node_t* left = TreeGetLeftNode(node);
    tree_node_t* right = TreeGetRightNode(node);
    TreeSetNodeData(node, OPERATOR_EXP);
    TreeSetNodeRight(node, NULL);

    tree_node_t* newnode1 = TreeCreateNode(TYPE_OPERATOR, OPERATOR_LN, left, NULL, NULL);

    tree_node_t* newnode2 = TreeCreateNode(TYPE_OPERATOR, OPERATOR_MULT, newnode1, right, node);

    TreeSetNodeLeft(node, newnode2);
    TreeSetNodeParent(newnode1, newnode2);
    TreeSetNodeParent(left, newnode1);
    TreeSetNodeParent(right, newnode2);

    //TreeDotDump(node);

    return node;
}

tree_node_t* DerivateLN(tree_node_t* node)
{
    assert(node);
    return DIV(NUM(1), CL);
}

tree_node_t* DerivateSIN(tree_node_t* node)
{
    assert(node);
    return COS(node);
}

tree_node_t* DerivateCOS(tree_node_t* node)
{
    assert(node);
    return MULT(NUM(-1), SIN(node));
}

tree_node_t* DerivateTAN(tree_node_t* node)
{
    assert(node);
    return DIV(NUM(1), POW(COS(node), NUM(2)));
}

tree_node_t* DerivatePowConst(tree_node_t* node)
{
    assert(node);
    return MULT(CR, POW(CL, MINUS(CR, NUM(1))));
}

tree_node_t* DerivateChosePOW(tree_node_t* node)
{
    assert(node);
    if(IsConst(TreeGetRightNode(node)))
    {
        return Slozhna(node, DerivatePowConst);
    }
    else
    {
        DerivateTranslatePOW(node);
        return Slozhna(node, DerivateEXP);
    }
    assert(0);
    return NULL;
}

tree_node_t* Derivate(tree_node_t* node)
{
    assert(node);
    tree_node_t* tmp = NULL;
    if(IS_NUM(node))
    {
        tmp = NUM(0);
        fprintf(tex, "Derivate of:");
        PrintTreeTex(node, tex);
        StupidReport(tex);
        PrintTreeTex(tmp, tex);
        return tmp;
    }
    if(IS_VAR(node))
    {
        tmp = NUM(1);
        fprintf(tex, "Derivate of:");
        PrintTreeTex(node, tex);
        StupidReport(tex);
        PrintTreeTex(tmp, tex);
        return tmp;  // upd in future
    }
    if(IS_OP(node))
    {
        switch((int) node -> data)
        {
            #define OPERATOR(str, name, code, print) \
            case OPERATOR_##name: \
            { \
                tmp = code; \
                fprintf(tex, "Derivate of:"); \
                PrintTreeTex(node, tex); \
                StupidReport(tex); \
                SimplifyRec(tmp); \
                PrintTreeTex(tmp, tex); \
                return tmp; \
            }
            #include"operators.h"
            #undef OPERATOR

            default:
                printf("fail\n");
                break;
        }
    }
    return NULL;
}

int IsConst(tree_node_t* node)
{
    assert(node);
    if(TreeGetNodeType(node) == TYPE_VARIABLE)
        return 0;
    if(node -> left != NULL)
        if(!IsConst(node -> left))
            return 0;
    if(node -> right != NULL)
        if(!IsConst(node -> right))
            return 0;
    return 1;
}

int Count(tree_node_t* node)
{
    assert(node);
    tree_value_t val = CountRec(node);
    TreeSetNodeData(node, val);
    TreeSetNodeType(node, TYPE_NUMBER);
    TreeDeleteNodeUnder(node);
    return 1;
}

tree_value_t CountRec(tree_node_t* node)
{
    assert(node);
    if(IS_OP(node))
    {
        if(TreeGetNodeData(node) == OPERATOR_PLUS)
            return CountRec(node -> left) + CountRec(node -> right);
        if(TreeGetNodeData(node) == OPERATOR_MULT)
            return CountRec(node -> left) * CountRec(node -> right);
        if(TreeGetNodeData(node) == OPERATOR_MINUS)
            return CountRec(node -> left) - CountRec(node -> right);
        if(TreeGetNodeData(node) == OPERATOR_POW)
            return pow(CountRec(node -> left), CountRec(node -> right));
        if(TreeGetNodeData(node) == OPERATOR_LN)
            return log(CountRec(node -> left));
        if(TreeGetNodeData(node) == OPERATOR_SIN)
            return sin(CountRec(node -> left));
        if(TreeGetNodeData(node) == OPERATOR_COS)
            return cos(CountRec(node -> left));
        if(TreeGetNodeData(node) == OPERATOR_TAN)
            return tan(CountRec(node -> left));
        printf("\nfail in count rec: %s\n", DerivateGetOperatorStr(node -> data));
    }
    else
        return TreeGetNodeData(node);
    return 0;
}

int SimplifyMult(tree_node_t* node)
{
    assert(node);
    tree_node_t* tmp = NULL;
    tree_node_t* tmp1 = NULL;
    assert(node);
    if((LEFTDATA == 0) || (RIGHTDATA == 0))
    {
        TreeDeleteNodeUnder(node);
        TreeSetNodeData(node, 0);
        TreeSetNodeType(node, TYPE_NUMBER);
        fixed = 1;
        return 1;
    }
    if(IS_NUM(TreeGetLeftNode(node)) && (LEFTDATA == 1))
    {
        tmp = TreeGetRightNode(node);
        TreeCopyNode(node, tmp);
        TreeSetNodeParent(node, NULL);
        //free(tmp);
        fixed = 1;
        return 1;
    }
    if(IS_NUM(TreeGetRightNode(node)) && (RIGHTDATA == 1))
    {
        tmp = TreeGetLeftNode(node);
        TreeCopyNode(node, tmp);
        TreeSetNodeParent(node, NULL);
        fixed = 1;
        return 1;
    }
    if(IS_OP(LEFT(node)) && (LEFTDATA == OPERATOR_DIV))
    {
        if(IS_NUM(LEFT(LEFT(node))) && (DATA(LEFT(LEFT(node))) == 1))
        {
            tmp = TreeCloneNode(RIGHT(LEFT(node)));
            tmp1 = TreeCloneNode(RIGHT(node));
            TreeDeleteNodeUnder(node);
            TreeSetNodeData(node, OPERATOR_DIV);
            TreeSetNodeLeft(node, tmp1);
            TreeSetNodeRight(node, tmp);
            fixed = 1;
            return 1;
        }
    }
    if(IS_OP(RIGHT(node)) && (RIGHTDATA == OPERATOR_DIV))
    {
        if(IS_NUM(LEFT(RIGHT(node))) && (DATA(LEFT(RIGHT(node))) == 1))
        {
            tmp = TreeCloneNode(RIGHT(RIGHT(node)));
            tmp1 = TreeCloneNode(LEFT(node));
            TreeDeleteNodeUnder(node);
            TreeSetNodeData(node, OPERATOR_DIV);
            TreeSetNodeLeft(node, tmp1);
            TreeSetNodeRight(node, tmp);
            fixed = 1;
            return 1;
        }
    }
    return 0;
}

int SimplifyPlus(tree_node_t* node)
{
    assert(node);
    if(IS_NUM(TreeGetLeftNode(node)) && (LEFTDATA == 0))
    {
        TreeCopyNode(node, TreeGetRightNode(node));
        TreeSetNodeParent(node, NULL);
        fixed = 1;
        return 1;
    }
    if(IS_NUM(TreeGetRightNode(node)) && (RIGHTDATA == 0))
    {
        TreeCopyNode(node, TreeGetLeftNode(node)); //free
        TreeSetNodeParent(node, NULL);
        fixed = 1;
        return 1;
    }
    return 0;
}

int SimplifyPow(tree_node_t* node)
{
    assert(node);
    if(LEFTDATA == 0)
    {
        TreeDeleteNodeUnder(node);
        TreeSetNodeData(node, 0);
        TreeSetNodeType(node, TYPE_NUMBER);
        fixed = 1;
        return 1;
    }
    if(RIGHTDATA == 0)
    {
        TreeDeleteNodeUnder(node);
        TreeSetNodeData(node, 1);
        TreeSetNodeType(node, TYPE_NUMBER);
        fixed = 1;
        return 1;
    }
    if((IS_NUM(node -> left)) && (LEFTDATA == 1))
    {
        TreeDeleteNodeUnder(node);
        TreeSetNodeData(node, 1);
        TreeSetNodeType(node, TYPE_NUMBER);
        fixed = 1;
        return 1;
    }
    if((IS_NUM(node -> right)) && (RIGHTDATA == 1))
    {
        TreeDeleteNodeRight(node);
        TreeCopyNode(node, TreeGetLeftNode(node));
        TreeSetNodeParent(node, NULL);
        fixed = 1;
        return 1;
    }
    return 0;
}

int Simplify(tree_node_t* node, int nSimpl)
{
    assert(node);
    fprintf(tex, "Simplify:");
    fixed = 1;
    for(int i = 0; (i < nSimpl) && (fixed == 1); i++)
    {
        fixed = 0;
        SimplifyRec(node);
        if(fixed == 1)
        {
            StupidReport(tex);
            PrintTreeTex(node, tex);
        }
    }
    return 0;
}

int SimplifyRec(tree_node_t* node)
{
    assert(node);
    if(!(IS_OP(node)))
        return 0;
    if(TreeGetNodeData(node) == OPERATOR_MULT)
        SimplifyMult(node);
    else if(TreeGetNodeData(node) == OPERATOR_PLUS)
        SimplifyPlus(node);
    else if(TreeGetNodeData(node) == OPERATOR_POW)
        SimplifyPow(node);
    //else if(IsConst(node)) ...............................
        //Count(node);
    if(TreeGetLeftNode(node) != NULL)
        SimplifyRec(TreeGetLeftNode(node));
    if(TreeGetRightNode(node) != NULL)
        SimplifyRec(TreeGetRightNode(node));
    return 0;
}
