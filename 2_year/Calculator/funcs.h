#ifndef FUNCS
#define FUNCS

#include "polis.h"

#define DIV_ZERO 1
#define MISS_VAR 2

int num_func(const void *elem, Size_elem size, Stack *stack);

int add_func(const void *elem, Size_elem size, Stack *stack);

int dec_func(const void *elem, Size_elem size, Stack *stack);

int mul_func(const void *elem, Size_elem size, Stack *stack);

int div_func(const void *elem, Size_elem size, Stack *stack);

int vars_func(const void *elem, Size_elem size, Stack *stack);

#endif
