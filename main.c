#include "config.h"
#include "dbgen.h"

static short count_link_cols(t_colinfo*, short);

int main(int argc, char *argv[])
{
    dbconfig database = {
        .amt_rows = 10000000, 
        .delim = ',', 
        .buffer = {0}, 
        .amt_cols = 0,
        .out_stream = NULL
    };
    // TEST ONLY
    database.out_stream = fopen("data.csv", "a");

    t_colinfo *info = parser(argc, argv, &database);

    //current col count without taking links into account
    short info_amt_cols = database.amt_cols;
    //add link count to current count
    database.amt_cols += count_link_cols(info, info_amt_cols);

    t_colgen **arrcolgen = start_arrcolgen(info, &database);
    //print_colinfo(info,info_amt_cols);
    clean_colinfo(info,info_amt_cols);

    //print_arrcolgen(arrcolgen, database.amt_cols);
    for ( unsigned int i = 0; i < database.amt_rows; ++i ){
        for ( unsigned short j = 0; j < database.amt_cols; ++j ){
            if ( arrcolgen[j]->fn ){
                (arrcolgen[j]->fn)(arrcolgen[j], &database);
            }
        }
    }

    destroy_arrcolgen(arrcolgen, database.amt_cols);    

    fclose(database.out_stream);

    return EXIT_SUCCESS;
}

static short count_link_cols(t_colinfo *info, short info_amt_cols)
{
    short amt_link = 0;

    for ( int i = 0; i < info_amt_cols; ++i ){
        if ( (info+i)->link ){
            t_colinfo *ptr_link = (info+i)->link;
            while ( ptr_link ){
                ptr_link = ptr_link->link;
                ++amt_link; 
            }
        }
    }

    return amt_link;
}
