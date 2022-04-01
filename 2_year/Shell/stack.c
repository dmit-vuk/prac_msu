#include <stdlib.h>
#include "stack.h"

int
init_stack(Stack *stack)
{
    stack->curr_size = 0;
    stack->act_size = INIT_SIZE;
    stack->data = (char*) malloc(sizeof(char) * stack->act_size);
    return 0;
}

void
delete_stack(Stack *stack)
{
    stack->curr_size = 0;
    stack->act_size = 0;
    free(stack->data);
}

int
pop_stack(Stack *stack, void *res, size_t size_res)
{
    if(stack->curr_size < size_res)
        return MEM_EXH;
    
    stack->curr_size -= size_res;
    for(size_t i = 0; i < size_res; i++)
        ((char *)res)[i] = ((char *)stack->data)[stack->curr_size + i];
    
    return 0;
}


int
push_stack(Stack *stack, void *res, size_t size_res)
{
    if(stack->act_size < stack->curr_size + size_res)
    {
        stack->act_size *= 2;
        stack->data = (char*)realloc(stack->data, sizeof(char) * stack->act_size);
        if(stack->data == NULL)
            return MEM_OVER;
    }
    
    for(size_t i = 0; i < size_res; i++)
        ((char *)stack->data)[stack->curr_size + i] = ((char *)res)[i];
    
    stack->curr_size += size_res;
    return 0;
}

