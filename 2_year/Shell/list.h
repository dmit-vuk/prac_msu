#ifndef LIST
#define LIST

#define JMP_EMPTY 10

typedef struct jmp{
    struct jmp *next;
    int bytes;
    int flag;
    int brackets;
} jmp;


void add_list(jmp **, int);

int del_first(jmp **);

void inc_bytes(jmp **);

void last_byte(jmp **, int, int);

#endif
