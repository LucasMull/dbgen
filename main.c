#include "config.h"
#include "dbgen.h"

int main(int argc, char *argv[])
{
    dbconfig *database = init_dbconfig();
    t_colinfo *info = parser(argc, argv, database);

    t_colgen **arrcolgen = start_arrcolgen(info, database);
    print_colinfo(info,database->amt_cols);
    destroy_colinfo(info,database->amt_cols);

    print_arrcolgen(arrcolgen, database->amt_cols);
    for ( unsigned int i = 0; i < database->amt_rows; ++i ){
        for ( unsigned short j = 0; j < database->amt_cols; ++j ){
            if ( arrcolgen[j]->fn ){
                (arrcolgen[j]->fn)(arrcolgen[j], database);
            }
        }
    }

    destroy_arrcolgen(arrcolgen, database->amt_cols);    
    destroy_dbconfig(database);

    return EXIT_SUCCESS;
}
