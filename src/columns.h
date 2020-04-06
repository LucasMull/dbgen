#include <stdio.h>

void joinRandomDATA(char joint[], const t_BLOCK *BLOCK1, const t_BLOCK *BLOCK2);

void fetchLinearDATA(char dest[], t_BLOCK *BLOCK, size_t i);

void fetchRandomDATA(char dest[], t_BLOCK *BLOCK);

void getUsers(FILE* f_out, t_BLOCK *addr[], t_DB *database);

void stringSwap(char **str1_ptr, char **str2_ptr);

void shuffleArray(t_BLOCK *BLOCK);
