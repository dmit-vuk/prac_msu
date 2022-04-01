#include "stack.h"
#include "funcs.h"
#include <string.h>
#include <math.h>
#include <float.h>
#include "vars.h"

int
num_func(const void *elem, Size_elem size, Stack *stack)
{
    double *ch = (double *)((char *)elem + sizeof(Calculate_elem));
    push_stack(stack, ch, sizeof(double));
    return 0;
}

int
add_func(const void *elem, Size_elem size, Stack *stack)
{
    double op_1 = 0, op_2 = 0, res = 0;
    pop_stack(stack, &op_1, sizeof(double));
    pop_stack(stack, &op_2, sizeof(double));
    res = op_1 + op_2;
    push_stack(stack, &res, sizeof(double));
    return 0;
}

int
dec_func(const void *elem, Size_elem size, Stack *stack)
{
    double op_1 = 0, op_2 = 0, res = 0;
    pop_stack(stack, &op_1, sizeof(double));
    pop_stack(stack, &op_2, sizeof(double));
    res = op_2 - op_1;
    push_stack(stack, &res, sizeof(double));
    return 0;
}

int
mul_func(const void *elem, Size_elem size, Stack *stack)
{
    double op_1 = 0, op_2 = 0, res = 0;
    pop_stack(stack, &op_1, sizeof(double));
    pop_stack(stack, &op_2, sizeof(double));
    res = op_1 * op_2;
    push_stack(stack, &res, sizeof(double));
    return 0;
}

int
div_func(const void *elem, Size_elem size, Stack *stack)
{
    double op_1 = 0, op_2 = 0, res = 0;
    pop_stack(stack, &op_1, sizeof(double));
    pop_stack(stack, &op_2, sizeof(double));
    if(fabs(op_1) < FLT_EPSILON)
        return DIV_ZERO;
    res = op_2 / op_1;
    push_stack(stack, &res, sizeof(double));
    return 0;
}

int
vars_func(const void *elem, Size_elem size, Stack *stack)
{
    char *str = (char *)elem + sizeof(Calculate_elem);
    char s[VAR_LEN + 1];
    double ch;
    int i = 0, fl = 0, size_str = size - sizeof(Calculate_elem);
    for(i = 0; i < size_str; ++i)
        s[i] = str[i];
    s[i] = '\0';
    vars temp = tab_vars;
    for(; temp; temp = temp->next)
    {
        if(!strcmp(s, temp->str))
        {
            ch = temp->value;
            fl = 1;
            break;
        }
    }
    if(fl)
    {
        push_stack(stack, &ch, sizeof(double));
        return 0;
    }
    return MISS_VAR;
}


