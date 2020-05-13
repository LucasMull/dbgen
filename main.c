#include "src/data_fetch.h"
#include "src/manipulate_data.h"

/* THIS IS INCLUDED IN MAIN.C JUST TO DOCUMENT WHICH LIBRARIES I'M USING*/
#ifndef FILE_H
#define FILE_H
#include "src/directives.h"
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#endif

typedef struct client {
    char fullname[50];
    char *id;
    char *agency;
    char *account;
} t_client;

void get_users(FILE* f_out, t_hblock *addr[]);

int main(void)
{
    srand(time(NULL)); //generate unique seed for generator on each run

    char *locale = setlocale(LC_CTYPE, ""); //set locale to UTF8

    FILE *f_name; //list of brazilian names
    FILE *f_surname; //list of brazilian surnames
    FILE *f_psw; //list of passwords
    FILE *f_out; //generated individuals

    size_t i;    

    t_heap heap;
    t_hblock name, surname, psw, id;

    assert(locale);
    fprintf(stderr,"Locale is set to %s\n",locale);
    
    init_h(&name, &heap); //init and assign name to heap.addr[0]
    init_h(&surname, &heap); //init and assign surname to heap.addr[1]
    init_h(&id, &heap); //init and assign id to heap.addr[2]
    init_h(&psw, &heap); //init and assign psw to heap.addr[3]
    
    f_name = fopen("content/nomes.txt", "r"); 
    assert(f_name);
    file_to_h(f_name, heap.addr[NAME], 0);
    fclose(f_name);    

    f_surname = fopen("content/sobrenomes.txt", "r"); 
    assert(f_surname);
    file_to_h(f_surname, heap.addr[SURNAME], 0);    
    fclose(f_surname);

    f_psw = fopen("content/passwords.txt", "r"); 
    assert(f_psw);
    file_to_h(f_psw, heap.addr[PSW], DBSIZE);    
    fclose(f_psw);
  
    nums_to_h(&id, MIN_2, MAX_2, AMT_2, LENGTH_2);

    f_out = fopen("data.csv", "w");
    assert(f_out);
    get_users(f_out, heap.addr); //the brain
    fclose(f_out);

    i = 0;
    while ( i <= heap.size ){
        free_h(heap.addr[i++], 0);
    }

    return 0;
}

//requires modularization
void get_users(FILE* f_out, t_hblock *addr[])
{
    t_tree tree[AMT_3];
    t_node *new_node = NULL;
    t_client client;
    t_sblock agency[AMT_3][LENGTH_3], account[AMT_4][LENGTH_4];

    char *acc, *psw;
    size_t i, temp_i;
    
    char folder[20];
    size_t length;
    FILE *f_temp;

    nums_to_s(MIN_3, MAX_3, AMT_3, LENGTH_3, agency);
    nums_to_s(MIN_4, MAX_4, AMT_4, LENGTH_4, account);

    for ( i = 0 ; i < AMT_3 ; ++i ){ //initialize tree for dependent data
        init_tree(tree+i, agency[i]);
    }
    
    shuffle_array(addr[ID]);

    for ( i = 0 ; i < DBSIZE ; ++i ){
        // THIS PICKS CLIENT'S FULLNAME
        strncpy(client.fullname, pick_random(addr[NAME]), 15);
        length = strlen(client.fullname);
        strcat(client.fullname, " ");
        strncat(client.fullname, pick_random(addr[SURNAME]), 15);
        if ( (strlen(client.fullname) - length) < 4){ //add another surname
            strcat(client.fullname, " ");
            strncat(client.fullname, pick_random(addr[SURNAME]), 15);
        }
        if ( rand() % 5 == 1 ){
            strcat(client.fullname, " ");
            strncat(client.fullname, pick_random(addr[SURNAME]), 15); 
        }

        // THIS PICKS CLIENT'S ID
        client.id = pick_index(addr[ID], i);

        //THIS PICKS CLIENT'S AGENCY AND ACCOUNT (agency is a requisite for account)
        temp_i = rand() % AMT_3;
        acc = find_xdata(tree+temp_i, AMT_4, LENGTH_4, account);
        assert(acc); //can't find exclusive members to tree
        psw = pick_random(addr[PSW]);
        assert(psw);

        insert_node( tree+temp_i, init_node(new_node), "%s %s", acc, psw );

        client.agency = agency[temp_i];
        client.account = acc;
        // PRINTS DATABASE TO OUTPUT STREAM
        fprintf(f_out,"%s,%s,%s,%s\n",client.id,
                                      client.agency, 
                                      client.account, 
                                      client.fullname
        );
        free(client.id); //because each pointer will only be used once
    } addr[ID]->size -= i;

    system("rm -rf clients; mkdir -p clients");
    for ( i = 0 ;  i < AMT_3 ; ++i ){ //print every tree node
        strcpy(folder,"clients/");
        strcat(folder, tree[i].tag);
        f_temp = fopen(folder, "a");
        print_tree(tree[i].root, f_temp);
        folder[0] = '\0';
        fclose(f_temp);
    }

    for ( i = 0 ; i < AMT_3 ; ++i ){ //free every tree node
        erase_tree(tree[i].root, tree+i);
    }
}
