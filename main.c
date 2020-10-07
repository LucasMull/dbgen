#include <stdio.h>
#include <stdlib.h>

#include "libdbgen.h"

int main(int argc, char *argv[])
{
    dbconfig *database = dbgen_init_dbconfig();
    t_colinfo *info = dbgen_parser(argc, argv, database);
    t_colgen **arrcolgen = dbgen_build_arrcolgen(info, database);

    dbgen_print_colinfo(info,database->amt_col);

    dbgen_destroy_colinfo(info,database->amt_col);

    dbgen_print_arrcolgen(arrcolgen, database->amt_col);

    dbgen_generate_database(arrcolgen, database);

    dbgen_destroy_arrcolgen(arrcolgen, database->amt_col);    

    dbgen_destroy_dbconfig(database);

    return EXIT_SUCCESS;
}
