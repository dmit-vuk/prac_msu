#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"
#include "polis.h"
#include "funcs.h"
#include "vars.h"

int
main(void)
{	
    Polis pol;
    pol.size = 0;
    printf("Enter the expression you want to calculate\n");
    if(build_polis(&pol) == EOF)
        return 0;
    int fl = 1;
    while(fl)
    {
        printf("Enter values of variables\n");
        printf("The entering ends with string 'end'\n");
        int fl_read = read_vars(&tab_vars);
        if (fl_read == LONG_LENG)
            fprintf(stderr, "%s", "Error: lenght of variable is too much\n");
        double res;
        int flag = calculate_polis(&pol, &res, sizeof(res));
        if(flag == MISS_VAR)
            fprintf(stderr, "%s", "Error: the values of variables isn't specified\n");
        else if(flag)
            fprintf(stderr, "%s", "Error!!\n");
        else
            printf("res = %lf\n", res);
        if(fl_read == EOF)
            break;
        printf("Do you want to finish? Write 'y' if yes, else 'n'\n");
        int c = '5';
        while(c != 'y' && c != 'n' && c != EOF)
            c = getchar();
        if(c == 'y' || c == EOF)
            fl = 0;
        if(c != EOF)
            while(c != '\n')
                c = getchar();
    }
    free(pol.data);
    delete_vars(&tab_vars);
	return 0;
}




