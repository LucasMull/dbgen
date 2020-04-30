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

void getUsers(FILE* f_out, t_BLOCK *addr[]);

int main(void)
{
    srand(time(NULL)); //generate unique seed for generator on each run

    char *locale = setlocale(LC_CTYPE, ""); //set locale to UTF8

    FILE *f_name; //list of brazilian names
    FILE *f_surnam; //list of brazilian surnames
    FILE *f_psw; //list of passwords
    FILE *f_out; //generated individuals

    size_t i;    

    t_HEAP HEAP;
    t_BLOCK b_name, b_surnam, b_psw, b_ID;

    assert(locale);
    fprintf(stderr,"Locale is set to %s\n",locale);
    
    initBLOCK(&b_name, &HEAP); //init and assign b_name to HEAP.addr[0]
    initBLOCK(&b_surnam, &HEAP); //init and assign b_surnam to HEAP.addr[1]
    initBLOCK(&b_ID, &HEAP); //init and assign b_ID to HEAP.addr[2]
    initBLOCK(&b_psw, &HEAP); //init and assign b_psw to HEAP.addr[3]
    
    f_name = fopen("content/nomes.txt", "r"); 
    assert(f_name);
    fileToBLOCK(f_name, HEAP.addr[NAME], 0);
    fclose(f_name);    

    f_surnam = fopen("content/sobrenomes.txt", "r"); 
    assert(f_surnam);
    fileToBLOCK(f_surnam, HEAP.addr[SURNAME], 0);    
    fclose(f_surnam);

    f_psw = fopen("content/passwords.txt", "r"); 
    assert(f_psw);
    fileToBLOCK(f_psw, HEAP.addr[PSW], DBSIZE);    
    fclose(f_psw);
  
    numsToHEAP(&b_ID, MIN_2, MAX_2, AMT_2, LENGTH_2);

    f_out = fopen("data.csv", "w");
    assert(f_out);
    getUsers(f_out, HEAP.addr); //the brain
    fclose(f_out);

    for ( i = 0 ; i <= HEAP.size ; ++i ){
        freeBLOCK(HEAP.addr[i], 0);
    }

    return 0;
}

//This function needs to be updated and be made into a new library, not modularized enough, basically compiles every information and print output into file
void getUsers(FILE* f_out, t_BLOCK *addr[])
{
    t_tree T[AMT_3];
    t_node *new_node;
    t_subj subject;
    char s_agency[AMT_3][LENGTH_3], s_account[AMT_4][LENGTH_4];

    char NIL = '\0';
    char *acc;
    size_t i, temp_i;
    
    char folder[20];
    FILE *f_temp;

    numsToSTACK(MIN_3, MAX_3, AMT_3, LENGTH_3, s_agency);
    numsToSTACK(MIN_4, MAX_4, AMT_4, LENGTH_4, s_account);

    for ( i = 0 ; i < AMT_3 ; ++i ){ //initialize tree for dependent data
        initTree(T+i, s_agency[i]);
    }
    
    shuffleArray(addr[ID]);
    for ( i = 0 ; i < DBSIZE ; ++i ){

        // THIS CREATES ATTRIBUTE 0 AND 1
        subject.attribute_0 = pickRandom(addr[NAME]);
        subject.attribute_1[0] = pickRandom(addr[SURNAME]);
        if ( strlen(subject.attribute_1[0]) > 3 ){
            subject.attribute_1[1] = &NIL;
        } else {
            subject.attribute_1[1] = pickRandom(addr[SURNAME]);
        }

        // THIS CREATES ATTRIBUTE 2
        subject.attribute_2 = pickIndex(addr[ID], i);

        //THIS CREATES ATTRIBUTE 3 AND 4 (3 is a requisite for 4)
        temp_i = rand() % AMT_3;
        acc = find_xData(T+temp_i, AMT_4, LENGTH_4, s_account);
        assert(acc); //can't find exclusive members to tree

        //ARRUMAR j SOLTO QUE DEVERIA VIR DE exclToTree()
        insertNode( T+temp_i, initNode(new_node), acc, pickRandom(addr[PSW]) );

        subject.attribute_3 = s_agency[temp_i];
        subject.attribute_4 = acc;
        // PRINTS DATABASE TO OUTPUT STREAM 
        fprintf(f_out,"%s,%s,%s,%s %s %s\n",subject.attribute_2,
                                            subject.attribute_3, 
                                            subject.attribute_4, 
                                            subject.attribute_0,
                                            subject.attribute_1[0],
                                            subject.attribute_1[1]);
        free(subject.attribute_2);
    }
    addr[ID]->size -= i;
    assert( addr[ID]->size == 0 );

    system("rm -rf clients; mkdir -p clients");
    for ( i = 0 ;  i < AMT_3 ; ++i ){ //print every tree node
        strcpy(folder,"clients/");
        strcat(folder, T[i].data);
        f_temp = fopen(folder, "a");
        printTree(T[i].root, f_temp);
        folder[0] = '\0';
        fclose(f_temp);
    }

    for ( i = 0 ; i < AMT_3 ; ++i ){ //free every tree node
        eraseTree(T[i].root, T+i);
    }
}
