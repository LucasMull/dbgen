#include "src/data_fetch.h"
#include "src/manipulate_data.h"

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <time.h>


#define AGENCYSIZE 10000


int main(void)
{
    srand(time(NULL)); //generate unique seed for generator on each run

    char *locale = setlocale(LC_CTYPE, ""); //set locale to UTF8

    FILE *f_name; //list of brazilian names
    FILE *f_surnam; //list of brazilian surnames
    FILE *f_out; //generated individuals

    size_t i;    

    t_DB database;
    t_HEAP HEAP;
    t_BLOCK b_name, b_surnam, b_ID, b_agency;

    
    if ( !locale ){
        fprintf(stderr,"Couldn't set locale");
    } else {
        fprintf(stderr,"Locale is set to %s\n",locale);
        free(locale);
    }
    
    initBLOCK(&b_name, &HEAP, 0); //init and assign b_name to HEAP.addr[0]
    initBLOCK(&b_surnam, &HEAP, 0); //init and assign b_surnam to HEAP.addr[1]
    initBLOCK(&b_ID, &HEAP, 12); //init and assign b_ID to HEAP.addr[2]
    initBLOCK(&b_agency, &HEAP, 6); //init and assign b_agency to HEAP.addr[3]

    f_name = fopen("content/nomes.txt", "r"); 
    f_surnam = fopen("content/sobrenomes.txt", "r"); 
    if ( !f_name )
        fprintf(stderr,"Couldn't open nomes.txt");
    if ( !f_surnam )
        fprintf(stderr,"Couldn't open sobrenomes.txt");

    fileToBLOCK(f_name, HEAP.addr[0]);
    fclose(f_name);    
    fileToBLOCK(f_surnam, HEAP.addr[1]);    
    fclose(f_surnam);
  
    NumsToBLOCK(HEAP.addr[2], 700000000, 900000000, DBSIZE);
    NumsToBLOCK(HEAP.addr[3], 1000, AGENCYSIZE, 5);
    
    f_out = fopen("data.csv", "a");
    getUsers(f_out, HEAP.addr, &database);
    fclose(f_out);

    for ( i=0; i<=HEAP.size; ++i )
        freeBLOCK(HEAP.addr[i], 0);

    return 0;
}
