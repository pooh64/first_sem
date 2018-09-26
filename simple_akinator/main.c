#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include"tree.h"

#define MAXDATA 150
#define MAXDEPTH 1000

//#define WITH_VOICE
#ifdef WITH_VOICE
#define SOUND(code) code;
#else
#define SOUND(code)
#endif

int GetFileSize(FILE* input)
{
    int cur = ftell(input);

    fseek(input, 0, SEEK_END);
    int size = ftell(input);
    fseek(input, 0, cur);

    return size;
}

int TreePrint(tree_node_t* node, FILE* output, int ntab)
{
    int i = 0;
    for(i = 0; i < ntab; i++)
        fprintf(output, "\t");
    fprintf(output, "(\"%s\"\n", TreeGetNodeData(node));
    if(TreeGetLeftNode(node) != NULL)
        TreePrint(TreeGetLeftNode(node), output, ntab + 1);
    if(TreeGetRightNode(node) != NULL)
        TreePrint(TreeGetRightNode(node), output, ntab + 1);
    for(i = 0; i < ntab; i++)
        fprintf(output, "\t");
    fprintf(output, ")\n");
    return 0;
}

char* TreeReadRec(tree_node_t* node, char* text)
{
    char* data = 0;
    while(*text != '\"')
        text++;
    text++;
    data = text;
    while(*text != '\"')
        text++;
    *text = '\0';
    TreeSetNodeData(node, data);
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
    return text ;
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
    free(text);
    return tree;
}

int GenerateSound(char* str)
{
    char newstr[MAXDATA + 25] = {};
    strcat(newstr, "espeak \"");
    strcat(newstr, str);
    strcat(newstr, "\" -v en+m1 -p 29 -s 110");
    system(newstr);
    return 0;
}

tree_node_t* AkinatorGetResult(tree_node_t* node)
{
    char c = 0;
    int quit = 0;
    while(TreeGetLeftNode(node) != NULL)
    {
        printf("%s\n", TreeGetNodeData(node));
        SOUND(GenerateSound(TreeGetNodeData(node)))
        quit = 0;
        while(quit == 0)
        {
            scanf("%s", &c);
            quit = 1;
            switch(c)
            {
                case 'y':
                    node = TreeGetRightNode(node);
                    break;
                case 'n':
                    node = TreeGetLeftNode(node);
                    break;
                default:
                    quit = 0;
                    break;
            }
        }
    }
    return node;
}

int Akinator(tree_t* tree)
{
    tree_node_t* node = TreeGetRoot(tree);
    char newdata[MAXDATA] = {};
    char c = 0;
    int len = 0;
    node = AkinatorGetResult(node);
    printf("This is %s?\n", TreeGetNodeData(node));
    SOUND(char str[MAXDATA * 2 + 100] = {})
    SOUND(strcat(str, "This is "))
    SOUND(strcat(str, TreeGetNodeData(node)))
    SOUND(GenerateSound(str));
    scanf("%s", newdata);
    c = *newdata;
    switch(c)
    {
        case 'y' :
            printf("HA HA HA, I won!!!\n");
            SOUND(GenerateSound("HA HA HA, I won"))
            break;
        case 'n' :
            TreeAddNodeLeft(node);
            TreeAddNodeRight(node);
            printf("Who is it?\n");
            SOUND(GenerateSound("Who is it?"))

            getchar(); //nice way to read
            fgets(newdata, MAXDATA, stdin);
            newdata[strlen(newdata) - 1] = '\0';

            TreeSetNodeData(TreeGetRightNode(node), newdata);
            TreeSetNodeData(TreeGetLeftNode(node), TreeGetNodeData(node));
            printf("What is the difference between %s and %s?\n%s is ", newdata, TreeGetNodeData(node), newdata);
            SOUND(*str = '\0')
            SOUND(strcat(str, "What is the difference between "))
            SOUND(strcat(str, newdata))
            SOUND(strcat(str, "and"))
            SOUND(strcat(str, TreeGetNodeData(node)))
            SOUND(strcat(str, " "))
            SOUND(GenerateSound(str))
            SOUND(*str = '\0')
            SOUND(strcat(str, newdata))
            SOUND(strcat(str, " is"))
            SOUND(GenerateSound(str))

            fgets(newdata, MAXDATA, stdin);
            newdata[strlen(newdata) - 1] = '?';

            TreeSetNodeData(node, newdata);
            printf("Congratulations! I will destroy your world!!!\n");
            SOUND(GenerateSound("Congratulations! I will destroy your world!"));
            break;
        default:
            printf("I don't understand\n");
            SOUND(GenerateSound("I don't understand"))
            break;
    }
    return 0;
}

