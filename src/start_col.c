#include "../config.h"
#include "../dbgen.h"

enum gentype {
    Template=1, 
    List=2,
    File=4,

    TotalGens=3,
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

    new_list->svalue = NULL;
    new_list->root = NULL;
    new_list->tree_size = 0;

    return new_list;
}

void start_arrlist(t_colgen *colgen)
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
        } else { // normal list
            nums_to_arrlist(new_arrlist, colgen);
        }
    }

    colgen->_list = new_arrlist;
}

void destroy_arrlist(short gentype, t_list** list, int amt_row)
{
    assert(list);

        for ( int i = 0; i < amt_row; ++i ){
            destroy_list(list[i], gentype);
        } free(list);
}

void destroy_list(t_list* list, short gentype)
{
    assert(list);

    if (( gentype == ( File | List ) ) && ( list->svalue ))
        free(list->svalue);
    if ( list->root )
        free(list->root);
    free(list);
}


t_templ *init_templ()
{
    t_templ *new_templ = malloc(sizeof(t_templ));
    assert(new_templ);

    new_templ->dvalue = 0;

    return new_templ;
}

void start_templ(t_colgen* colgen, t_templ* new_templ)
{
    assert(colgen);
    assert(new_templ);
    assert(!colgen->_template);

    new_templ->dvalue = colgen->rwall - colgen->lwall;
    
    if (colgen->gentype & Template){
        if (colgen->gentype & File){
            //filesetter_templ(colgen);
        } else {
            numsetter_templ(colgen);
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
        if ( strchr(info->option, letter[i]) ){
            new_method |= ( methods << i );
        }
    }
    
    //if new_method is random and also unique or scalable
    if (( new_method & Rnd ) && ( new_method & ( Unq | Scl ) )){ 
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

    new_colgen->fn = NULL;
    new_colgen->method = Undef;
    new_colgen->gentype = Undef;
    new_colgen->_list = NULL; //will NULL _template aswell
    new_colgen->amt_row = database->size; 
    new_colgen->lwall = 0.0;
    new_colgen->rwall = database->size;
    new_colgen->delim = database->delim;
    new_colgen->decimals = 0;
    new_colgen->_linker = NULL;

    return new_colgen;
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

    if (( info->amount ) && ( atoi(info->amount) < database->size ))
        colgen->amt_row = atoi(info->amount);
    else if (( colgen->rwall - colgen->lwall ) < database->size )
        colgen->amt_row = colgen->rwall - colgen->lwall;


    def_typeof(info, colgen);
    if ( colgen->gentype & List )
        start_arrlist(colgen); 
    else if ( colgen->gentype & Template )
        start_templ(colgen, init_templ());
    
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

    return new_arrcolgen;
}

void print_list(size_t amt_rows, t_list **list, short gentype)
{
    assert(list);
    
    for ( int i = 0; i < amt_rows; ++i ){
        if ( gentype == List ){
            fprintf(stderr, "\t# %f\n", list[i]->dvalue);
        }
        else if (( gentype == ( File | List ) ) && ( list[i]->svalue )){
            fprintf(stderr, "\t# %s\n", list[i]->svalue);
        }

        if ( list[i]->tree_size ){
            fprintf(stderr, "\t#%d tree_size: %ld", i+1,
                                                    list[i]->tree_size);
        }
    } fputc('\n', stderr);
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
        fprintf(stderr, "n#: %d\nmethod: %d\namt_row: %ld\ndelim: %c\ndecimals: %d\nlinker: %p\n", i+1, colgen[i]->method, colgen[i]->amt_row, colgen[i]->delim, colgen[i]->decimals, (void*)colgen[i]->_linker);
        
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
