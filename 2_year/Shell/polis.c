#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <setjmp.h>
#include <stdio.h>
#include "funcs.h"
#include "stack.h"
#include "polis.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define SAFE(call) do{\
                    flag = call;\
                    if(flag && flag < 100)\
                        goto FINALLY;\
                    }\
                    while(0)

int
calculate_polis(const Polis *pol, void *resp, size_t size_res, jmp **jump, Stack *stack)
{
    size_t curr_size = 0;
    
    int flag, fd[2], skips = 0, prev_in = dup(0);
    int std[2];
    SAFE(pipe(std));
    pipes exec = {dup(0), 235, 0, 0, jump, 0, dup(0), dup(1), std[0], std[1]};
    Calculate_elem func = 0;
    for(; curr_size < pol->size; curr_size += sizeof(Size_elem) + ((char *)pol->data)[curr_size])
    {
        
        if(skips > 0)
        {
            --skips;
            continue;
        }
        if(func != out_add_func && func != out_func && func != in_func && func != comand_func)
            exec.out_f = 0;
        void *elem = &((char *)pol->data)[curr_size + sizeof(Size_elem)];
        memcpy(&func, elem, sizeof(Calculate_elem));
        
        if(func == or_func || func == and_and_func ||
           func == or_or_func)
        {
            if(!exec.out_f)
            {
                    if(func == or_func)
                    {
                        if(stack->curr_size)
                        {
                            SAFE(pipe(fd));
                            exec.in = prev_in;
                            exec.out = fd[1];
                            prev_in = fd[0];
                        }
                        else
                            prev_in = exec.stdin_my;
                    }
                    else{
                        exec.in = prev_in;
                        prev_in = exec.sdin;
                    }
            }
            else
            {
                exec.out = exec.in;
                exec.in = prev_in;
            }
        }
        if(func == cl_br_func)
        {
            if(exec.out_f)
                exec.out = exec.in;
            exec.in = prev_in;
        }

        SAFE(func(elem, ((char *)pol->data)[curr_size], stack, &exec));
        if(flag > 100)
        {
            skips = flag - 100;
        }
        if(exec.in_f)
        {
            prev_in = exec.in_f;
            exec.in_f = 0;
        }
        if(func == or_func && exec.out != exec.sdout && exec.out){
            close(exec.out);
        }
    }
    
    if(prev_in != exec.stdin_my)
    {
        char buf[LEN];
        int n;
        close(exec.stdout_my);
        while((n = read(exec.stdin_my, buf, LEN)) > 0)
            if(write(exec.sdout, buf, n) == -1) return WRONG_WR;
    }
    
    if(stack->curr_size)
    {
        if(!exec.out_f)
                exec.out = exec.sdout;
        else
            exec.out = exec.in;
        exec.in = prev_in;
        pid_t pid = last_func(NULL, 0, stack, &exec); 
        waitpid(pid, 0, 0);
        close(exec.in);
        close(exec.out);
        close(exec.stdout_my);
    }
        
    close(exec.stdin_my);

    delete_stack(stack);
    return 0;

    FINALLY:
        delete_stack(stack);
        return flag;
}

jmp_buf begin;

int
build_polis(Polis *pol, jmp **jump, cmd_inf * cmd_info)
{
    setjmp(begin);
    cmd_info->and_was = 0;
    int c = getchar();
    if(c == EOF)
        return EOF;
    and_or(&c, pol, jump, cmd_info);
    return cmd_info->and_was;
}

int
next_char(int *curc,cmd_inf *cmd_info)
{
    *curc = getchar();
    if(*curc != '\n' && *curc != EOF && *curc != '&')
        return 0;
    if(*curc == '&')
    {
        *curc = getchar();
        if(*curc != '\n' && *curc != EOF && *curc != '&')
        {
            printf("ERROR, & must be at the end of your expression\n");
            return 5;
        }
        if(*curc == '&')
            return 0;
        cmd_info->and_was = AND;
    }
    return 1;
}

void// && ||
and_or(int *curc, Polis *pol, jmp **jump, cmd_inf * cmd_info)
{
    one_or(curc, pol, jump, cmd_info);
    while(*curc == '|' || *curc == '&')
    {
        if(*curc == '|')
            make_polis(cmd_info->or_or, pol, or_or_func);
        else
            make_polis(cmd_info->and_and, pol, and_and_func);
        last_byte(jump, -1, cmd_info->brackets);
        add_list(jump, cmd_info->brackets);
        next_char(curc, cmd_info);
        one_or(curc, pol, jump, cmd_info);
    }
}

void// |
one_or(int *curc, Polis *pol, jmp **jump, cmd_inf * cmd_info)
{
    out_in(curc, pol, jump, cmd_info);
    if(*curc == cmd_info->or_op)
    {
        int prevc = *curc;
        next_char(curc, cmd_info);
        if(*curc != cmd_info->or_op)
        {
            while(prevc == cmd_info->or_op)
            {
                make_polis(&cmd_info->or_op, pol, or_func);
                out_in(curc, pol, jump, cmd_info);
                prevc = 1;
                if(*curc == cmd_info->or_op)
                {
                    next_char(curc, cmd_info);
                    if(*curc != cmd_info->or_op)
                        prevc = cmd_info->or_op;
                }
            }
        }
    }
}

