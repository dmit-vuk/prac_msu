#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <setjmp.h>
#include <stdio.h>
#include "funcs.h"
#include "vars.h"
#include "stack.h"
#include "polis.h"

#define SAFE(call) do{\
                    flag = call;\
                    if(flag)\
                        goto FINALLY;\
                    }\
                    while(0)

int
calculate_polis(const Polis *pol, void *resp, size_t size_res)
{
    size_t curr_size = 0;
    Stack stack;
    int flag = init_stack(&stack);
    if(flag)
    {
        return flag;
    }
    
    for(; curr_size < pol->size; curr_size += sizeof(Size_elem) + ((char *)pol->data)[curr_size])
    {	
        void *elem = &((char *)pol->data)[curr_size + sizeof(Size_elem)];
        Calculate_elem func;
        memcpy(&func, elem, sizeof(Calculate_elem));
        SAFE(func(elem, ((char *)pol->data)[curr_size], &stack));
       
    }
    SAFE(pop_stack(&stack, resp, size_res));
    
    delete_stack(&stack);
    return 0;
    
    FINALLY:
        delete_stack(&stack);
        return flag;
}

jmp_buf begin;

int
build_polis(Polis *pol)
{
    setjmp(begin);
    int c = getchar();
    if(c == EOF)
		return EOF;
    add_dec(&c, pol);
    return 0;
}

int
next_char(int *curc)
{
	*curc = getchar();
    if(*curc != '\n' && *curc != EOF)
        return 0;
    
    return 1;
}

void
add_dec(int *curc, Polis *pol)
{
    mul_div(curc, pol);
    while(*curc == '+' || *curc == '-')
    {
        int temp = *curc;
        next_char(curc);
        mul_div(curc, pol);
        char out = (char)temp;
        make_polis(&out, pol);
    }
}

void
mul_div(int *curc, Polis *pol)
{
    symbol(curc, pol);
    while(*curc=='*' || *curc == '/')
    {
        int temp = *curc;
        next_char(curc);
        symbol(curc, pol);
        char out = (char)temp;
        make_polis(&out, pol);
    } 
}

int brackets = 0;

void
symbol(int *curc, Polis *pol)
{
    char *out;
    
    while(*curc == ' ')
        if(next_char(curc))
            break;
    
    if(*curc == '(')
    {
        ++brackets;
        next_char(curc);
        add_dec(curc, pol);
        if(*curc != ')')
        {
            fprintf(stderr, "%s", "Error: wrong number of brackets, '(' is extra\n");
            error(pol, curc);
        }
        else
        {
            next_char(curc);
            while(*curc == ' ')
                next_char(curc);
            if(*curc == '(')
            {
                fprintf(stderr, "%s", "Error: the right operation is missing\n");
                error(pol, curc);
            }
        }
    }
    else if(isdigit(*curc) || (*curc) == '.')
    {
        int fl = 0, i = 0, dot = 0;
        out = malloc(sizeof(out));
        if((*curc) == '.')
        {
            out[0] = '0';
            ++i;
        }
        while(isdigit(*curc) || *curc == '.')
        {
            if(*curc == '.' && dot)
            {
                free(out);
                fprintf(stderr, "%s", "Error: wrong operand\n");
                error(pol, curc);
            }
            else if(*curc == '.')
                ++dot;
            
            out = realloc(out, sizeof(char) * (i + 2));
            out[i] = *(char *)curc;
            ++i;
            if(next_char(curc))
            {
                fl = 1;
                break;
            }
        }
        out[i] = '\0';
        make_polis(out, pol);
        free(out);

        check_stdin(curc, fl, pol);
    }
    
    else if(isalpha(*curc))
    {
        int fl = 0, i = 0;
        out = malloc(sizeof(out));
        while(*curc != '+' && *curc != '-' &&
              *curc != '*' && *curc != '/' &&
              *curc != ' ' && *curc != ')' && *curc != '(')
        {
            if(!isalpha(*curc) && !isdigit(*curc) && *curc != '_')
            {
                free(out);
                fprintf(stderr, "%s", "Error: wrong operand\n");
                error(pol, curc);
            }
            
            if(i == VAR_LEN)
            {
				free(out);
                fprintf(stderr, "%s", "Error: lenght of variable is too much\n");
				error(pol, curc);
			}
            out = realloc(out, sizeof(char) * (i + 2));
            out[i] = *(char *)curc;
            ++i;
			
            if(next_char(curc))
            {
                fl = 1;
                break;
            }
        }
        out[i] = '\0';
        make_polis(out, pol);
        free(out);
        
        check_stdin(curc, fl, pol);
    }
    
    else if(*curc == '+' || *curc == '-' ||
            *curc == '*' || *curc == '/' ||
            *curc == '\n' || *curc == EOF)
    {
        fprintf(stderr, "%s", "Error: the operand is missing\n");
        error(pol, curc);
    }
    
    else if(*curc == ')')
    {
        if(!brackets)
        {
            fprintf(stderr, "%s", "Error: wrong number of brackets, ')' is extra\n");
            error(pol, curc);
        }
        else
        {
            fprintf(stderr, "%s", "Error: the operand is missing\n");
            error(pol, curc);
        }
	}
    else
    {
        fprintf(stderr, "%s", "Error: wrong command!\n");
        error(pol, curc);
    }
}

