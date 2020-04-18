#include <stdio.h>

#define DBSIZE 100000
#define TOTAL_BLOCKS 5 //max possible blocks in heap

#define NAME 0 // << change define name to whatever you want to represent there
#define LENGTH_0 25
//#define MIN_0
//#define MAX_0
//#define AMT_0

#define SURNAME 1
#define LENGTH_1 25
//#define MIN_1
//#define MAX_1
//#define AMT_1

#define ID 2
#define LENGTH_2 13
#define MIN_2 70000000000L
#define MAX_2 99999999999L
#define AMT_2 DBSIZE

#define AGENCY 3
#define LENGTH_3 7
#define MIN_3 10000
#define MAX_3 99999
#define AMT_3 25 //qtd de agências

#define ACCOUNT 4
#define LENGTH_4 7
#define MIN_4 10000
#define MAX_4 99999
#define AMT_4 89999

#define PSW 3
#define LENGTH_5 9
//#define MIN_5
//#define MAX_5
//#define AMT_5

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

void fileToBLOCK(FILE* f_read, t_BLOCK* BLOCK, size_t lntotal);

void numsToHEAP(t_BLOCK *BLOCK, long int first, long int last, size_t amount, size_t digits);

void numsToSTACK(long int first, long int last, size_t amount, size_t digits, char STACK[][digits]);

void stringSwap(char **str1_ptr, char **str2_ptr);

int badAlloc(void *ptr);