void// < > >>
out_in(int *curc, Polis *pol, jmp **jump, cmd_inf * cmd_info)
{
    symbol(curc, pol, jump, cmd_info);
    while(*curc == '>' || *curc == '<')
    {
        int temp = *curc, fl_out_add = 0;
        if(*curc == cmd_info->out && !next_char(curc, cmd_info))
        {
            if(*curc == cmd_info->out)
            {
                next_char(curc, cmd_info);
                fl_out_add = 1;
            }
        }
        else
            next_char(curc, cmd_info);
        symbol(curc, pol, jump, cmd_info);
        if(!fl_out_add)
        {
            char out = (char)temp;
            if(temp == cmd_info->out)
                make_polis(&out, pol, out_func);
            else
                make_polis(&out, pol, in_func);
        }
        else
            make_polis(cmd_info->out_add, pol, out_add_func);
    }
}

void
symbol(int *curc, Polis *pol, jmp **jump, cmd_inf * cmd_info)
{
    char *out;

    while(*curc == ' ')
        if(next_char(curc, cmd_info))
            break;

    if(*curc == '(')
    {
        (cmd_info->brackets)++;
        next_char(curc, cmd_info);
        and_or(curc, pol, jump, cmd_info);
        if(*curc != ')')
        {
            fprintf(stderr, "%s", "Error: wrong number of brackets, '(' is extra\n");
            error(pol, curc, jump, cmd_info);
        }
        else
        {
            make_polis((char *) curc, pol, cl_br_func);
            next_char(curc, cmd_info);
            while(*curc == ' ')
                next_char(curc, cmd_info);
            inc_bytes(jump);
        }
    }
    
    else if(*curc == '&' || *curc == '|' ||
            *curc == '<' || *curc == '>' ||
            *curc == '\n' || *curc == EOF)
    {
        fprintf(stderr, "%s", "Error: the operand is missing\n");
        error(pol, curc, jump, cmd_info);
    }

    else if(*curc == ')')
    {
        if(!cmd_info->brackets)
        {
            fprintf(stderr, "%s", "Error: wrong number of brackets, ')' is extra\n");
            error(pol, curc, jump, cmd_info);
        }
        else
        {
            fprintf(stderr, "%s", "Error: the operand is missing\n");
            error(pol, curc, jump, cmd_info);
        }
    }
    else
    {
        int fl = 0;
        out = malloc(sizeof(out));
        int i = 0;
        while(*curc != cmd_info->and_op && *curc != cmd_info->or_op &&
              *curc != cmd_info->out && *curc != cmd_info->in && !fl &&
              *curc != ')' && *curc != '(')
        {
            while(*curc != cmd_info->and_op && *curc != cmd_info->or_op &&
                  *curc != cmd_info->out && *curc != cmd_info->in &&
                  *curc != ' ' && *curc != ')' && *curc != '(')
            {
                out = realloc(out, sizeof(char) * (i + 2));
                out[i] = *(char *)curc;
                ++i;
                if(next_char(curc, cmd_info))
                {
                    fl = 1;
                    break;
                }
            }
            out[i] = ' ';
            ++i;
            check_stdin(curc, &fl, pol, jump, cmd_info);
        }
        out[i - 1] = '\0';
        make_polis(out, pol, comand_func);
        inc_bytes(jump);
        if(*curc == cmd_info->and_op || *curc == cmd_info->or_op ||
            *curc == cmd_info->out || *curc == cmd_info->in)
              inc_bytes(jump);
    }

}

void
check_stdin(int *curc, int *fl, Polis *pol, jmp **jump, cmd_inf * cmd_info)
{
    while(*curc == ' ')
    {
        if(next_char(curc, cmd_info))
        {
            *fl = 1;
            break;
        }
    }

    if(*curc == ')')
    {
        last_byte(jump, 1, cmd_info->brackets);
        if(!cmd_info->brackets)
        {
            fprintf(stderr, "%s", "Error: wrong number of brackets, ')' is extra\n");
            error(pol, curc, jump, cmd_info);
        }
        else
        {
            --(cmd_info->brackets);
        }
    }
}

void
error(Polis *pol, int *curc, jmp **jump, cmd_inf *cmd_info)
{
    delete_polis(pol);
    while(*jump)
        del_first(jump);
    if(*curc != '\n' && *curc != EOF)
        while(!next_char(curc, cmd_info));
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
make_polis(const char *str, Polis *pol, Calculate_elem func)
{
    size_t curr_size = pol->size;
    char size;


    size = (char)(sizeof(char *) + sizeof(Calculate_elem));
    pol->size += (size_t)(size + sizeof(Size_elem));
    if(curr_size == 0)
        pol->data = malloc(pol->size);
    else
        pol->data = realloc(pol->data, pol->size);
    write_polis(pol, &size, sizeof(size), &curr_size);
    write_polis(pol, &func, sizeof(func), &curr_size);
    write_polis(pol, &str, sizeof(str), &curr_size);
    return 0;
}

void
delete_polis(Polis *pol)
{
    size_t curr_size = 0;
    for(; curr_size < pol->size; curr_size += sizeof(Size_elem) + ((char *)pol->data)[curr_size])
    {
        void *elem = &((char *)pol->data)[curr_size + sizeof(Size_elem)];
        char *str;
        
        Calculate_elem func;
        memcpy(&func, elem, sizeof(Calculate_elem));
        if(func == comand_func){
            memcpy(&str, ((char *)elem + sizeof(Calculate_elem)), sizeof(char *));
            free(str);}
    }
    if(pol->size)
        free(pol->data);
    pol->size = 0;
}


