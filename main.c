#include "src/data_fetch.h"
#include "src/manipulate_data.h"

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <time.h>
#include <assert.h>

typedef struct client {
    char *name;
    char *surname[3]; //for surname, maximum of 3
    char *id;
    char *agency;
    char *account;
} t_subj;

void getUsers(FILE* f_out, t_BLOCK *addr[]);

int main(void)
{
    srand(time(NULL)); //generate unique seed for generator on each run

    char *locale = setlocale(LC_CTYPE, ""); //set locale to UTF8

    FILE *f_name; //list of brazilian names
    FILE *f_surname; //list of brazilian surnames
    FILE *f_psw; //list of passwords
    FILE *f_out; //generated individuals

    size_t i;    

    t_HEAP HEAP;
    t_BLOCK b_name, b_surname, b_psw, b_ID;

    assert(locale);
    fprintf(stderr,"Locale is set to %s\n",locale);
    
    initBLOCK(&b_name, &HEAP); //init and assign b_name to HEAP.addr[0]
    initBLOCK(&b_surname, &HEAP); //init and assign b_surname to HEAP.addr[1]
    initBLOCK(&b_ID, &HEAP); //init and assign b_ID to HEAP.addr[2]
    initBLOCK(&b_psw, &HEAP); //init and assign b_psw to HEAP.addr[3]
    
    f_name = fopen("content/nomes.txt", "r"); 
    assert(f_name);
    fileToBLOCK(f_name, HEAP.addr[NAME], 0);
    fclose(f_name);    

    f_surname = fopen("content/sobrenomes.txt", "r"); 
    assert(f_surname);
    fileToBLOCK(f_surname, HEAP.addr[SURNAME], 0);    
    fclose(f_surname);

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
    t_node *new_node = NULL;
    t_subj client;
    char s_agency[AMT_3][LENGTH_3], s_account[AMT_4][LENGTH_4];

    char *acc, *psw;
    size_t i, temp_i;
    
    char folder[20];
    char NIL = '\0';
    FILE *f_temp;

    numsToSTACK(MIN_3, MAX_3, AMT_3, LENGTH_3, s_agency);
    numsToSTACK(MIN_4, MAX_4, AMT_4, LENGTH_4, s_account);

    for ( i = 0 ; i < AMT_3 ; ++i ){ //initialize tree for dependent data
        initTree(T+i, s_agency[i]);
    }
    
    shuffleArray(addr[ID]);
    for ( i = 0 ; i < DBSIZE ; ++i ){

        // THIS CREATES ATTRIBUTE 0 AND 1
        client.name = pickRandom(addr[NAME]);
        client.surname[0] = pickRandom(addr[SURNAME]);
        if ( strlen(client.surname[0]) > 3 ){
            client.surname[1] = &NIL;
        } else {
            client.surname[1] = pickRandom(addr[SURNAME]);
        }

        // THIS CREATES ATTRIBUTE 2
        client.id = pickIndex(addr[ID], i);

        //THIS CREATES ATTRIBUTE 3 AND 4 (3 is a requisite for 4)
        temp_i = rand() % AMT_3;
        acc = find_xData(T+temp_i, AMT_4, LENGTH_4, s_account);
        assert(acc); //can't find exclusive members to tree
        psw = pickRandom(addr[PSW]);
        assert(psw);

        insertNode( T+temp_i, initNode(new_node), "%s %s", acc, psw );

        client.agency = s_agency[temp_i];
        client.account = acc;
        // PRINTS DATABASE TO OUTPUT STREAM 
        fprintf(f_out,"%s,%s,%s,%s %s %s\n",client.id,
                                            client.agency, 
                                            client.account, 
                                            client.name,
                                            client.surname[0],
                                            client.surname[1]);
        free(client.id); //since each pointer will only be used once
    }
    addr[ID]->size -= i;

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
