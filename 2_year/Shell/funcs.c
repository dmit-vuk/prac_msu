#include "stack.h"
#include "funcs.h"
#include "polis.h"
#include <string.h>
#include <math.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

int
comand_func(const void *elem, Size_elem size, Stack *stack, pipes *exec)
{
    char **str = (char **)((char *)elem + sizeof(Calculate_elem));
    int f;
    if((f = push_stack(stack, str, sizeof(char *))) != 0)
        return f;
    return 0;
}

char **
get_argv(char *cmd, int *argc)
{
    char *str = cmd;
    char **argv = malloc(sizeof(char *));
    int len;
    while((str = strchr(str, ' ')) != NULL)
    {
        len = str - cmd;
        argv = realloc(argv, sizeof(char *)*(*argc + 1));
        argv[*argc] = malloc(sizeof(char) * (len + 1));
        strncpy(argv[*argc], cmd, len);
        argv[*argc][len] = '\0';
        (*argc)++;
        ++str;
        cmd = str;
    }
    argv = (char **) realloc(argv, sizeof(char **)*(*argc + 2));
    argv[*argc] = malloc(sizeof(char) * (strlen(cmd) + 1));
    strncpy(argv[*argc], cmd, strlen(cmd) + 1);
    (*argc)++;
    argv[*argc] = NULL;
    return argv;
}

int// |
or_func(const void *elem, Size_elem size, Stack *stack, pipes *exec)
{
    
    int argc = 0;
    char *cmd;
    
    if(!stack->curr_size)
        return 0;
    int f;
    if((f = pop_stack(stack, &cmd, sizeof(char *))) != 0)
        return f;
    char **argv = get_argv(cmd, &argc);
    
    if(exec->in == exec->stdin_my){
        close(exec->stdout_my);
        int new_std[2];
        if(pipe(new_std) == -1) return WR_PIPE;
        exec->stdin_my = new_std[0];
        exec->stdout_my = new_std[1];
    }
    
    if(fork() == 0)
    {
        dup2(exec->in, 0);
        dup2(exec->out, 1);
        close(exec->in);
        close(exec->out);
        execvp(argv[0], argv);
        perror("Exec is wrong");
        exit(errno);
    }
    
    for(int i = 0; i < argc; i++)
        free(argv[i]);
    free(argv);
    
    return 0;
}

int// >>
out_add_func(const void *elem, Size_elem size, Stack *stack, pipes *exec)
{
    char *cmd;
    int f;
    if((f = pop_stack(stack, &cmd, sizeof(char *))) != 0)
        return f;
    int fd;
    if((fd = open(cmd, O_WRONLY | O_APPEND | O_CREAT, 0644))==-1)
        return NON_CREAT;
    exec->out_f = 1;
    exec->in = fd;
    
    if(!stack->curr_size)
    {
        char buf[LEN];
        int n;
        close(exec->stdout_my);
        while((n = read(exec->stdin_my, buf, LEN)) > 0)
            if(write(fd, buf, n) == -1) return WRONG_WR;
        close(fd);
        close(exec->stdin_my);
        
        int new_std[2];
        if(pipe(new_std) == -1) return WR_PIPE;
        exec->stdin_my = new_std[0];
        exec->stdout_my = new_std[1];
    }
    return 0;
}

int//  >
out_func(const void *elem, Size_elem size, Stack *stack, pipes *exec)
{
    char *cmd;
    int f;
    if((f = pop_stack(stack, &cmd, sizeof(char *))) != 0)
        return f;
    int fd;
    if((fd = open(cmd, O_WRONLY | O_TRUNC | O_CREAT, 0644))==-1)
        return NON_CREAT;
    exec->out_f = 1;
    exec->in = fd;
    
    if(!stack->curr_size)
    {
        char buf[LEN];
        int n;
        close(exec->stdout_my);
        while((n = read(exec->stdin_my, buf, LEN)) > 0)
            if(write(fd, buf, n) == -1) return WRONG_WR;
        close(fd);
        close(exec->stdin_my);
        
        int new_std[2];
        if(pipe(new_std) == -1) return WR_PIPE;
        exec->stdin_my = new_std[0];
        exec->stdout_my = new_std[1];
    }
    return 0;
}

int// <
in_func(const void *elem, Size_elem size, Stack *stack, pipes *exec)
{
    char *cmd;
    int f;
    if((f = pop_stack(stack, &cmd, sizeof(char *))) != 0)
        return f;
    int fd;
    if((fd = open(cmd, O_RDONLY))==-1)
        return NON_CREAT;
    exec->in_f = fd;
    return 0;
}

