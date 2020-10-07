## TODO

## HIGH

  - Fix formattingm variable naming, function scope, only then start working on anything else

  - Instead of repeating the same logic in a bunch of functions (see `data_fetch.c`) handling differente datatypes, implement callbacks

  - Add hashtable logic

  - So much uneccessary stuff, clean up 

  - REPLACE CURRENT ATTACH SYMBOL \~ TO SOMETHING ELSE

  - REPLACE INTEGER RANDOM NUMBER GENERATING TO FLOATING POINT RANDOM NUMBER GENERATING

  - ADD OPTION TO CREATE .csv FILES OF COMPOSITE KEYS ATTACHMENTS
    - EX: dbgen [FIELD1] \~ [FIELD2]
        - Suppose that FIELD1 generates 1234, 1111 and 2323, Then files 1234.csv, 1111.csv and 2323.csv will be created, and each .csv file will list FIELD2 contents attached to it.

  - ADD INSTALL OPTION TO MAKEFILE IN ORDER TO INSTALL dbgen TO usr/bin

  - ADD -- OPTIONS

## MID

  - parser.c can be 100% replaced with some POSIX library
