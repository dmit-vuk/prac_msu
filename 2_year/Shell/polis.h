#ifndef POLIS
#define POLIS

#include <stddef.h>
#include "stack.h"
#include "list.h"

#define AND 10

typedef struct
{
    void *data; //указатель на массив
    size_t size; // размер массива
} Polis;

typedef struct cmd_inf{
    char out;
    char *out_add;
    char in;
    char *and_and;
    char *or_or;
    char and_op;
    char or_op;
    int brackets;
    int and_was;
} cmd_inf;

typedef struct pipes{
    int in;
    int out;
    int out_f;
    int in_f;
    jmp **jump;
    int status;
    int sdin;
    int sdout;
    int stdin_my;
    int stdout_my;
}pipes; 

typedef char Size_elem;

typedef int (*Calculate_elem)(const void *elem, Size_elem size, Stack *stack, pipes *);

int calculate_polis(const Polis *, void *, size_t, jmp **, Stack *);

int build_polis(Polis *, jmp **, cmd_inf *);

int next_char(int *, cmd_inf *);

void and_or(int *, Polis *, jmp **, cmd_inf *);

void symbol(int *, Polis *, jmp **, cmd_inf *);

int make_polis(const char *, Polis *, Calculate_elem);

void one_or(int *, Polis *, jmp **, cmd_inf *);

void out_in(int *curc, Polis *pol, jmp **, cmd_inf *);

void error(Polis *, int *, jmp **, cmd_inf *);

void check_stdin(int *, int *, Polis *, jmp **, cmd_inf *);

void delete_polis(Polis *pol);

#endif
