#ifndef POLIS
#define POLIS

#include <stddef.h>
#include "stack.h"

typedef struct
{
    void *data; //указатель на массив
    size_t size; // размер массива
} Polis;

typedef char Size_elem;

typedef int (*Calculate_elem)(const void *elem, Size_elem size, Stack *stack);

int calculate_polis(const Polis *, void *, size_t);

int build_polis(Polis *);

int next_char(int *);

void add_dec(int *, Polis *);

void mul_div(int *, Polis *);

void symbol(int *, Polis *);

int make_polis(const char *, Polis *);

void error(Polis *, int *);

void check_stdin(int *, int, Polis *);

#endif
