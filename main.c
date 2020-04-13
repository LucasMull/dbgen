#include "src/data_fetch.h"
#include "src/manipulate_data.h"

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <time.h>
#include <assert.h>

typedef struct subject {
    char *attribute_0;
    char *attribute_1[3]; //for surname, maximum of 3
    char *attribute_2;
    char *attribute_3;
    char *attribute_4;
    //int attribute_5;
    //void *attribute_6;
    // create or remove attributes as needed
} t_subj;

void getUsers(FILE* f_out, t_BLOCK *addr[],
              char s_agency[][STRLEN], char s_account[][STRLEN]);

int main(void)
{
    srand(time(NULL)); //generate unique seed for generator on each run

    char *locale = setlocale(LC_CTYPE, ""); //set locale to UTF8

    FILE *f_name; //list of brazilian names
    FILE *f_surnam; //list of brazilian surnames
    FILE *f_out; //generated individuals

    size_t i;    

    t_HEAP HEAP;
    t_BLOCK b_name, b_surnam, b_ID;

    char s_ID[AMT_2][STRLEN], s_agency[AMT_3][STRLEN], s_account[AMT_4][STRLEN];

    if ( !locale ){
        fprintf(stderr,"Couldn't set locale");
    } else {
        fprintf(stderr,"Locale is set to %s\n",locale);
        free(locale);
    }
    
    initBLOCK(&b_name, &HEAP); //init and assign b_name to HEAP.addr[0]
    initBLOCK(&b_surnam, &HEAP); //init and assign b_surnam to HEAP.addr[1]
    initBLOCK(&b_ID, &HEAP); //init and assign b_ID to HEAP.addr[2]
    
    f_name = fopen("content/nomes.txt", "r"); 
    f_surnam = fopen("content/sobrenomes.txt", "r"); 
    if ( !f_name )
        fprintf(stderr,"Couldn't open nomes.txt");
    if ( !f_surnam )
        fprintf(stderr,"Couldn't open sobrenomes.txt");
  
    fileToBLOCK(f_name, HEAP.addr[NAME]);
    fclose(f_name);    
    fileToBLOCK(f_surnam, HEAP.addr[SURNAME]);    
    fclose(f_surnam);

    numsToHEAP(&b_ID, MIN_2, MAX_2, AMT_2, LENGTH_2);
    numsToSTACK(s_agency, MIN_3, MAX_3, AMT_3, LENGTH_3);
    numsToSTACK(s_account, MIN_4, MAX_4, AMT_4, LENGTH_4);

    f_out = fopen("data.csv", "w");
    getUsers(f_out, HEAP.addr, s_agency, s_account); //the brain
    fclose(f_out);

    for ( i=0; i<=HEAP.size; ++i )
        freeBLOCK(HEAP.addr[i], 0);

    return 0;
}

//This function needs to be updated and be made into a new library, not modularized enough, basically compiles every information and print output into file
void getUsers(FILE* f_out, t_BLOCK *addr[], 
              char s_agency[][STRLEN], char s_account[][STRLEN])
{
    t_tree T[AMT_3];    
    t_subj subject;

    char *temp_str;
    char NIL = '\0';
    char *child;
    size_t i, rand_i, rand_j;
    
    for ( i=0; i<AMT_3; ++i ){
        initTree(T+i, s_agency[i]);
    }
    
    shuffleArray(addr[ID]);
    for ( i=0; i<DBSIZE; ++i ){

        // THIS CREATES ATTRIBUTE 0 AND 1
        subject.attribute_0 = pickRandom(addr[NAME]);
        subject.attribute_1[0] = pickRandom(addr[SURNAME]);
        if (strlen(subject.attribute_1[0]) <= 3){
            subject.attribute_1[1] = pickRandom(addr[SURNAME]);
        } else subject.attribute_1[1] = &NIL;

        // THIS CREATES ATTRIBUTE 2
        subject.attribute_2 = fetchLinear(addr[ID], i);

        //THIS CREATES ATTRIBUTE 3 AND 4 (3 is a requisite for 4)
        rand_i = rand()%AMT_3;
        do{
            rand_j = rand()%AMT_4;
            while ( rand_j < AMT_4 ){
                if ( child = uniqueChild(T+rand_i,s_account[rand_j]) )
                    break;
                ++rand_j;
            }
        } while (!child);
        insertChild(T+rand_i, child);
        subject.attribute_3 = s_agency[rand_i];
        subject.attribute_4 = child;
         
        // PRINTS DATABASE TO OUTPUT STREAM 
        fprintf(f_out,"%s,%s,%s,%s %s %s\n",subject.attribute_2,
                                            subject.attribute_3, 
                                            subject.attribute_4, 
                                            subject.attribute_0,
                                            subject.attribute_1[0],
                                            subject.attribute_1[1]);
    }

    for ( i=0; i<AMT_3; ++i ){
        eraseTree(T[i].root, T+i);
    }
}
