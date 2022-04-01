#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"
#include "polis.h"
#include "funcs.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int sigint[2];

void
sigint_handler(int s)
{
    Polis *pol;
    jmp **jump;
    Stack *stack;
    if(read(sigint[0], &pol, sizeof(pol)) > 0){}
    if(read(sigint[0], &jump, sizeof(jump)) > 0){}
    if(read(sigint[0], &stack, sizeof(stack)) > 0){}
    delete_polis(pol);
    while(*jump)
        del_first(jump);
    delete_stack(stack);
    close(sigint[0]);
    close(sigint[1]);
    kill(getpid(), SIGKILL);
}

void
write_pipe(Polis *pol, jmp **jump, Stack *stack)
{
    init_stack(stack);
    if(pipe(sigint) == -1) exit(WR_PIPE);
    if(write(sigint[1], &pol, sizeof(pol)) == -1) exit(WRONG_WR);
    if(write(sigint[1], &jump, sizeof(jump)) == -1) exit(WRONG_WR);
    if(write(sigint[1], &stack, sizeof(stack)) == -1) exit(WRONG_WR);
}


int
main(void)
{	
    int fl = 1;
    int res;
    while(fl)
    {
        Polis pol;
        pol.data = NULL;
        pol.size = 0;
        jmp *jump = NULL;
        cmd_inf cmd_info = { '>', ">>", '<', "&&", "||", '&', '|', 0, 0};
        printf("Enter the expression you want to calculate\n");
        int fl_build = build_polis(&pol, &jump, &cmd_info);
        if(fl_build == EOF)
            return 0;
        if(fl_build == AND)
        {
            if(fork() == 0)
            {
                signal(SIGINT, sigint_handler);
                Stack stack;
                write_pipe(&pol, &jump, &stack);
                int flag = calculate_polis(&pol, &res, sizeof(res), &jump, &stack);
                if(flag)
                    fprintf(stderr, "%s", "Error!!\n");
                delete_polis(&pol);
                while(jump)
                    del_first(&jump);
                close(sigint[0]);
                close(sigint[1]);
                exit(flag);
            }
            while(waitpid(-1, NULL, WNOHANG) > 0){};
            delete_polis(&pol);
            while(jump)
                del_first(&jump);
        }
        else
        {
            signal(SIGINT, sigint_handler);
            Stack stack;
                write_pipe(&pol, &jump, &stack);
            int flag = calculate_polis(&pol, &res, sizeof(res), &jump, &stack);
            if(flag)
                fprintf(stderr, "%s", "Error!!\n");
            signal(SIGINT, SIG_DFL);
            delete_polis(&pol);
            while(jump)
                del_first(&jump);
            while(waitpid(-1, NULL, WNOHANG) > 0){};
            close(sigint[0]);
            close(sigint[1]);
        }
    
    }
	return 0;
}




