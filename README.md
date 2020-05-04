# FAKE DATABASE GENERATOR

## DESCRIPTION

generate_table provides tools for generating fake databases with simple yet efficient libraries; data_fetch and manipulate_data. 

### data_fetch

data_fetch provides functions pertaining to the dynamic extraction of data from files, and to generating numerical data at runtime, that can be stored either dynamically or statically, but will need predefined length's array in the latter.

For dynamic extraction, each array is initialized with the **initBLOCK()** function, that is then assigned to a new (t_BLOCK)BLOCK object, and stored at (char\*\*)BLOCK.data; the total amount of chunks of data stored is stored in (size_t)BLOCK.size .

Each BLOCK initialized will be assigned to the HEAP object, and may be completely (or partially) free'd from memory with the **freeBLOCK()** function. 

The (t_HEAP)HEAP object responsibility is of keeping track of the BLOCKS, by pointing to each of them with the (t_BLOCK\*)HEAP.addr attribute. The maximum amount of BLOCKS that can be created is set to a default 5 at TOTAL_BLOCKS keyword. The (size_t)HEAP.size attribute holds a value that corresponds to the amount of BLOCKs of which that particular HEAP is pointing to.

The **fileToBLOCK()** function will extract file's data (each chunk of data corresponds to a line of text); and have it assigned to the BLOCK object given to it.
### TO DO
**initBLOCK()**
**freeBLOCK()**
**fileToBLOCK()**
**numsToHEAP()**
**numsToSTACK()**

### manipulate_data
### TO DO
**pickRandom()**
**pickIndex()**
**stringSwap()**
**shuffleArray()**
**initTree()**
**initNode()**
**insertNode()**
**parseStrIntoNode()**
**xData()**
**findxData()**
**printTree()**
**eraseTree()**

## IMPORTANT NOTES

all the pre-processing directives are located in src/data_fetch.h, which are crucial on generating static array data with dynamic defined sizes, also serves as documentation for each data to be generated.

main.c is just an example code to go by, every important library functions are located at .h/.c files in src folder
         
main.c will generate a bank's client database of 2million individuals. Each row represents a client, with each column pertaining to a particular data about its client. The first column indicates a unique ID, followed by the client's agency, an account with a unique value to his agency, and his/her name.
Because the third column (account) is dependant on the second (agency), it will check for any occurrence of its current value in the agency's tree of accounts,and will add it's value to the tree if there is none. 
The code's speed (in this example case) is directly affected by the amount of individuals to be generated (as in DBSIZE) and the amount of agencies available (as in AMT_3). More agencies means fewer accounts being verified for duplicates, and as the number of individuals increases, so must follow the number of agencies to increase the likelihood of coming across a unique account in fewer tries.

## COMPILE AND EXECUTION

type 'make' to create the 'run' executable, 'make clean' before recompiling, and 'make debug' to debug with gdb.

database will be stored in data.csv by default

content file contains pre-existing data to be dynamically manipulated in the heap, there is a nome.txt and sobrenome.txt in there by default, that are used in main.c to compile fake names

## TO BE FIXED/IMPLEMENTED

* use enum for repeated definitions
        
* fix giveUniqueValue() function not addressing value to return_ptr

* sorting function for binary tree

* iterative method to free each tree node (maybe)
