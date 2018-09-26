typedef char tree_value_t;
typedef struct tree_t tree_t;
typedef struct tree_node_t tree_node_t;
struct tree_node_t
{
    tree_node_t* left;
    tree_node_t* right;
    tree_node_t* parent;
    tree_value_t* data;
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
tree_value_t* TreeGetNodeData(tree_node_t* node);
int TreeSetNodeData(tree_node_t* node, tree_value_t* data);
int TreeDeleteNodeUnder(tree_node_t* node);
int TreeDeleteNodeLeft(tree_node_t* node);
int TreeDeleteNodeRight(tree_node_t* node);
int TreeDotDump(tree_t* tree);
tree_node_t* TreeFindNode(tree_t* tree, tree_value_t* data);