int // ||
or_or_func(const void *elem, Size_elem size, Stack *stack, pipes *exec)
{
    if(stack->curr_size)
    {
        char *cmd;
        int f;
        if((f = pop_stack(stack, &cmd, sizeof(char *))) != 0)
            return f;
        int argc = 0;
        char **argv = get_argv(cmd, &argc);        
        int pid;
        if((pid = fork()) == 0)
        {
            if(!exec->out_f)
                exec->out = exec->stdout_my;
            
            dup2(exec->in, 0);
            dup2(exec->out, 1);
            close(exec->in);
            close(exec->out);
            execvp(argv[0], argv);
            perror("Exec is wrong");
            exit(errno);
        }
        for(int i = 0; i < argc; i++)
            free(argv[i]);
        free(argv);
        
        int status;
        int skips;
        waitpid(pid, &status, 0);
        if(WIFEXITED(status) && !WEXITSTATUS(status))
        {
            jmp **j = exec->jump;
            skips = (*j)->bytes;
            del_first(exec->jump);
            exec->status = status;
            return 100 + skips;
        }
        else
        {
            del_first(exec->jump);
        }
    }
    else
    {
        if(WIFEXITED(exec->status) && !WEXITSTATUS(exec->status))
        {
            int skips;
            jmp **j = exec->jump;
            skips = (*j)->bytes;
            del_first(exec->jump);
            return 100 + skips;
        }
        else
        {
            del_first(exec->jump);
        }
    }
    return 0;
}

int// &&
and_and_func(const void *elem, Size_elem size, Stack *stack, pipes *exec)
{
    if(stack->curr_size)
    {
        char *cmd;
        int f;
        if((f = pop_stack(stack, &cmd, sizeof(char *))) != 0)
            return f;
        int argc = 0;
        char **argv = get_argv(cmd, &argc);        
        int pid;
        if((pid = fork()) == 0)
        {
            if(!exec->out_f)
                exec->out = exec->stdout_my;
            
            dup2(exec->in, 0);
            dup2(exec->out, 1);
            close(exec->in);
            close(exec->out);
            execvp(argv[0], argv);
            perror("Exec is wrong");
            exit(errno);
        }
        
        for(int i = 0; i < argc; i++)
            free(argv[i]);
        free(argv);
        
        int status;
        int skips;
        waitpid(pid, &status, 0);
        if(!WIFEXITED(status) || WEXITSTATUS(status))
        {
            jmp **j = exec->jump;
            skips = (*j)->bytes;
            del_first(exec->jump);
            exec->status = status;
            return 100 + skips;
        }
        else
        {
            del_first(exec->jump);
        }
    }
    else
    {
        if(!WIFEXITED(exec->status) || WEXITSTATUS(exec->status))
        {
            int skips;
            jmp **j = exec->jump;
            skips = (*j)->bytes;
            del_first(exec->jump);
            return 100 + skips;
        }
        else
        {
            del_first(exec->jump);
        }
    }
    return 0;
}

int
cl_br_func(const void *elem, Size_elem size, Stack *stack, pipes *exec)
{
    if(!stack->curr_size)
        return 0;

    int argc = 0;
    char *cmd;
    
    int f;
    if((f = pop_stack(stack, &cmd, sizeof(char *))) != 0)
        return f;
    char **argv = get_argv(cmd, &argc);
    if(exec->out != exec->sdout && !exec->out_f)
        close(exec->out);
    
    if(exec->in == exec->stdin_my){
        close(exec->stdout_my);
        int new_std[2];
        if(pipe(new_std) == -1) return WR_PIPE;
        exec->stdin_my = new_std[0];
        exec->stdout_my = new_std[1];
    }

    int pid;
    if((pid = fork()) == 0)
    {
        dup2(exec->in, 0);
        if(!exec->out_f){
            dup2(exec->stdout_my, 1);
            close(exec->stdout_my);
        }
        else{
            dup2(exec->out, 1);
            close(exec->out);
        }
        close(exec->in);
        execvp(argv[0], argv);
        perror("Exec is wrong");
        exit(errno);
    }
    
    int status;
    waitpid(pid, &status, 0);
    exec->status = status;
    
    for(int i = 0; i < argc; i++)
        free(argv[i]);
    free(argv);
    
    return 0;
}

int// last
last_func(const void *elem, Size_elem size, Stack *stack, pipes *exec)
{
    
    int argc = 0;
    char *cmd;
    
    int f;
    if((f = pop_stack(stack, &cmd, sizeof(char *))) != 0)
        return f;
    char **argv = get_argv(cmd, &argc);
    
    if(exec->in == exec->stdin_my){
        close(exec->stdout_my);
        int new_std[2];
        if(pipe(new_std) == -1) return WR_PIPE;
        exec->stdin_my = new_std[0];
        exec->stdout_my = new_std[1];
    }
    
    pid_t pid;
    if((pid = fork()) == 0)
    {
        dup2(exec->in, 0);
        dup2(exec->out, 1);
        close(exec->in);
        close(exec->out);
        execvp(argv[0], argv);
        perror("Exec is wrong");
        exit(errno);
    }
    
    for(int i = 0; i < argc; i++)
        free(argv[i]);
    free(argv);
    
    return pid;
}


