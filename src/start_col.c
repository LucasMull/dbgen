#include "../config.h"
#include "../dbgen.h"

enum gentype {
    Template=1, 
    List=2,
    File=4,

    TotalGens=3
};

enum methodtype { 
    Rnd=1, 
    Unq=2, 
    Scl=4, 

    TotalMethods=3,
    Undef=0
};


t_list *init_list()
{
    t_list *new_list = malloc(sizeof(t_list));
    assert(new_list);

    const t_list default_list = { NULL };
    *new_list = default_list;

    return new_list;
}

void start_arrlist(t_colgen *colgen, dbconfig *database)
{
    assert(!colgen->_list);
    assert(colgen->amt_row);

    t_list **new_arrlist = malloc(colgen->amt_row * sizeof(t_list*));
    assert(new_arrlist);

    for ( int i = 0; i < colgen->amt_row; ++i )
        new_arrlist[i] = init_list();
    
    if ( colgen->gentype & List ){
        if ( colgen->gentype & File ){
            file_to_arrlist(new_arrlist, colgen);
        } else {
            nums_to_arrlist(new_arrlist, colgen);
        }
    }

    colgen->_list = new_arrlist;
}

void destroy_arrlist(short gentype, t_list** arrlist, int amt_row)
{
    assert(arrlist);

    for ( int i = 0; i < amt_row; ++i ){
        destroy_list(arrlist[i], gentype);
    } 
    free(arrlist);
}

void destroy_list(t_list* list, short gentype)
{
    assert(list);

    if (( gentype == ( File | List )) && ( list->svalue ))
        free(list->svalue);
    if ( list->root )
        free(list->root);
    free(list);
}


t_templ *init_templ()
{
    t_templ *new_templ = malloc(sizeof(t_templ));
    assert(new_templ);

    const t_templ default_templ = { NULL };
    *new_templ = default_templ;

    return new_templ;
}

void start_templ(t_colgen* colgen, t_templ* new_templ, dbconfig* database)
{
    assert(colgen);
    assert(new_templ);
    assert(!colgen->_template);

    new_templ->dvalue = colgen->lwall;
    
    if (colgen->gentype & Template){
        if (colgen->gentype & File){
            //filesetter_templ(colgen);
        } else {
            numsetter_templ(colgen, database);
        }
    }
    
    colgen->_template = new_templ;
}

void destroy_templ(t_templ *templ)
{
    assert(templ);
    free(templ);
}

void def_typeof(t_colinfo *info, t_colgen *colgen)
{
    assert(info);
    assert(colgen);

    char letter[3] = {'r','u','s'}; //this needs to be made global

    short new_method = Undef;
    short new_gentype = Undef;

    enum methodtype methods = Rnd;
    for ( short i = 0; i < TotalMethods; ++i ){
        if ( strchr(info->option, letter[i])){
            new_method |= ( methods << i );
        }
    }
    
    //if new_method is random and also unique or scalable
    if (( new_method & Rnd ) && ( new_method & ( Unq | Scl ))){ 
        new_gentype = List; //then it must be a list
    } else {
        new_gentype = Template; //otherwise it's a template
    }

    if ( info->file ){
        if ( new_method & Rnd ){
            new_gentype = List;
        } 
        new_gentype |= File;
    }

    colgen->method = new_method;
    colgen->gentype = new_gentype;
}

t_colgen *init_colgen(dbconfig* database)
{
    t_colgen *new_colgen = malloc(sizeof(t_colgen));

    //everything else is set to NULL value
    const t_colgen default_colgen = { 
        .amt_row = database->amt_rows,
        .rwall = database->amt_rows,
        .delim = database->delim    
    };

    *new_colgen = default_colgen;

    return new_colgen;
}

static size_t count_flines(char *file)
{
    FILE *f_count = fopen(file, "r");
    const short LEN = 50;
    assert(f_count);

    char ptr_str[LEN];
    size_t ln;
    for( ln = 0; fgets(ptr_str, LEN-1, f_count); ++ln )
        continue;

    fclose(f_count);
    
    return ln;
}

