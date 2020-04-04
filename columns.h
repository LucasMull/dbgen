#include <stdio.h>

void getFullName(char fullname[], const t_BLOCK *b_name, const t_BLOCK *b_surnam);

void getID(char str[], t_BLOCK *BLOCK);

void getAgency(char str[], t_BLOCK *BLOCK);

void getUsers(FILE* f_out, t_BLOCK *addr[], t_DB *database);

void stringSwap(char **str1_ptr, char **str2_ptr);