int Describe(tree_t* tree, char* newdata)
{
    tree_node_t* node = TreeFindNode(tree, newdata);
    if(node == NULL)
        return 1;
    printf("%s:\n", TreeGetNodeData(node));
    SOUND(GenerateSound(TreeGetNodeData(node)))
    SOUND(char str[MAXDATA + 10])
    char data[MAXDATA] = {};
    while(TreeGetParent(node) != NULL)
    {
        SOUND(*str = '\0';)
        if(TreeGetLeftNode(TreeGetParent(node)) == node)
        {
            printf("is not ");
            SOUND(strcat(str, "is not"))
        }
        else
        {
            printf("is ");
            SOUND(strcat(str, "is "))
        }
        node = TreeGetParent(node);
        strcpy(data, TreeGetNodeData(node));
        data[strlen(data) - 1] = '\0';
        printf("%s\n", data);
        SOUND(strcat(str, data))
        SOUND(GenerateSound(str))
    }
    return 0;
}

int Compare(tree_t* tree, char* data1, char* data2)
{
    int i = 0, n = 0, len1 = 0, len2 = 0;
    tree_node_t* nodeway1[MAXDEPTH] = {};
    tree_node_t* nodeway2[MAXDEPTH] = {};
    tree_node_t* node1 = TreeFindNode(tree, data1);
    tree_node_t* node2 = TreeFindNode(tree, data2);
    if((node1 == NULL) || (node2 == NULL))
        return 1;
    *nodeway1 = node1;
    *nodeway2 = node2;
    for(i = 1; ; i++)
    {
        nodeway1[i] = TreeGetParent(nodeway1[i - 1]);
        if(nodeway1[i] == NULL)
        {
            len1 = i;
            break;
        }
    }

    for(i = 1; ; i++)
    {
        nodeway2[i] = TreeGetParent(nodeway2[i - 1]);
        if(nodeway2[i] == NULL)
        {
            len2 = i;
            break;
        }
    }

    for(i = 0; ;i++)
    {
        if(nodeway1[len1 - i] != nodeway2[len2 - i])
            break;
    }

    printf("%s:\n", data1);
    SOUND(GenerateSound(data1))

    for(n = 1; n < len1 - i + 2; n++)
    {
        if(nodeway1[n - 1] == TreeGetLeftNode(nodeway1[n]))
        {
            printf("is not ");
            SOUND(GenerateSound("is not"))
        }
        else
        {
            printf("is ");
            SOUND(GenerateSound("is"))
        }
        printf("%s\n", TreeGetNodeData(nodeway1[n]));
        SOUND(GenerateSound(TreeGetNodeData(nodeway1[n])))
    }

    printf("%s:\n", data2);
    SOUND(GenerateSound(data2))

    for(n = 1; n < len2 - i + 2; n++)
    {
        if(nodeway2[n - 1] == TreeGetLeftNode(nodeway2[n]))
        {
            printf("is not ");
            SOUND(GenerateSound("is not"))
        }
        else
        {
            printf("is ");
            SOUND(GenerateSound("is"))
        }
        printf("%s\n", TreeGetNodeData(nodeway2[n]));
        SOUND(GenerateSound(TreeGetNodeData(nodeway2[n])))
    }
    return 0;
}

int PlayAkinator(tree_t* tree)
{
    SOUND(GenerateSound("Hello human race!"))
    char c = 0;
    int quit = 0;
    char newdata[MAXDATA + 100] = {};
    char tmp[MAXDATA + 100] = {};
    while(quit == 0)
    {
        scanf("%s", newdata);
        c = *newdata;
        switch(c)
        {
            case 'd': //describe
                getchar();
                fgets(newdata, MAXDATA, stdin);
                newdata[strlen(newdata) - 1] = '\0';
                if(Describe(tree, newdata))
                {
                    printf("I don't know what is it\n");
                    SOUND(GenerateSound("I don't know what is it"))
                }
                break;

            case 'g': //guess
                Akinator(tree);
                break;

            case 'q': //quit
                quit = 1;
                break;

            case 'c':
                getchar();
                fgets(newdata, MAXDATA, stdin);
                newdata[strlen(newdata) - 1] = '\0';
                fgets(tmp, MAXDATA, stdin);
                tmp[strlen(tmp) - 1] = '\0';
                printf("here\n");
                if(Compare(tree, newdata, tmp))
                {
                    printf("I don't know\n");
                    SOUND(GenerateSound("I don't know"));
                }
                break;

            case 't':
                TreeDotDump(tree);
                break;
            default :
                printf("I don't understand\n");
                SOUND(GenerateSound("I don't understand"))
                break;
        }
    }
}

int main()
{
    //read from database
    FILE* input = fopen("output.txt", "r");
    tree_t* tree = TreeRead(input);
    fclose(input);

    PlayAkinator(tree);

    //save
    input = fopen("output.txt", "w");
    TreePrint(TreeGetRoot(tree), input, 0);
    fclose(input);
    TreeDestruct(tree);
    return 0;
}
