#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "vars.h"

void
add_var(vars *v, char *name, double num)
{
	if(!(*v))
    {
        *v = malloc(sizeof(struct var_list));
        (*v)->next = NULL;
        (*v)->str[0] = '\0';
    }
    vars temp = (*v), ad_temp;
    int fl = 0;
    for(; temp->next; temp = temp->next)
    {
        if(!strcmp(name, temp->str))
        {
            temp->value = num;
            fl = 1;
            break;
        }
    }
    
    if(!strcmp(name, temp->str))
    {
        temp->value = num;
        fl = 1;
    }

    if(!fl)
    {
        ad_temp = malloc(sizeof(struct var_list));
        ad_temp->value = num;
        ad_temp->next = NULL;
        strcpy(ad_temp->str, name);
        temp->next = ad_temp;
    }
}

int
read_vars(vars *v)
{
    char s[VAR_LEN + 1];
    char str[MAX_LEN + 2];
    double num;
    int i = 0;
    char end[5] = "end\n\0";
    for(;;)
    {
        if(fgets(str, sizeof(str), stdin) == NULL)
            return EOF;
        
        if(!strcmp(str, end))
            break;
        
        i = 0;
        while((str[i] != ' ') && (str[i] != '='))
        {
            if(i == VAR_LEN)
				return LONG_LENG;
            s[i] = str[i];
            ++i;
        }
        s[i] = '\0';
        while(str[i] != '=')
            ++i;
        
        if(!sscanf(&str[i+1], "%lf", &num))
            return 1;
        
        add_var(v, s, num);
    }
    return 0;
}

void
delete_vars(vars *v)
{
	vars temp = *v;
	while(*v)
    {
        (*v) = (*v)->next;
		free(temp);
        temp = *v;
	}
}