void
check_stdin(int *curc, int fl, Polis *pol)
{
    while(*curc == ' ')
    {
        if(next_char(curc))
        {
            fl = 1;
            break;
        }
    }
        
    if((isdigit(*curc) || isalpha(*curc) || *curc == '(') && !fl)
    {
        fprintf(stderr, "%s", "Error: the right operation is missing\n");
        error(pol, curc);
    }
    
    else if(*curc == ')')
    {
        if(!brackets)
        {
            fprintf(stderr, "%s", "Error: wrong number of brackets, ')' is extra\n");
            error(pol, curc);
        }
        else
            --brackets;
    }
    
    if(*curc != '+' && *curc != '-' && *curc != ')' &&
       *curc != '*' && *curc != '/' && !fl)
    {
        
        fprintf(stderr, "%s", "Error: wrong command\n");
        error(pol, curc);
    }
}

void
error(Polis *pol, int *curc)
{
    if(pol->size)
        free(pol->data);
    pol->size = 0;
    delete_vars(&tab_vars);
    if(*curc != '\n' && *curc != EOF)
		while(!next_char(curc));
    printf("Please write new expression\n");
    longjmp(begin, 1);
    
}


void
write_polis(Polis *pol, const void *elem, size_t size, size_t *curr_size)
{
    for (size_t i = 0; i < size; ++i)
    {
        ((char *)pol->data)[*curr_size] = ((char*)elem)[i];
        *curr_size += 1;
    }
}


int
make_polis(const char *str, Polis *pol)
{
    size_t curr_size = pol->size;
    char size;
    Calculate_elem f;
    
    if(isdigit(str[0]) || isalpha(str[0]))
    {
        double chis;
        if(sscanf(str, "%lf", &chis))
        {
            size = (char)(sizeof(chis) + sizeof(Calculate_elem));
            pol->size += (size_t)(size + sizeof(Size_elem));
            if(curr_size == 0)
                pol->data = malloc(pol->size);
            else
                pol->data = realloc(pol->data, pol->size);
            
            write_polis(pol, &size, sizeof(size), &curr_size);
            f = num_func;
            write_polis(pol, &f, sizeof(f), &curr_size);
            write_polis(pol, &chis, sizeof(chis), &curr_size);
        }
        else
        {
            int len = strlen(str);
            size = (char)(len + sizeof(Calculate_elem));
            pol->size += (size_t)(size + sizeof(Size_elem));
            if(curr_size == 0)
                pol->data = malloc(pol->size);
            else
                pol->data = realloc(pol->data, pol->size);
            
            write_polis(pol, &size, sizeof(size), &curr_size);
            f = vars_func;
            write_polis(pol, &f, sizeof(f), &curr_size);
            write_polis(pol, str, len, &curr_size);
        }
    }
    else
    {
        char oper = str[0];
        size = (char)(sizeof(oper) + sizeof(Calculate_elem));
        pol->size += (size_t)(size + sizeof(Size_elem));
        pol->data = realloc(pol->data, pol->size);
        write_polis(pol, &size, sizeof(size), &curr_size);
        
        if(oper == '+')
            f = add_func;
        else if(oper == '-')
            f = dec_func;
        if(oper == '*')
            f = mul_func;
        if(oper == '/')
            f = div_func;
                
        write_polis(pol, &f, sizeof(f), &curr_size);
        write_polis(pol, &oper, sizeof(oper), &curr_size);
    }
    return 0;
}

