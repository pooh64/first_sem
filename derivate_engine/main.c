#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include"tree.h"
#include"diff.h"


FILE* tex = NULL;

int GetFileSize(FILE* input)
{
    assert(input);
    int cur = ftell(input);
    fseek(input, 0, SEEK_END);
    int size = ftell(input);
    fseek(input, 0, cur);
    return size;
}

int TestDumpNode(tree_node_t* node)
{
    assert(node);
    printf("\nnode: %p\n", node);
    printf("left: %p\n", node->left);
    printf("right: %p\n", node -> right);
    printf("data: %lg\n\n", node -> data);
    return 0;
}

int main()
{
    srand(time(NULL));
    FILE* input = fopen("input.txt", "r");
    FILE* output = fopen("output.txt", "w");
    tex = fopen(REPORT_FILE, "w");
    tree_t* tree = TreeRead(input);

    PrintTexInit(tex);
    fprintf(tex, "\\begin{center}Science article number %d\\\\Author: github.com/pooh64/Derivate\\_Engine \
    \n\nAim: Find derivate of:\\end{center}", rand() % 9000 + 1000);
    PrintTreeTex(TreeGetRoot(tree), tex);

    tree_node_t* node = Derivate(TreeGetRoot(tree));
    Simplify(node, 10);

    fprintf(tex, "\nfinally:\n\n");
    PrintTreeTex(node, tex);
    fprintf(tex, "\nList of used literature:\n\n The C Programming Language K\\&R\n\nMIPT 2017");
    PrintTexEnd(tex);
    fclose(tex);
    GetTexPdf();

    TreePrint(node, output, 0);
    TreeDestruct(tree);
    TreeDeleteNodeRec(node);

    return 0;
}
