#ifndef DEF_DIFF
#define DEF_DIFF
#include"tree.h"

#define REPORT_FILE "tex.tex"

#define DATA(node) TreeGetNodeData(node)
#define TYPE(node) TreeGetNodeType(node)
#define LEFT(node) TreeGetLeftNode(node)
#define RIGHT(node) TreeGetRightNode(node)

#define LEFTDATA TreeGetNodeData(TreeGetLeftNode(node))
#define RIGHTDATA TreeGetNodeData(TreeGetRightNode(node))
#define IS_NUM(node) TreeGetNodeType(node) == TYPE_NUMBER
#define IS_VAR(node) TreeGetNodeType(node) == TYPE_VARIABLE
#define IS_OP(node) TreeGetNodeType(node) == TYPE_OPERATOR

#define DL Derivate(TreeGetLeftNode(node))
#define DR Derivate(TreeGetRightNode(node))
#define CL TreeCloneNode(TreeGetLeftNode(node))
#define CR TreeCloneNode(TreeGetRightNode(node))

#define PLUS(left, right)  TreeCreateNode(TYPE_OPERATOR, OPERATOR_PLUS,  left, right, NULL)
#define MINUS(left, right) TreeCreateNode(TYPE_OPERATOR, OPERATOR_MINUS, left, right, NULL)
#define MULT(left, right)  TreeCreateNode(TYPE_OPERATOR, OPERATOR_MULT,  left, right, NULL)
#define DIV(left, right)   TreeCreateNode(TYPE_OPERATOR, OPERATOR_DIV,   left, right, NULL)
#define NUM(arg)           TreeCreateNode(TYPE_NUMBER,   arg,            NULL, NULL,  NULL)
#define EXP(node)          TreeCreateNode(TYPE_OPERATOR, OPERATOR_EXP,   CL,   NULL,  NULL)
#define POW(left, right)   TreeCreateNode(TYPE_OPERATOR, OPERATOR_POW,   left, right, NULL)
#define LN(node)           TreeCreateNode(TYPE_OPERATOR, OPERATOR_LN,    CL,   NULL,  NULL)
#define SIN(node)          TreeCreateNode(TYPE_OPERATOR, OPERATOR_SIN,   CL,   NULL,  NULL)
#define COS(node)          TreeCreateNode(TYPE_OPERATOR, OPERATOR_COS,   CL,   NULL,  NULL)

enum TYPES
{
    #define DEF_TYPE(str) TYPE_##str,
    TYPE_NULL = 0,
    #include"types.h"
    #undef DEF_TYPE
};

enum VARIABLES
{
    #define VARIABLE(str) VARIABLE_##str,
    VARIABLE_NULL = 0,
    #include"variables.h"
    #undef VARIABLE
};

enum OPERATORS
{
    #define OPERATOR(str, name, code, print) OPERATOR_##name,
    OPERATOR_NULL = 0,
    #include"operators.h"
    #undef OPERATOR
};

int PrintTreeTex(tree_node_t* node, FILE* output);
int PrintTexInit(FILE* output);
int PrintTexEnd(FILE* output);
int GetTexPdf();
int PrintTreeTexRec(tree_node_t* node, FILE* output);
int PrintNodeOperator(tree_node_t* node, int method, FILE* output);
int IsConst(tree_node_t* node);
tree_value_t CountRec(tree_node_t* node);


int DerivateGetOperatorInt(char* data);
char* DerivateGetOperatorStr(double data);
int DerivateGetVariableInt(char* data);
char* DerivateGetVariableStr(double data);
tree_node_t* Derivate(tree_node_t* node);

#endif
