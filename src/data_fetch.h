#ifndef FILE_H
#define FILE_H
#include "directives.h"
#endif

typedef struct BLOCK {
    char **data;
    size_t size;
} t_BLOCK;

typedef struct HEAP {
    t_BLOCK *addr[TOTAL_BLOCKS];
    size_t size;
} t_HEAP;

void initBLOCK(t_BLOCK *BLOCK, t_HEAP *HEAP);

void freeBLOCK(t_BLOCK *BLOCK, size_t first_i);

void fileToBLOCK(FILE* f_read, t_BLOCK* BLOCK, size_t lntotal);

void numsToHEAP(t_BLOCK *BLOCK, long int first, long int last, size_t amount, size_t digits);

void numsToSTACK(long int first, long int last, size_t amount, size_t digits, char STACK[][digits]);
