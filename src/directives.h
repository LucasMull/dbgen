#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <locale.h>
#include <time.h>
#include <limits.h>

#define DBSIZE 100000
#define TOTAL_BLOCKS 5 //maximum possible blocks in heap

#define NAME 0 // change define name to whatever you want to represent there
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
