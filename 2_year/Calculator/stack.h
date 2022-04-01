#ifndef STACK
#define STACK

#include <stddef.h>

#define INIT_SIZE 1024
#define MEM_EXH 3
#define MEM_OVER 4

typedef struct Stack
{
    size_t curr_size;
    size_t act_size;
    void *data;
} Stack;

int init_stack(Stack *stack);

void delete_stack(Stack *stack);

int pop_stack(Stack *stack, void *res, size_t size_res);

int push_stack(Stack *stack, void *res, size_t size_res);

#endif
