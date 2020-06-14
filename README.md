# dbgen
### database generator cli utility tool

## SYNOPSIS
      dbgen [OPTION]... [DELIM]... [FIELD]... [COMPOSITE_KEY]...

## DESCRIPTION
      (WIP)

## INSTALLING
      (WIP) type sudo make install to build dbgen in your usr/local/bin
      type make to build dbgen in it's folder.

## OPTIONS
      -r
            random values from specified field file or range of numbers.
      -u      
            unique values from specified field file or range of numbers.
      -s      
            scale values proportionally to field's range of numbers (if specified).
      -S
            size of database
            Ex: -S 1000 ; database of 1000 elements
      -o
            output stream
            ex: -o data.csv ; outputs database to data.csv

## DELIM
      /s
            delim of designated column will be blank space
      /t
            delim of designated column will be tab
      \ " ' ;
            delim of designated column will be any of the above chars

## FIELD
     Every field must be enclosed by square brackets, and can either assume 
     a range of numbers to be manipulated, or a file to have it's members 
     manipulated. 

     To indicate a limit on how much numbers or file strings are to be, a value
     should be indicated after a comma.

     Examples:

        dbgen [100-500]
            will generate any values from 1 to 500 linearly
        dbgen -r [100-500,25]
            will generate 25 possible values from 1 to 500 randomly
        dbgen -r [file.txt]
            will fetch any line from file.txt from 1 to n randomly
        dbgen -rus [1000-3000]
            will generate unique values from 1000 to 3000, that scale
            proportionally to the database size, and are randomly arranged

## COMPOSITE_KEY
     A field may have it's entities attached to another field's entities,
     meaning that no entity occurrence (table row) may be duplicated.

     A field may receive many attachments from different fields, but each is treated as a unique composite key, for example:

     dbgen [FIELD1] \~ [FIELD2] \~ [FIELD3]

     The resulting keys will be FIELD1_FIELD2 and FIELD1_FIELD3, which is different from a chain FIELD1_FIELD2_FIELD3 (field2 attach to field1 and field3 attach to field2).

     Each entity inserted by FIELD2 and FIELD3 will be unique to the
     FIELD1's entity, if there are no possible unique entities for
     insertion, NULL will be output instead.
     
     The attachment always occurs to the leftmost not-attached field, and a composite key may never attach to another composite key.


     \~
        will attach the field at the right side of the symbol, to the leftmost not-attached field

     Examples:
        
        dbgen [data.txt] \~ [100-500] \~ [users.txt]
                creates a composite key of data.txt and numbers from a
                range of 100-500, and creates a composite key of data.txt
                and user.txt
