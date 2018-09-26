#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<assert.h>
#include"stack.h"

typedef double stack_data_t;

#define STK_START_SIZE 30
#define STK_RESIZE_MULTIPLY 2

#define STACK_OVERFLOW_MESSAGE "stack overflow\n"
#define STACK_UNDERFLOW_MESSAGE "stack underflow\n"
#define STACK_OUT_MEM_MESSAGE "out of memory\n"
#define STACK_DEAD_CANARY_MESSAGE "don't TOUCH MY CANARY !!!11!1\n"
#define STACK_CANT_RESIZE_MESSAGE "resize failed\n"

#define DEBUG
#ifdef DEBUG
#define HASH_CHECK() \
do { \
    if(stack -> hash != stack_hash(stack)) { \
        printf("hash failed: %d\nfile: %s \nfunction: %s() \n", STACK_HASH_ERROR_CODE, __FILE__, __func__); \
        stack_dump(stack); \
        abort(); \
    } \
} \
while (0)

#define HASH_REFRESH() \
do { \
    stack_hash(stack); \
    } \
while (0)

#define STACK_OK() \
do { \
    int error_c;\
    if((error_c = stack_ok(stack)) != 0) \
    { \
        printf("error: %d\nfile: %s \nfunction: %s() \n", error_c, __FILE__, __func__); \
        stack_dump(stack); \
        abort(); \
    } \
} while (0)
#else
#define STACK_OK() \
do { \
    int error_c;\
    if((error_c = stack_ok(stack)) != 0) \
    { \
        return error_c; \
    } \
} while (0)
#define HASH_REFRESH()
#define HASH_CHECK()
#endif

#include"stack.h"

int stack_ok(stack_t* stack)
{
    if(stack == NULL)
    {
        return STACK_NULL_POINTER_CODE;
    }
    if(stack -> count > stack -> size)
    {
        return STACK_OVERFLOW_CODE;
    }

    if(stack -> count < 0)
    {
        return STACK_UNDERFLOW_CODE;
    }

    if(stack -> data == NULL)
    {
        return STACK_OUT_OF_MEM_CODE;
    }

    if(stack -> canary_beg != stack -> canary_end)
    {
        return STACK_DEAD_CANARY_CODE;
    }
    return 0;
}

int stack_dump(stack_t* stack)
{
    printf("-------dump-------\n");
    printf("size: %d\n", stack -> size);
    printf("count: %d\n", stack -> count);
    for(int num = 0; num < stack -> size; num++)
        printf("stack.data[%d] = %lg\n", num, *(stack -> data +num));
    printf("-------/dump-------\n");
    return 0;
}

int stack_prepare(stack_t* stack)
{
    assert(stack);
    srand(time(NULL));
    stack -> canary_beg = rand() + 1;
    stack -> canary_end = stack -> canary_beg;
    stack -> size = STK_START_SIZE;
    stack -> count = 0;
    stack -> data = (stack_data_t*) calloc(STK_START_SIZE, sizeof(stack_data_t));
    HASH_REFRESH();
    STACK_OK();
    return 0;
}

stack_t* stack_construct()
{
    stack_t* stack = (stack_t*) calloc(1, sizeof(stack_t));
    assert(stack);
    stack_prepare(stack);
    return stack;
}

int stack_destruct(stack_t* stack)
{
    STACK_OK();
    HASH_CHECK();
    free(stack);
    return 0;
}

int stack_resize(stack_t* stack)
{
    STACK_OK();
    HASH_CHECK();
    stack -> size = stack -> size * STK_RESIZE_MULTIPLY;
    stack_data_t *tmp = (stack_data_t*) realloc(stack -> data, stack -> size * sizeof(stack_data_t));
    if (tmp == NULL)
    {
        printf(STACK_CANT_RESIZE_MESSAGE);
        return STACK_CANT_RESIZE_CODE;
    }
    stack -> data = tmp;
    HASH_REFRESH();
    STACK_OK();
    return 0;
}

stack_data_t stack_pop(stack_t* stack)
{
    STACK_OK();
    HASH_CHECK();
    stack -> count--;
    STACK_OK();
    HASH_REFRESH();
    return *(stack -> data + stack -> count);
}

int stack_push(stack_t* stack, stack_data_t val)
{
    STACK_OK();
    HASH_CHECK();
    if(stack -> count == stack -> size)
        stack_resize(stack);
    *(stack -> data + stack -> count) = val;
    stack -> count++;
    STACK_OK();
    HASH_REFRESH();
    return 0;
}

stack_data_t stack_hash(stack_t* stack)
{
    stack -> hash = 0;
    char* beg = (char*) &(stack -> canary_beg);
    char* end = (char*) &(stack -> canary_end);
    for(char* ptr = beg; ptr <= end; ptr++)
        stack -> hash = stack -> hash + *(ptr);
    return stack -> hash;
}
