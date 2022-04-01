#ifndef VARS
#define VARS


#define VAR_LEN 6
#define LONG_LENG 7
#define MAX_LEN 1024

struct var_list
{
    char str[VAR_LEN + 1];
    double value;
    struct var_list *next;
};

typedef struct var_list *vars;

int read_vars(vars *);
void delete_vars(vars *v);

vars tab_vars;

#endif

