#include "tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

node_t* node_construct()
{
    node_t *node = (node_t*) calloc(1, sizeof(node_t));
    assert(node);
    return node;
}

node_t* node_create(node_t *left, node_t *right, node_data_t data, node_type_t type)
{
    node_t *node = (node_t*) calloc(1, sizeof(node_t));
    assert(node);
    node -> left = left;
    node -> right = right;
    node -> data = data;
    node -> type = type;
    return node;
}

int node_destruct_rec(node_t *node)
{
    assert(node);
    if(node -> left != NULL)
        node_destruct_rec(node -> left);
    if(node -> right != NULL)
        node_destruct_rec(node -> right);
    free(node);
    return 0;
}

int node_set_left(node_t *node, node_t *left)
{
    assert(node);
    node -> left = left;
    return 0;
}

int node_set_right(node_t *node, node_t *right)
{
    assert(node);
    node -> right = right;
    return 0;
}

int node_set_data(node_t *node, node_data_t data)
{
    assert(node);
    node -> data = data;
    return 0;
}

int node_set_type(node_t *node, node_type_t type)
{
    assert(node);
    node -> type = type;
    return 0;
}

node_t* node_get_left(node_t *node)
{
    assert(node);
    return node -> left;
}

node_t* node_get_right(node_t *node)
{
    assert(node);
    return node -> right;
}

node_type_t node_get_type(node_t *node)
{
    assert(node);
    return node -> type;
}

node_data_t node_get_data(node_t *node)
{
    assert(node);
    return node -> data;
}

int node_get_operator_int(char *data)
{
    assert(data);
    #define OPERATOR(str, name) \
    if(!strcmp(str, data)) \
        return OPERATOR_##name;
    #include"operators.h"
    #undef OPERATOR
    return 0;
}

char* node_get_operator_str(double data)
{
    #define OPERATOR(str, name) \
    if(data == OPERATOR_##name) \
        return str;
    #include"operators.h"
    #undef OPERATOR
    return 0;
}

int node_print_file_step(node_t *node, FILE *output)
{
    assert(node);
    if(node_get_type(node) == TYPE_NUMBER)
        fprintf(output, "(\"%lg\"\n", node_get_data(node));
    if(node_get_type(node) == TYPE_OPERATOR)
        fprintf(output, "(\"%s\"\n", node_get_operator_str(node_get_data(node)));
    return 0;
}

int node_print_file(node_t *node, FILE *output, int ntab)
{
    assert(node);
    int i = 0;
    for(i = 0; i < ntab; i++)
        fprintf(output, "\t");
    node_print_file_step(node_get_right(node), output);

    if(node -> left != NULL)
    {
        node_print_file(node -> left, output, ntab + 1);
    }

    if(node -> right != NULL)
    {
        node_print_file(node -> left, output, ntab + 1);
    }

    for(i = 0; i < ntab; i++)
        fprintf(output, "\t");
    fprintf(output, ")\n");
    return 0;
}

char* node_read_text_rec(node_t* node, char* text)
{
    char *data = 0;
    while(*text != '\"')
    {
        text++;
    }
    text++;
    data = text;
    while(*text != '\"')
        text++;
    *text = '\0';

    if(isdigit(*data)) //number
    {
        node_set_data(node, atof(data));
        node_set_type(node, TYPE_NUMBER);
    }
    else if(node_get_operator_int(data)) //operator
    {
        node_set_data(node, node_get_operator_int(data));
        node_set_type(node, TYPE_OPERATOR);
    }
    else
    {
        printf("\nRead failed\n\n");
        return 0;
    }

    while(*text != '(')
    {
        if(*text == ')')
            return text;
        text++;
    }

    node -> left = node_construct();
    node -> right = node_construct();

    text = node_read_text_rec(node -> left, text);
    text = node_read_text_rec(node -> right, text);

    return text;
}

node_t* node_read_text(char *text)
{
    node_t* node = node_construct();
    node_read_text_rec(node, text);
    return node;
}

int node_dot_dump(node_t* node)
{
    assert(node);
    FILE* dot = fopen("./test/dotText.dot", "w");
    fprintf(dot, "digraph dump {\n    node [shape = record];\n");
    node_dot_dump_rec(node, dot);
    fprintf(dot, "}");
    fclose(dot);
    system("dot -Tps ./test/dotText.dot -o ./test/treeDump.ps");
    system("xdg-open ./test/treeDump.ps");
    return 0;
}

int node_dot_dump_rec(node_t* node, FILE* output)
{
    assert(node);
    if(node -> type == TYPE_NUMBER)
        fprintf(output, "%d [shape = record, label = \"{data: %lg|ptr: %p|type: NUMBER|left: %p|right: %p}\"];\n", \
            node, node -> data, node, node -> left, node -> right);
    if(node -> type == TYPE_OPERATOR)
    {
        if((node -> data != OPERATOR_IA) && (node -> data != OPERATOR_IB))
        fprintf(output, "%d [shape = record, label = \"{data: %s|ptr: %p|type: OPERATOR|left: %p|right: %p}\"];\n", \
            node, node_get_operator_str(node -> data), node, node -> left, node -> right);
        else if(node -> data == OPERATOR_IA)
            fprintf(output, "%d [shape = record, label = \"{data: %s|ptr: %p|type: OPERATOR|left: %p|right: %p}\"];\n", \
            node, "IA", node, node -> left, node -> right);
        else if(node -> data == OPERATOR_IB)
            fprintf(output, "%d [shape = record, label = \"{data: %s|ptr: %p|type: OPERATOR|left: %p|right: %p}\"];\n", \
            node, "IB", node, node -> left, node -> right);
    }
    if(node -> type == TYPE_VARIABLE)
        fprintf(output, "%d [shape = record, label = \"{data: %lg|ptr: %p|type: VARIABLE|left: %p|right: %p}\"];\n", \
            node, node -> data, node, node -> left, node -> right);
    if(node -> type == TYPE_FUNCTION)
        fprintf(output, "%d [shape = record, label = \"{data: %lg|ptr: %p|type: FUNCTION|left: %p|right: %p}\"];\n", \
            node, node -> data, node, node -> left, node -> right);
    if(node -> type == TYPE_FUNCCALL)
        fprintf(output, "%d [shape = record, label = \"{data: %lg|ptr: %p|type: FUNCCALL|left: %p|right: %p}\"];\n", \
            node, node -> data, node, node -> left, node -> right);
    if(node -> left != NULL)
    {
        fprintf(output, "%d -> %d\n", node, node -> left);
        node_dot_dump_rec(node -> left, output);
    }
    if(node -> right != NULL)
    {
        node_dot_dump_rec(node -> right, output);
        fprintf(output, "%d -> %d\n", node, node -> right);
    }
    return 0;
}
