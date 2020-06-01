#include "config.h"
#include "dbgen.h"

static short add_link_cols(t_colinfo*, short);

int main(int argc, char *argv[])
{
    dbconfig database = {
        .size = 10, 
        .delim = ',', 
        .buffer = {0}, 
        .amt_cols = 0,
        .out_stream = NULL
    };

    t_colinfo *info = parser(argc, argv, &database);

    short info_amt_cols = database.amt_cols;
    database.amt_cols = add_link_cols(info, info_amt_cols);

    t_colgen **arrcolgen = start_arrcolgen(info, &database);

    print_colinfo(info,info_amt_cols);
    clean_colinfo(info,info_amt_cols);
    
    print_arrcolgen(arrcolgen, database.amt_cols);
    destroy_arrcolgen(arrcolgen, database.amt_cols);    

    return EXIT_SUCCESS;
}

static short add_link_cols(t_colinfo *info, short info_amt_cols)
{
    short amt_total = info_amt_cols;

    for ( int i = 0; i < info_amt_cols; ++i ){
        if ( (info+i)->link ){
            t_colinfo *ptr_link = (info+i)->link;
            while ( ptr_link ){
                ptr_link = ptr_link->link;
                ++amt_total; 
            }
        }
    }

    return amt_total;
}
