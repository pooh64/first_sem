#include "tree.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include "grammar.h"
#include "reader.h"

list_t *G_GET_list = NULL;

#define NEXT(G_GET_list) G_GET_list = G_GET_list -> next;

#define dump(G_GET_list) printf("\n data: %lg, type: %d\n", G_GET_list -> data, G_GET_list -> type);

//#define report     printf(" func: %s\ndata: %lg\ntype: %d\n", __func__, G_GET_list -> data, G_GET_list -> type);

#define report

#define IED(G_GET_list, arg) (((G_GET_list) -> type == TYPE_OPERATOR) && ((G_GET_list) -> data == arg))

/**
num -> var -> func () -> * div -> + - -> @ -> /0
**/

node_t* g_get_num()
{
    report;
    if(G_GET_list -> type == TYPE_NUMBER)
    {
        node_data_t tmp = G_GET_list -> data;
        NEXT(G_GET_list);
        return node_create(NULL, NULL, tmp, TYPE_NUMBER);
    }
    return NULL;
}

node_t* g_get_var()
{
    report;
    if(G_GET_list -> type == TYPE_VARIABLE)
    {
        node_data_t tmp = G_GET_list -> data;
        NEXT(G_GET_list);
        return node_create(NULL, NULL, tmp, TYPE_VARIABLE);
    }
    return g_get_num();
}

node_t* g_get_ram()
{
    report;
    if(IED(G_GET_list, OPERATOR_RAM))
    {
        NEXT(G_GET_list);
        assert(IED(G_GET_list, OPERATOR_SQBR1));
        NEXT(G_GET_list);
        node_t *node = g_get_e();
        assert(IED(G_GET_list, OPERATOR_SQBR2));
        NEXT(G_GET_list);
        return node_create(node, NULL, OPERATOR_RAM, TYPE_OPERATOR);
    }
    else
        return g_get_var();
}

node_t* g_get_funccall()
{
    report;
    int num_arg = 0;
    if(G_GET_list -> type == TYPE_FUNCCALL)
    {
        node_data_t tmp = G_GET_list -> data;
        NEXT(G_GET_list);
        assert(IED(G_GET_list, OPERATOR_PRTS1));
        NEXT(G_GET_list);
        node_t *node1 = g_get_e(); //dangerous w/o args
        while(1)
        {
            num_arg++;
            //if(IED(G_GET_list, OPERATOR_COMMA))
            //{
              //  NEXT(G_GET_list);
            //}
            if(IED(G_GET_list, OPERATOR_PRTS2))
            {
                NEXT(G_GET_list);
                return node_create(node1, node_create(NULL, NULL, (double) num_arg, TYPE_NUMBER), tmp, TYPE_FUNCCALL);
            }
            node1 = node_create(node1, g_get_e(), OPERATOR_NEWLINE, TYPE_OPERATOR);
        }
        //return node_create(node1, NULL, G_GET_list -> data, TYPE_FUNCCALL);
    }
    else
        return g_get_ram();
}

node_t* g_get_p()
{
    report;
    if(IED(G_GET_list, OPERATOR_PRTS1))
    {
        NEXT(G_GET_list);
        node_t *node1 = g_get_e();
        assert(IED(G_GET_list, OPERATOR_PRTS2));
        NEXT(G_GET_list);
        return node1;
    }
    else
        return g_get_funccall();
}

node_t* g_get_t()
{
    report;
    node_t *node = g_get_p();
    while(IED(G_GET_list, OPERATOR_MUL) || IED(G_GET_list, OPERATOR_DIV))
    {
        node_data_t op = G_GET_list -> data;
        NEXT(G_GET_list);
        node = node_create(node, g_get_p(), op, TYPE_OPERATOR);
    }
    return node;
}

node_t* g_get_e() // a = - 3
{
    report;
    node_t *node = g_get_t();
    while(IED(G_GET_list, OPERATOR_ADD) || IED(G_GET_list, OPERATOR_SUB))
    {
        node_data_t op = G_GET_list -> data;
        NEXT(G_GET_list);
        if((op == OPERATOR_SUB) && (node == NULL))
            node = node_create(node_create(NULL, NULL, 0, TYPE_NUMBER), g_get_e(), OPERATOR_SUB, TYPE_OPERATOR);
        else
            node = node_create(node, g_get_t(), op, TYPE_OPERATOR);
    }
    return node;
}

node_t* g_get_cmp()
{
    report;
    node_t *node1 = g_get_e();
    if(IED(G_GET_list, OPERATOR_IE) || IED(G_GET_list, OPERATOR_IA) || IED(G_GET_list, OPERATOR_IB) || IED(G_GET_list, OPERATOR_INE))
    {
        node_data_t op = G_GET_list -> data;
        NEXT(G_GET_list);
        return node_create(node1, g_get_e(), op, TYPE_OPERATOR);
    }
    else
    {
        return node1;
    }
    return node1;
}

