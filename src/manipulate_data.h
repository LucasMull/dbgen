#include <stdio.h>

char *pickRandomDATA(t_BLOCK *BLOCK);

void joinStrings(char joint[], char str1[], char str2[]);

void joinRandomDATA(char joint[], const t_BLOCK *BLOCK1, const t_BLOCK *BLOCK2);

void fetchLinearDATA(char dest[], t_BLOCK *BLOCK, size_t i);

void fetchRandomDATA(char dest[], t_BLOCK *BLOCK);

void stringSwap(char **str1_ptr, char **str2_ptr);

void shuffleArray(t_BLOCK *BLOCK);
