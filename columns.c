#include "generator.h"
#include "columns.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void getFullName(char fullname[], const t_BLOCK *b_name, const t_BLOCK *b_surnam){
    char *temp_surnam = b_surnam->data[rand()%b_name->size];

    if ( strlen(temp_surnam) > 3 ){
        snprintf(fullname, STRLENGTH-1, "%s %s", b_name->data[rand()%b_name->size],
                                                 temp_surnam);
    } else {
        snprintf(fullname, STRLENGTH-1, "%s %s %s", b_name->data[rand()%b_name->size],
                                                    temp_surnam,
                                                    b_surnam->data[rand()%b_name->size]);
    }
}

void getID(char str[], t_BLOCK *BLOCK){
    static size_t i = 0;
    snprintf(str,BLOCK->digits-1,"%s",BLOCK->data[i]);
    ++i;
}

void getAgency(char str[], t_BLOCK *BLOCK){
    snprintf(str,BLOCK->digits-1,"%s",BLOCK->data[rand()%BLOCK->size]);
}

void getUsers(FILE* f_out, t_BLOCK *addr[], t_DB *database)
{
    size_t i;
    
    for ( i=0; i<DBSIZE; ++i ){
        getFullName(database->user[i].fullname,addr[0],addr[1]);
        getID(database->user[i].ID, addr[2]);
        getAgency(database->user[i].agency, addr[3]);

        fprintf(f_out,"%s,%s,%s\n", database->user[i].ID, 
                                    database->user[i].agency,
                                    database->user[i].fullname); 
    }
}

void stringSwap(char **str1_ptr, char **str2_ptr)
{
    char *temp = *str1_ptr;
    *str1_ptr = *str2_ptr;
    *str2_ptr = temp;
}
