#ifndef FUNCS
#define FUNCS

#include "polis.h"

#define NON_CREAT 6
#define LEN 1024
#define WRONG_WR 11
#define WR_PIPE 12

int comand_func(const void *elem, Size_elem size, Stack *stack, pipes *);

int out_add_func(const void *elem, Size_elem size, Stack *stack, pipes *);

int out_func(const void *elem, Size_elem size, Stack *stack, pipes *);

int in_func(const void *elem, Size_elem size, Stack *stack, pipes *);

int or_func(const void *elem, Size_elem size, Stack *stack, pipes *);

int and_and_func(const void *elem, Size_elem size, Stack *stack, pipes *);

int or_or_func(const void *elem, Size_elem size, Stack *stack, pipes *);

int cl_br_func(const void *elem, Size_elem size, Stack *stack, pipes *);

int last_func(const void *elem, Size_elem size, Stack *stack, pipes *);

#endif
