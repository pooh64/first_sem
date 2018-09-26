#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include"tree.h"
#include"diff.h"

int TreeConstruct(tree_t* tree)
{
    assert(tree);
    tree_node_t* newroot = (tree_node_t*) calloc(1, sizeof(tree_node_t));
    tree -> root = newroot;
    tree -> root -> left = NULL;
    tree -> root -> right = NULL;
    tree -> root -> parent = NULL;
    tree -> root -> data = 0;
    tree -> root -> type = 0;
    return 0;
}

tree_node_t* TreeGetRoot(tree_t* tree)
{
    assert(tree);
    return tree -> root;
}

int TreeSetNodeLeft(tree_node_t* node, tree_node_t* newnode)
{
    assert(node);
    node -> left = newnode;
    return 0;
}

int TreeSetNodeRight(tree_node_t* node, tree_node_t* newnode)
{
    assert(node);
    node -> right = newnode;
    return 0;
}

int TreeSetNodeParent(tree_node_t* node, tree_node_t* newnode)
{
    assert(node);
    node -> parent = newnode;
    return 0;
}

int TreeCopyNode(tree_node_t* node, tree_node_t* source)
{
    assert(node);
    assert(source);
    node -> data = source -> data;
    node -> left = source -> left;
    node -> right = source -> right;
    node -> type = source -> type;
    node -> parent = source -> parent;
    return 0;
}

tree_node_t* TreeCloneNode(tree_node_t* source)
{
    assert(source);
    tree_node_t* node = (tree_node_t*) calloc(1, sizeof(tree_node_t));
    assert(node);
    node -> data = source -> data;
    node -> type = source -> type;
    node -> parent = source -> parent;

    if(source -> left != NULL)
        node -> left = TreeCloneNode(source -> left);
    else
        node -> left = NULL;

    if(source -> right != NULL)
        node -> right = TreeCloneNode(source -> right);
    else
        node -> right = NULL;

    return node;
}

int TreeConstructNode(tree_node_t* node)
{
    assert(node);
    node -> parent = NULL;
    node -> left = NULL;
    node -> right = NULL;
    node -> data = 0;
    node -> type = 0;
    return 0;
}

tree_node_t* TreeCreateNode(int type, double data, tree_node_t* left, tree_node_t* right, tree_node_t* parent)
{
    tree_node_t* node = calloc(1, sizeof(tree_node_t));
    node -> left = left;
    node -> right = right;
    node -> parent = parent;
    node -> type = type;
    node -> data = data;
    return node;
}

tree_node_t* TreeGetLeftNode(tree_node_t* node)
{
    assert(node);
    return node -> left;
}

tree_node_t* TreeGetRightNode(tree_node_t* node)
{
    assert(node);
    return node -> right;
}

tree_node_t* TreeGetParent(tree_node_t* node)
{
    assert(node);
    return node -> parent;
}

int TreeAddNodeLeft(tree_node_t* node)
{
    assert(node);
    if(node -> left != NULL)
        return ERROR_TREE_ADD_NODE_LEFT;
    node -> left  = (tree_node_t*) calloc(1, sizeof(tree_node_t));
    node -> left -> parent = node;
    node -> left -> left = NULL;
    node -> left -> right = NULL;
    node -> left -> data = 0;
    node -> left -> type = 0;
    return 0;
}

int TreeAddNodeRight(tree_node_t* node)
{
    assert(node);
    if(node -> right != NULL)
        return ERROR_TREE_ADD_NODE_RIGHT;
    node -> right  = (tree_node_t*) calloc(1, sizeof(tree_node_t));
    node -> right -> parent = node;
    node -> right -> left = NULL;
    node -> right -> right = NULL;
    node -> right -> data = 0;
    node -> right -> type = 0;
    return 0;
}

tree_value_t TreeGetNodeData(tree_node_t* node)
{
    assert(node);
    return node -> data;
}

tree_datatype_t TreeGetNodeType(tree_node_t* node)
{
    assert(node);
    return node -> type;
}

int TreeSetNodeData(tree_node_t* node, tree_value_t data)
{
    assert(node);
    node -> data = data;
    return 0;
}

int TreeSetNodeType(tree_node_t* node, tree_datatype_t type)
{
    assert(node);
    node -> type = type;
    return 0;
}

int TreeDeleteNodeUnder(tree_node_t* node)
{
    if(node -> left  != NULL)
        TreeDeleteNodeRec(node -> left);
    if(node -> right != NULL)
        TreeDeleteNodeRec(node -> right);
    node -> left = NULL;
    node -> right = NULL;
    return 0;
}

int TreeDeleteNodeLeft(tree_node_t* node)
{
    assert(node);
    if(node -> left == NULL)
        return ERROR_TREE_DELETE_NODE;
    TreeDeleteNodeRec(node -> left);
    node -> left = NULL;
    return 0;
}

int TreeDeleteNodeRight(tree_node_t* node)
{
    assert(node);
    if(node -> right == NULL)
        return ERROR_TREE_DELETE_NODE;
    TreeDeleteNodeRec(node -> right);
    node -> right = NULL;
    return 0;
}

int TreeDeleteNodeRec(tree_node_t* node)
{
    assert(node);
    if(node -> left  != NULL)
        TreeDeleteNodeRec(node -> left);
    if(node -> right != NULL)
        TreeDeleteNodeRec(node -> right);
    //free(node -> data);
    free(node);
    return 0;
}

int TreeDotDump(tree_node_t* node)
{
    assert(node);
    FILE* dot = fopen("dumptext.dot", "w");
    fprintf(dot, "digraph dump {\n    node [shape = record];\n");
    TreeDotDumpRec(node, dot);
    fprintf(dot, "}");
    fclose(dot);
    system("dot -Tps dumptext.dot -o dumpimg.ps");
    system("xdg-open dumpimg.ps");
    return 0;
}

