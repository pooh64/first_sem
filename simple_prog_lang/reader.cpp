#include "reader.h"
#include "tree.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

char *text;
int R_num_var = 0;
int R_num_func = 0;

#define MAX_VARS 100
#define MAX_VAR_LEN 20
#define MAX_FUNCS 100
#define MAX_FUNC_LEN 20
char R_var_names[MAX_VARS][MAX_VAR_LEN] = {};
char R_func_names[MAX_FUNCS][MAX_FUNC_LEN] = {};
char R_main_name[] = "main";

#define IED(list, arg) (((list) -> type == TYPE_OPERATOR) && ((list) -> data == arg))



list_t* r_get_op();


list_t* list_construct()
{
    return (list_t*) calloc(1, sizeof(list_t));
}

list_t* list_create(node_data_t data, node_type_t type, list_t *next)
{
    list_t *list = (list_t*) calloc(1, sizeof(list_t));
    list -> data = data;
    list -> type = type;
    list -> next = next;
    return list;
}

int list_set_next(list_t *list, list_t *newlist)
{
    assert(list);
    assert(newlist);
    list -> next = newlist;
    return 0;
}

list_t* list_add(list_t *list, list_t *newlist)
{
    assert(list);
    assert(newlist);
    list -> next = newlist;
    return newlist;
}

int list_destruct_rec(list_t *list)
{
    assert(list);
    if(list -> next != NULL)
        list_destruct_rec(list -> next);
    free(list);
    return 0;
}


int r_cmp(char *a, char *b) //str, text!!!!!!!!!!
{
    char a0 = *a;
    if((*a == '\0') && (*b == '\0'))
        return 0;
    if((*a == '\0') || (*b == '\0'))
        return 1;
    while(1)
    {
        if(*a != *b)
            return a - b;
        a++;
        b++;
        if((*a == '\0'))
        {
            if(isalpha(a0) && isalpha(*b))
                return 1;
            return 0;
        }
    }
    return 0;
}

int r_get_var(char *str)
{
    for(int i = 0; i < R_num_var; i++)
    {
        if(!r_cmp(R_var_names[i], str))
        {
            return i;
        }
    }
    return -1;
}

int r_get_func(char *str)
{
    for(int i = 0; i < R_num_func; i++)
    {
        if(!r_cmp(R_func_names[i], str))
            return i;
    }
    return -1;
}


list_t* r_get_num()
{
    while(isspace(*text))
        text++;

    node_data_t val = 0;

    if(!isdigit(*text))
    {
        printf("\nError: %s\n", text);
        assert(0);
        return NULL;
    }

    while(('0' <= *text) && (*text <= '9'))
    {
        val = val * 10 + *text - '0';
        text++;
    }
    if((*text == ',') || (*text == '.'))
    {
        text++;
        for(int i = 1; ('0' <= *text) && (*text <= '9'); i++)
        {
            val = val + (*text - '0') / pow(10, i);
            text++;
        }
    }
    list_t *list = list_create(val, TYPE_NUMBER, NULL);
    //printf("text: %s got: %lg\n\n", text, val);
    while(isspace(*text))
        text++;

    return list;
}

int r_add_var()
{
    int i = 0;
    R_num_var++;
    while(isspace(*text))
        text++;
    while(isalnum(*text))
    {
        R_var_names[R_num_var - 1][i] = *text;
        text++;
        i++;
    }
    return 0;
}

list_t* r_add_func()
{
    R_num_var = 0;
    int i = 0;
    list_t *list1 = NULL;

    while(isspace(*text))
        text++;

    if(r_get_func(text) == 0)
    {
        while(isalnum(*text))
            text++;

        list1 = r_get_op();
        assert(IED(list1, OPERATOR_PRTS1));

        while(1)
        {
            free(list1);
            list1 = r_get_op();
            if(IED(list1, OPERATOR_PRTS2))
                break;
        }
        free(list1);
        return list_create(0, TYPE_FUNCTION, NULL);
    }

    if(r_get_func(text) == -1)
    {
        R_num_func++;
        while(isalnum(*text))
        {
            R_func_names[R_num_func - 1][i] = *text;
            text++;
            i++;
        }

        list1 = r_get_op();
        assert(IED(list1, OPERATOR_PRTS1));
        free(list1);

        while(1)
        {
            list1 = r_get_op();
            if(IED(list1, OPERATOR_PRTS2))
                break;
            else
                free(list1);
        }
        free(list1);
        return list_create(R_num_func - 1, TYPE_FUNCTION, NULL);
    }
    return NULL;
}


list_t* r_get_op()
{
    while(isspace(*text))
        text++;

    if(R_num_func > 0)
    {
        int tmp = r_get_func(text);
        if(tmp != -1)
        {
            text = text + strlen(R_func_names[tmp]);
            return list_create((double) tmp, TYPE_FUNCCALL, NULL);
        }
    }

    if(!r_cmp("func", text))
    {
        text = text + strlen("func");
        return r_add_func();
    }

    if(R_num_var > 0)
    {
        int tmp = r_get_var(text);
        if(tmp != -1)
        {
            text = text + strlen(R_var_names[tmp]);
            return list_create((double) tmp, TYPE_VARIABLE, NULL);
        }
    }

    if(!r_cmp("var", text))
    {
        text = text + strlen("var");
        r_add_var();
        list_t *list1 = r_get_op(); //eat next newline symb (@)
        if((list1 -> type == TYPE_OPERATOR) && (list1 -> data == OPERATOR_NEWLINE))
        {
            free(list1);
            return r_get_op();
        }
        return list1;
    }

    #define OPERATOR(str, name) \
    if(!r_cmp(str, text)) \
    { \
        text = text + strlen(str); \
        return list_create(OPERATOR_##name, TYPE_OPERATOR, NULL); \
    }

    #include "operators.h"

    #undef OPERATOR

    return r_get_num();
}

list_t* r_go(char *str)
{
    assert(str);
    strncpy(R_func_names[0], R_main_name, strlen(R_main_name));
    R_num_func = 1;

    text = str;
    printf("%s", text);
    list_t *list0 = r_get_op();
    list_t *list = list0;
    while(1)
    {
        list = list_add(list, r_get_op());
        assert(list);
        if((list -> type == TYPE_OPERATOR) && (list -> data == OPERATOR_EOF))
           return list0;
    }
}
