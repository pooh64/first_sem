#ifndef _TREE_H_
#define _TREE_H_

#include <stdio.h>

typedef struct node_t node_t;
typedef double node_data_t;
typedef int node_type_t;

enum TYPES
{
    TYPE_NULL = 0,
    TYPE_NUMBER,
    TYPE_OPERATOR,
    TYPE_VARIABLE,
    TYPE_FUNCTION,
    TYPE_FUNCCALL,
};

enum OPERATORS
{
    #define OPERATOR(str, name) OPERATOR_##name,
    #define OPERATOR_HARD(str, name) OPERATOR_##name,
    OPERATOR_NULL = 0,
    #include"operators.h"
    #undef OPERATOR
    #undef OPERATOR_HARD
};

struct node_t
{
    node_t *left;
    node_t *right;
    node_data_t data;
    node_type_t type;
};

node_t* node_construct();
node_t* node_create(node_t *left, node_t *right, node_data_t data, node_type_t type);
int node_destruct_rec(node_t *node);

int node_set_left(node_t *node, node_t *left);
int node_set_right(node_t *node, node_t *right);
int node_set_data(node_t *node, node_data_t data);
int node_set_type(node_t *node, node_type_t type);
int node_dot_dump(node_t* node);
int node_dot_dump_rec(node_t* node, FILE* output);

node_t* node_get_left(node_t *node);
node_t* node_get_right(node_t *node);

#endif // _TREE_H_
