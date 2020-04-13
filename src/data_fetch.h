#include <stdio.h>

//DBSIZE limit seems to be at 90~100k range
//depends on how much you end up alloc'ing
#define DBSIZE 330000
#define TOTAL_BLOCKS 5 //max possible blocks in heap

#define NAME 0 // << change define name to whatever you want to represent there
#define LENGTH_0 25
#define MIN_0
#define MAX_0
#define AMT_0

#define SURNAME 1
#define LENGTH_1 25
#define MIN_1
#define MAX_1
#define AMT_1

#define ID 2
#define LENGTH_2 11
#define MIN_2 700000000
#define MAX_2 999999999
#define AMT_2 DBSIZE

#define AGENCY 3
#define LENGTH_3 7
#define MIN_3 10000
#define MAX_3 99999
#define AMT_3 5

#define ACCOUNT 4
#define LENGTH_4 7
#define MIN_4 10000
#define MAX_4 99999
#define AMT_4 89999

#define STRLEN 25
#define UNDEFINED 0


typedef struct BLOCK {
    size_t size;
    char **data;
} t_BLOCK;

typedef struct HEAP {
    t_BLOCK *addr[TOTAL_BLOCKS];
    size_t size;
} t_HEAP;

void initBLOCK(t_BLOCK *BLOCK, t_HEAP *HEAP);

void freeBLOCK(t_BLOCK *BLOCK, size_t first_i);

void fileToBLOCK(FILE* f_read, t_BLOCK* BLOCK);

void numsToHEAP(t_BLOCK *BLOCK, int first, int last, size_t amount, size_t digits);

void numsToSTACK(int first, int last, size_t amount, size_t digits, char STACK[][digits]);

void stringSwap(char **str1_ptr, char **str2_ptr);

int badAlloc(void *ptr);
