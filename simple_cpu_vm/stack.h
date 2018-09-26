#ifndef _STACK_H_
#define _STACK_H_

enum STACK_ERROR_CODE
{
    STACK_OVERFLOW_CODE = 1,
    STACK_UNDERFLOW_CODE,
    STACK_OUT_OF_MEM_CODE,
    STACK_DEAD_CANARY_CODE,
    STACK_CANT_RESIZE_CODE,
    STACK_HASH_ERROR_CODE,
    STACK_NULL_POINTER_CODE,
};

typedef double stack_data_t;

struct stack_t
{
    int canary_beg;
    int size;
    int count;
    stack_data_t* data;
    char hash;
    int canary_end;
};

typedef struct stack_t stack_t;

int stack_ok(stack_t* stack);
int stack_dump(stack_t* stack);
stack_t* stack_construct();
int stack_prepare(stack_t* stack);
int stack_destruct(stack_t* stack);
int stack_resize(stack_t* stack);
stack_data_t stack_pop(stack_t* stack);
int stack_push(stack_t* stack, stack_data_t data);
stack_data_t stack_hash(stack_t* stack);

#endif // _STACK_H_
