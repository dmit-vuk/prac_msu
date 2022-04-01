#include <stdio.h>
#include <stdlib.h>
#include "list.h"


void
add_list(jmp **lst, int brackets)
{
    jmp *tmp, *ad_tmp;
    
    if(!(*lst))
	{
		(*lst) = malloc(sizeof(jmp));
		(*lst)->next = NULL;
		(*lst)->bytes = 0;
        (*lst)->flag = 0;
        (*lst)->brackets = brackets;
	}
    else
    {
		tmp = (*lst);
		while(tmp->next)
			tmp = tmp->next;
		
		ad_tmp = malloc(sizeof(jmp));
		ad_tmp->bytes = 0;
		ad_tmp->next = NULL;
		ad_tmp->flag = 0;
        ad_tmp->brackets = brackets;
        tmp->next = ad_tmp;
	}
}

int
del_first(jmp **jump)
{
    if(jump)
    {
        jmp *temp = *jump;
        *jump = (*jump)->next;
        free(temp);
    }
    else
        return JMP_EMPTY;
   return 0; 
}

void
inc_bytes(jmp **lst)
{
    jmp *temp = *lst;
    while(temp)
    {
        if(!temp->flag)
            ++temp->bytes;
        temp = temp->next;
    }
}

void
last_byte(jmp **lst, int add, int brackets)
{
    jmp *temp = *lst;
    while(temp)
    {
        if(temp->brackets == brackets && !temp->flag)
        {
            ++temp->flag;
            temp->bytes += add;
        }
        temp = temp->next;
    }
}
