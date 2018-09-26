#include"cmdcodes.h"
#include"stack.h"
#include<ctype.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<assert.h>

#define CMD_MAX_LENGHT 30
#define LABEL_MAX_COUNT 100
#define CMD_LABEL "LABEL"
#define MAX_FILE_NAME 1000
#define NO_COMMAND_ERROR() \
    else printf("cant find command: %d\n", com); \
    abort();

#define DEF_CMD(name, code1, code2) \
    else if(strcmp(str, #name) == 0) \
    { \
        code2 \
    }

typedef struct label_t label_t;

struct label_t
{
    char str[CMD_MAX_LENGHT];
    int num_cmd;
};

int compiler_print_reg(char* str, FILE* compiled)
{
    #define REG(name) \
    else if(strcmp(str, #name) == 0) \
    { \
        fprintf(compiled, "%d ", REG_##name); \
        return 1; \
    }

    if(0) {}
    #include"regs.h"
    else
        return 0;
    #undef REG
}

int compiler(FILE* code, FILE* compiled)
{
    char str[CMD_MAX_LENGHT] = {};
    label_t label[LABEL_MAX_COUNT] = {};
    int label_num = 0, cmd_count = 0, label_count = 0, tmp = 0;
    stack_data_t val = 0;

    fprintf(compiled, "%d %d ", CMD_VER, ASM_VER); //version
    cmd_count = 2;

    for(;;) //labels check
    {
        if(fscanf(code, "%s", str) == EOF)
            break;
        if(strcmp(str, CMD_LABEL) == 0)
        {
            fscanf(code, "%s", label[label_num].str);
            label[label_num].num_cmd = cmd_count;
            label_num++;
        }
        else
            cmd_count++;
    }
    fseek(code, 0, SEEK_SET);

    while(1)
    {
        if(fscanf(code ,"%s", str) == EOF)
            break;
        //printf("\ncur: %s", str);
        if(0);
        #include"commands.h"
    }
}

int main()
{
    char file_name[MAX_FILE_NAME] = "./cpu_workspace/cpucode.txt";

    FILE* code = fopen(file_name, "r");
    if(code == NULL)
    {
        printf("\nerror: can't find ./cpu_workspace/cpucode.txt\n");
        return 0;
    }

    FILE* compiled = fopen("./cpu_workspace/execode.txt", "w");

    compiler(code, compiled);

    fclose(code);
    fclose(compiled);
    return 0;
}

