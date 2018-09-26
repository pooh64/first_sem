#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include"tree.h"
int TreeConstruct(tree_t* tree)
{
    assert(tree);
    tree_node_t* newroot = (tree_node_t*) calloc(1, sizeof(tree_node_t));
    assert(newroot);
    tree -> root = newroot;
    tree -> root -> left = NULL;
    tree -> root -> right = NULL;
    tree -> root -> parent = NULL;
    tree -> root -> data = NULL;
    return 0;
}

tree_node_t* TreeGetRoot(tree_t* tree)
{
    assert(tree);
    return tree -> root;
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
    node -> left -> data = NULL;
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
    node -> right -> data = NULL;
    return 0;
}

tree_value_t* TreeGetNodeData(tree_node_t* node)
{
    assert(node);
    return node -> data;
}

int TreeSetNodeData(tree_node_t* node, tree_value_t* data)
{
    assert(node);
    int len = strlen(data);
    tree_value_t* newdata = (tree_value_t*) calloc(len + 1, sizeof(tree_value_t));
    strcpy(newdata, data);
    node -> data = newdata;
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
    free(node -> data);
    free(node);
    return 0;
}

int TreeDotDump(tree_t* tree)
{
    assert(tree);
    FILE* dot = fopen("dumptext.dot", "w");
    fprintf(dot, "digraph dump {\n    node [shape = record];\n");
    TreeDotDumpRec(tree -> root, dot);
    fprintf(dot, "}");
    fclose(dot);
    system("dot -Tps dumptext.dot -o dumpimg.ps");
    system("xdg-open dumpimg.ps");
    return 0;
}

int TreeDotDumpRec(tree_node_t* node, FILE* output)
{
    assert(node);
    fprintf(output, "%d [shape = record, label = \"{%s}\"];\n", node, node -> data);
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
