#include <stdio.h>

#define DBSIZE 100000
#define NAMELENGTH 50

#define TOTAL_BLOCKS 5
#define STRLENGTH 15
#define UNDEFINED 0


typedef struct BLOCK {
    size_t size;
    size_t digits;
    char **data;
} t_BLOCK;

typedef struct HEAP {
    t_BLOCK *addr[10];
    size_t size;
} t_HEAP;

typedef struct user {
    char ID[7];
    char agency[5];
    char account[5];
    char fullname[NAMELENGTH];
} t_user;

typedef struct DB {
    t_user user[DBSIZE];
} t_DB;

void initBLOCK(t_BLOCK *BLOCK, t_HEAP *HEAP, int digits);

void freeBLOCK(t_BLOCK *BLOCK, size_t first_i);

void fileToBLOCK(FILE* f_read, t_BLOCK* BLOCK);

void randNumToBLOCK(t_BLOCK *BLOCK, int start, int scope, size_t amount);

void stringSwap(char **str1_ptr, char **str2_ptr);