node_t* g_get_assign()
{
    report;
    node_t *node1 = g_get_cmp();
    if(IED(G_GET_list, OPERATOR_ASSIGNMENT))
    {
        NEXT(G_GET_list);
        return node_create(node1, g_get_cmp(), OPERATOR_ASSIGNMENT, TYPE_OPERATOR);
    }
    /*else
    { //
        dump(G_GET_list);
        assert(!"find =");
    }*/
    return node1; // only lines with ....=....
}

node_t* g_get_f()
{
    report;
    if(IED(G_GET_list, OPERATOR_IN))
    {
        NEXT(G_GET_list);
        return node_create(g_get_ram(), NULL, OPERATOR_IN, TYPE_OPERATOR);
    }
    else if(IED(G_GET_list, OPERATOR_OUT))
    {
        NEXT(G_GET_list);
        return node_create(g_get_e(), NULL, OPERATOR_OUT, TYPE_OPERATOR);
    }
    else if(IED(G_GET_list, OPERATOR_RETURN))
    {
        NEXT(G_GET_list);
        return node_create(g_get_e(), NULL, OPERATOR_RETURN, TYPE_OPERATOR);
    }
    else
        return g_get_assign();
}

node_t* g_get_if()
{
    report;
    if(IED(G_GET_list, OPERATOR_IF))
    {
        NEXT(G_GET_list);
        assert(IED(G_GET_list, OPERATOR_PRTS1));
        NEXT(G_GET_list);
        node_t *node1 = g_get_f();
        assert(IED(G_GET_list, OPERATOR_PRTS2));
        NEXT(G_GET_list);
        assert(IED(G_GET_list, OPERATOR_PRTS1));
        NEXT(G_GET_list);
        node_t *node2 = g_get_line();
        assert(IED(G_GET_list, OPERATOR_PRTS2));
        NEXT(G_GET_list);
        return node_create(node1, node2, OPERATOR_IF, TYPE_OPERATOR);
    }
    else if(IED(G_GET_list, OPERATOR_WHILE))
    {
        NEXT(G_GET_list);
        assert(IED(G_GET_list, OPERATOR_PRTS1));
        NEXT(G_GET_list);
        node_t *node1 = g_get_f();
        assert(IED(G_GET_list, OPERATOR_PRTS2));
        NEXT(G_GET_list);
        assert(IED(G_GET_list, OPERATOR_PRTS1));
        NEXT(G_GET_list);
        node_t *node2 = g_get_line();
        assert(IED(G_GET_list, OPERATOR_PRTS2));
        NEXT(G_GET_list);
        return node_create(node1, node2, OPERATOR_WHILE, TYPE_OPERATOR);
    }
    else
    {
        return g_get_f();
    }
}

int g_find_max_var(list_t *lst, int depth)
{
    report;
    int max = -1;
    while(depth > 0)
    {
        assert(lst);
        if(IED(lst, OPERATOR_PRTS1))
            depth++;
        if(IED(lst, OPERATOR_PRTS2))
            depth--;
        if((lst -> type == TYPE_VARIABLE))
        {
            if((lst -> data) > max)
                max = lst -> data;
        }
        lst = lst -> next;
    }
    return max;
}

node_t* g_get_line()
{
    report;
    node_t *node = g_get_if();
    while(IED(G_GET_list, OPERATOR_NEWLINE))
    {
        NEXT(G_GET_list);
        node = node_create(node, g_get_if(), OPERATOR_NEWLINE, TYPE_OPERATOR);
    }
    return node;
}

node_t* g_get_func()
{
    report;
    node_t *node = NULL;
    node_t *node2 = NULL;
    if(G_GET_list -> type == TYPE_FUNCTION)
    {
        while(G_GET_list -> type == TYPE_FUNCTION)
        {
            node_data_t tmp = G_GET_list -> data;
            NEXT(G_GET_list);
            assert(IED(G_GET_list, OPERATOR_PRTS1));
            NEXT(G_GET_list);
            int num_of_var = g_find_max_var(G_GET_list, 1) + 1; // ((( ))))
            node_t *node1 = g_get_line();
            assert(IED(G_GET_list, OPERATOR_PRTS2));
            NEXT(G_GET_list);
            node2 = node_create(node1, node_create(NULL, NULL, (double) num_of_var, TYPE_NUMBER), tmp, TYPE_FUNCTION);
            if(node == NULL)
                node = node2;
            else
                node = node_create(node, node2, OPERATOR_NEWLINE, TYPE_OPERATOR);

        }
        return node;
    }
    else
        return g_get_line();
}

node_t* g_get_go(list_t *list0) //n fix
{
    G_GET_list = list0;
    report;
    node_t *node = g_get_func();
    //if(!IED(G_GET_list, OPERATOR_EOF))
    //{
        //printf("\nMistake found\n");
        //abort();
   // }
    return node;
}