t_colgen *start_colgen(t_colinfo* info, t_colgen* colgen, dbconfig* database)
{
    if ( info->file ){
        colgen->file = strdup(info->file);
    } else {
        if ( info->lwall )
            colgen->lwall = strtod(info->lwall, NULL);
        if ( info->rwall )
            colgen->rwall = strtod(info->rwall, NULL);
    }

    if ( info->delim )
        colgen->delim = info->delim;
    if ( info->decimals )
        colgen->decimals = info->decimals;

    def_typeof(info, colgen);
    /*
     * if is of file-list type and random method than it is necessary to
     * fetch the entire file into memory
     * (otherwise random elements scope would be limited to default db size)
     */
    if (( colgen->gentype == ( File | List )) && ( colgen->method & Rnd )){
        colgen->amt_row = count_flines(colgen->file);
    } // else if designated amount to be generated is lower than default db size
    else if (( info->amount ) && ( atoi(info->amount) < database->amt_rows )){
        colgen->amt_row = atoi(info->amount);
    } // else if range is lower than default db_size, assign range as amt
    else if (( colgen->rwall - colgen->lwall ) < database->amt_rows ){
        colgen->amt_row = colgen->rwall - colgen->lwall;
    }
    // else amt_row will be of default db size

    if ( colgen->gentype & File ){
        sprintf(colgen->format_data,"%%s%%c");
    } else {
        sprintf(colgen->format_data,"%%.%df%%c",colgen->decimals);
    }

    if ( colgen->gentype & List ){
        start_arrlist(colgen, database);
    } 
    else if ( colgen->gentype & Template ){
        start_templ(colgen, init_templ(), database);
    }

    return colgen;
}

t_colgen **start_arrcolgen(t_colinfo *info, dbconfig* database)
{
    assert(info);

    t_colgen **new_arrcolgen = malloc(database->amt_cols * sizeof(t_colgen*));
    assert(new_arrcolgen);

    int j = 0; //iterate through, and add to, colgen index
    int i = 0; //iterate through colinfo index
    while ( j < database->amt_cols ){
        new_arrcolgen[j] = start_colgen(info+i, init_colgen(database), database); 

        if ( (info+i)->link ){
            int linker_index = j; //hold onto the linker index
            t_colinfo *ptr_link = (info+i)->link;
            while ( ptr_link ){
                ++j;
                new_arrcolgen[j] = start_colgen(ptr_link, init_colgen(database), database);
                new_arrcolgen[j]->_linker = new_arrcolgen[linker_index];

                ptr_link = ptr_link->link;
            }
        }
        
        ++j;
        if ( i < database->amt_cols ){
            ++i;        
        }
    }
    // makes sure last col will not have delim and will jump line
    new_arrcolgen[database->amt_cols-1]->delim = '\n';

    return new_arrcolgen;
}

void print_list(size_t amt_rows, t_list **list, short gentype)
{
    assert(list);
    
    for ( int i = 0; i < 5; ++i ){
        if ( gentype == List ){
            fprintf(stderr, "\t# %f\n", list[i]->dvalue);
        }
        else if (( gentype == ( File | List )) && ( list[i]->svalue )){
            fprintf(stderr, "\t# %s\n", list[i]->svalue);
        }
    } fputc('\n', stderr);
    fprintf(stderr, "\tand %d more ...\n",abs(amt_rows-5));
}

void print_templ(t_templ *templ)
{
    assert(templ);

    fprintf(stderr, "\tvalue: %f\n", templ->dvalue);
}

void print_arrcolgen(t_colgen **colgen, int amt_cols)
{
    assert(colgen);

    for ( int i = 0; i < amt_cols; ++i ){
        fprintf(stderr, "n#: %d\nmethod: %d\namt_row: %d\ndelim: %c\ndecimals: %d\nlinker: %p\n", i+1, colgen[i]->method, colgen[i]->amt_row, colgen[i]->delim, colgen[i]->decimals, (void*)colgen[i]->_linker);
        
        if ( colgen[i]->gentype & Template){
            if ( colgen[i]->gentype & File ){
                fprintf(stderr,"file: %s\ngentype: file_template\n",
                                colgen[i]->file
                                                                    );
            } else {
                fprintf(stderr,"range: %f-%f\ngentype: template\n",
                                colgen[i]->lwall,
                                colgen[i]->rwall
                                                                    );
            }
            print_templ(colgen[i]->_template);
        } 
        else if ( colgen[i]->gentype & List ){
            if ( colgen[i]->gentype & File ){
                fprintf(stderr,"file: %s\ngentype: file_list\n",
                                colgen[i]->file
                                                                    );
            } else {
                fprintf(stderr,"range: %f-%f\ngentype: list\n",
                                colgen[i]->lwall,
                                colgen[i]->rwall
                                                                    );
            }
            print_list( colgen[i]->amt_row, colgen[i]->_list, colgen[i]->gentype );
        }
    }
}

void destroy_colgen(t_colgen *colgen)
{
    if ( colgen->gentype & Template )
        destroy_templ(colgen->_template);
    else if ( colgen->gentype & List )
        destroy_arrlist(colgen->gentype, colgen->_list, colgen->amt_row);
    if ( colgen->gentype & File )
        free(colgen->file); 
    
    free(colgen);
}

void destroy_arrcolgen(t_colgen **arrcolgen, int amt_cols)
{
    assert(arrcolgen);

    for ( int i = 0; i < amt_cols; ++i )
        destroy_colgen(arrcolgen[i]);
    free (arrcolgen);
}
