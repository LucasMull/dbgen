#include <stdio.h>

//limit seems to be at 90~100k range
//depends on how much you end up alloc'ing
#define DBSIZE 100000
#define LENGTH_0 12
#define LENGTH_1 6
#define LENGTH_2 6
#define LENGTH_3 50

#define TOTAL_BLOCKS 5
#define STRLENGTH 25
#define UNDEFINED 0


typedef struct BLOCK {
    size_t heap_id;
    size_t size;
    size_t digits;
    char **data;
} t_BLOCK;

typedef struct HEAP {
    t_BLOCK *addr[TOTAL_BLOCKS];
    size_t size;
} t_HEAP;

typedef struct subject {
    char *attribute_0;
    char *attribute_1;
    char *attribute_2;
    char attribute_3[LENGTH_3];
    // create or remove attributes as needed
} t_subj;

void initBLOCK(t_BLOCK *BLOCK, t_HEAP *HEAP, int digits);

void freeBLOCK(t_BLOCK *BLOCK, size_t first_i);

void fileToBLOCK(FILE* f_read, t_BLOCK* BLOCK);

void NumsToBLOCK(t_BLOCK *BLOCK, int start, int scope, size_t amount);

void stringSwap(char **str1_ptr, char **str2_ptr);

int badAlloc(void *ptr);
