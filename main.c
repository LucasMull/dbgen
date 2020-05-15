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

enum h_addr { Name, Surname, Id, Psw }; 

typedef struct client {
    char fullname[50];
    char *id;
    char *agency;
    char *account;
} t_client;

static t_sblock agency[AMT_3][LENGTH_3], account[AMT_4][LENGTH_4];

int main(void)
{
    char *locale = setlocale(LC_CTYPE, ""); //set locale to UTF8
    assert(locale);
    fprintf(stderr,"Locale is set to %s\n",locale);

    void get_users(FILE* f_out, t_hblock *addr[]);
    srand(time(NULL)); //generate unique seed for generator on each run

    t_heap heap;
    t_hblock name, surname, psw, id;

    init_h(&name, &heap);
    init_h(&surname, &heap);
    init_h(&id, &heap);
    init_h(&psw, &heap);
    { /* FILE FETCHING BLOCK */ 
        FILE *f_name = fopen("content/nomes.txt", "r"); 
        assert(f_name);
        file_to_h(f_name, heap.addr[Name], 0);
        fclose(f_name);    

        FILE *f_surname = fopen("content/sobrenomes.txt", "r"); 
        assert(f_surname);
        file_to_h(f_surname, heap.addr[Surname], 0);    
        fclose(f_surname);

        FILE *f_psw = fopen("content/passwords.txt", "r"); 
        assert(f_psw);
        file_to_h(f_psw, heap.addr[Psw], DBSIZE);    
        fclose(f_psw);
    }

    nums_to_h(&id, MIN_2, MAX_2, AMT_2, LENGTH_2);
    nums_to_s(MIN_3, MAX_3, AMT_3, LENGTH_3, agency);
    nums_to_s(MIN_4, MAX_4, AMT_4, LENGTH_4, account);
    { /* DATABASE CREATION BLOCK */
        FILE *f_out = fopen("data.csv", "w");
        assert(f_out);
        get_users(f_out, heap.addr); //the brain
        fclose(f_out);
    }

    size_t i = 0;
    while ( i <= heap.size ){
        free_h(heap.addr[i++], 0);
    } heap.size = 0;

    return 0;
}


void get_users(FILE* f_out, t_hblock *addr[])
{
    t_node *new_node = NULL;
    t_tree tree[AMT_3];

    for ( size_t i = 0 ; i < AMT_3 ; ++i ){ //initialize tree for dependent data
        init_tree(tree+i, agency[i]);
    }
    
    shuffle_array(addr[Id]);

    for ( size_t i = 0 ; i < DBSIZE ; ++i ){
        t_client client;
        char *acc, *psw;
        size_t temp_i, length;

        // THIS PICKS CLIENT'S FULLNAME
        strncpy(client.fullname, pick_random(addr[Name]), 15);
        length = strlen(client.fullname);
        strcat(client.fullname, " ");
        strncat(client.fullname, pick_random(addr[Surname]), 15);
        if ( (strlen(client.fullname) - length) < 4){ //add another surname
            strcat(client.fullname, " ");
            strncat(client.fullname, pick_random(addr[Surname]), 15);
        }
        if ( rand() % 5 == 1 ){
            strcat(client.fullname, " ");
            strncat(client.fullname, pick_random(addr[Surname]), 15); 
        }

        // THIS PICKS CLIENT'S ID
        client.id = pick_index(addr[Id], i);

        //THIS PICKS CLIENT'S AGENCY AND ACCOUNT (agency is a requisite for account)
        temp_i = rand() % AMT_3;
        acc = find_xdata(tree+temp_i, AMT_4, LENGTH_4, account);
        assert(acc); //can't find exclusive members to tree
        psw = pick_random(addr[Psw]);
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
        free(client.id); //since each id will only be used once
    } addr[Id]->size -= DBSIZE;

    system("rm -rf clients; mkdir -p clients");
    for ( size_t i = 0 ;  i < AMT_3 ; ++i ){ //print every tree node
        FILE *f_temp;
        char folder[20];

        strcpy(folder,"clients/");
        strcat(folder, tree[i].tag);
        f_temp = fopen(folder, "a");
        print_tree(tree[i].root, f_temp);
        folder[0] = '\0';
        fclose(f_temp);
    }

    for ( size_t i = 0 ; i < AMT_3 ; ++i ){ //free every tree node
        erase_tree(tree[i].root, tree+i);
    }
}
