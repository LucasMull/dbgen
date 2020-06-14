#include "config.h"
#include "dbgen.h"

int main(int argc, char *argv[])
{
    dbconfig *database = init_dbconfig();
    t_colinfo *info = parser(argc, argv, database);
    t_colgen **arrcolgen = start_arrcolgen(info, database);

    print_colinfo(info,database->amt_col);

    destroy_colinfo(info,database->amt_col);

    print_arrcolgen(arrcolgen, database->amt_col);

    generate_database(arrcolgen, database);

    destroy_arrcolgen(arrcolgen, database->amt_col);    

    destroy_dbconfig(database);

    return EXIT_SUCCESS;
}
