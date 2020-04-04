#include "generator.h"
#include "columns.h"

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
    initBLOCK(&b_ID, &HEAP, 7); //init and assign b_ID to HEAP.addr[2]
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
  
    randNumToBLOCK(HEAP.addr[2], 1, DBSIZE, UNDEFINED);
    randNumToBLOCK(HEAP.addr[3], 1, AGENCYSIZE, 5);
    
    f_out = fopen("people.csv", "a");
    getUsers(f_out, HEAP.addr, &database);
    fclose(f_out);

    for ( size_t i=0; i<=HEAP.size; ++i )
        freeBLOCK(HEAP.addr[i], 0);

    return 0;
}
