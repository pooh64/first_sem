#ifndef DEF_TREE
#define DEF_TREE

typedef double tree_value_t;
typedef struct tree_t tree_t;
typedef struct tree_node_t tree_node_t;
typedef int tree_datatype_t;

struct tree_node_t
{
    tree_node_t* left;
    tree_node_t* right;
    tree_node_t* parent;
    tree_value_t data;
    tree_datatype_t type;
};

struct tree_t
{
    tree_node_t* root;
};

enum TREE_ERROR_LIST
{
    ERROR_TREE_ADD_NODE_LEFT = 1,
    ERROR_TREE_ADD_NODE_RIGHT,
    ERROR_TREE_DELETE_NODE,
};

int TreeConstruct(tree_t* tree);
int TreeDestruct(tree_t* tree);
tree_node_t* TreeGetRoot(tree_t* tree);
tree_node_t* TreeGetLeftNode(tree_node_t* node);
tree_node_t* TreeGetRightNode(tree_node_t* node);
int TreeAddNodeLeft(tree_node_t* node);
int TreeAddNodeRight(tree_node_t* node);
tree_value_t TreeGetNodeData(tree_node_t* node);
int TreeSetNodeData(tree_node_t* node, tree_value_t data);
tree_datatype_t TreeGetNodeType(tree_node_t* node);
int TreeSetNodeType(tree_node_t* node, tree_datatype_t data);
int TreeDeleteNodeUnder(tree_node_t* node);
int TreeDeleteNodeLeft(tree_node_t* node);
int TreeDeleteNodeRight(tree_node_t* node);
int TreeDotDump(tree_node_t* node);
tree_node_t* TreeFindNode(tree_t* tree, tree_value_t* data);
int TreePrint(tree_node_t* node, FILE* output, int ntab);
tree_t* TreeRead(FILE* input);
int TreePrintNode(tree_node_t* node, FILE* output);
int TreeSetNodeLeft(tree_node_t* node, tree_node_t* newnode);
int TreeSetNodeRight(tree_node_t* node, tree_node_t* newnode);
int TreeSetNodeParent(tree_node_t* node, tree_node_t* newnode);
tree_node_t* TreeCreateNode(int type, double data, tree_node_t* left, tree_node_t* right, tree_node_t* parent);
tree_node_t* TreeCloneNode(tree_node_t* source);
int TreeDeleteNodeRec(tree_node_t* node);

#endif