int TreeDotDumpRec(tree_node_t* node, FILE* output)
{
    assert(node);
    if(node -> type == TYPE_NUMBER)
        fprintf(output, "%d [shape = record, label = \"{data: %lg|ptr: %p|type: %d|left: %p|right: %p|parent: %p}\"];\n", \
                node, node -> data, node, node -> type, node -> left, node -> right, node -> parent);
    if(node -> type == TYPE_VARIABLE)
        fprintf(output, "%d [shape = record, label = \"{data: %s|ptr: %p|type: %d|left: %p|right: %p|parent: %p}\"];\n", \
                node, DerivateGetVariableStr(node -> data), node, node -> type, node -> left, node -> right, node -> parent);
    if(node -> type == TYPE_OPERATOR)
        fprintf(output, "%d [shape = record, label = \"{data: %s|ptr: %p|type: %d|left: %p|right: %p|parent: %p}\"];\n", \
                node, DerivateGetOperatorStr(node -> data), node, node -> type, node -> left, node -> right, node -> parent);
    if(node -> left != NULL)
    {
        fprintf(output, "%d -> %d\n", node, node -> left);
        TreeDotDumpRec(node -> left, output);
    }
    if(node -> right != NULL)
    {
        TreeDotDumpRec(node -> right, output);
        fprintf(output, "%d -> %d\n", node, node -> right);
    }
    return 0;
}

int TreeDestruct(tree_t* tree)
{
    assert(tree);
    TreeDeleteNodeRec(tree -> root);
    free(tree);
    return 0;
}

/*  next versions
tree_node_t* TreeFindNodeRec(tree_node_t* node, tree_value_t* data)
{
    if(strcmp(node -> data, data) == 0)
        return node;

    if(node -> left == NULL)
    {
        if(node -> parent -> right != node)
        {
            node = node -> parent -> right;
            node = TreeFindNodeRec(node, data);
            return node;
        }
        else
        {
            while((node -> parent != NULL) && (node -> parent -> right == node))
                node = node -> parent;
            if(node -> parent == NULL)
            {
                node = NULL;
                return node;
            }
            node = node -> parent -> right;
            node = TreeFindNodeRec(node, data);
            return node;
        }
    }

    node = node -> left;
    node = TreeFindNodeRec(node, data);
        return node;
}

tree_node_t* TreeFindNode(tree_t* tree, tree_value_t* data)
{
    tree_node_t* node = TreeGetRoot(tree);
    node = TreeFindNodeRec(node, data);
    return node;
}
*/

int TreePrintNode(tree_node_t* node, FILE* output)
{
    if(TreeGetNodeType(node) == TYPE_NUMBER)
        fprintf(output, "(\"%lg\"\n", TreeGetNodeData(node));
    if(TreeGetNodeType(node) == TYPE_VARIABLE)
        fprintf(output, "(\"x\"\n"); //only x
    if(TreeGetNodeType(node) == TYPE_OPERATOR)
        fprintf(output, "(\"%s\"\n", DerivateGetOperatorStr(TreeGetNodeData(node)));
    return 0;
}

int TreePrint(tree_node_t* node, FILE* output, int ntab)
{
    int i = 0;
    for(i = 0; i < ntab; i++)
        fprintf(output, "\t");
    //fprintf(output, "(\"%s\"\n", TreeGetNodeData(node));
    TreePrintNode(node, output);
    if(TreeGetLeftNode(node) != NULL)
        TreePrint(TreeGetLeftNode(node), output, ntab + 1);
    if(TreeGetRightNode(node) != NULL)
        TreePrint(TreeGetRightNode(node), output, ntab + 1);
    for(i = 0; i < ntab; i++)
        fprintf(output, "\t");
    fprintf(output, ")\n");
    return 0;
}

char* TreeReadRec(tree_node_t* node, char* text) //input with "7" "x" "sin"...
{
    char* data = 0;
    while(*text != '\"')
        text++;
    text++;
    data = text;
    while(*text != '\"')
        text++;
    *text = '\0';

    if(isdigit(*data)) //number
    {
        TreeSetNodeData(node, atof(data));
        TreeSetNodeType(node, TYPE_NUMBER);
    }
    else if(DerivateGetOperatorInt(data)) //operator
    {
        TreeSetNodeData(node, DerivateGetOperatorInt(data));
        TreeSetNodeType(node, TYPE_OPERATOR);
    }
    else if(DerivateGetVariableInt(data)) //variable
    {
        TreeSetNodeData(node, DerivateGetVariableInt(data)); //re
        TreeSetNodeType(node, TYPE_VARIABLE);
    }
    else
    {
        printf("\nRead failed\n\n");
        return 0;
    }

    for(;;text++)
    {
        if(*text == '(')
        {
            TreeAddNodeLeft(node);
            TreeAddNodeRight(node);
            text = TreeReadRec(node -> left, text + 1);
            break;
        }
        if(*text == ')')
            return text + 1;
    }

    for(;;text++)
    {
        if(*text == '(')
        {
            text = TreeReadRec(node -> right, text + 1);
            break;
        }
    }
    return text;
}

tree_t* TreeRead(FILE* input)
{
    tree_t* tree = (tree_t*) calloc(1, sizeof(tree_t));
    TreeConstruct(tree);   // create first node
    int fsize = GetFileSize(input);
    char* text = (char*) calloc(fsize, sizeof(char));
    fread(text, 1, fsize, input);
    while(*text != '(')
        text++;
    TreeReadRec(TreeGetRoot(tree), text + 1);
    return tree;
}

