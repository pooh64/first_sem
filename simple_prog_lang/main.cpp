#include <stdio.h>
#include <assert.h>
#include "tree.h"
#include "reader.h"

#include "translate.h"

#include "grammar.h"
#include "stdlib.h"

int get_file_size(FILE* input)
{
    assert(input);
    int cur = ftell(input);
    fseek(input, 0, SEEK_END);
    int size = ftell(input);
    fseek(input, 0, cur);
    return size;
}

int list_dump(list_t *list)
{
    printf("___list dump____\n");
    while(list != NULL)
    {
        printf("data: %4lg   type: %d\n", list->data, list->type);
        list = list -> next;
    }
    printf("____/dump_____\n");
    return 0;
}

int main()
{
    FILE *file = fopen("./cpu_workspace/code.txt", "r");
    int size = get_file_size(file);
    char *text = (char*) calloc(size + 1, sizeof(char));
    fread(text, size + 1, sizeof(char), file);
    list_t *list = r_go(text);

    //list_dump(list);

    node_t *node = g_get_go(list);
    //node_dot_dump(node);

    FILE *out = fopen("./cpu_workspace/cpucode.txt", "w");
    translate_start(node, out);
    fclose(out);
    system("./cpuasm");
    printf("\n\n------------\nRun:\n");
    system("./Stack_CPU_emulator");
    printf("\n");

    node_destruct_rec(node);
    list_destruct_rec(list);
    free(text);

    return 0;
}
