CFLAGS	= -Wall -Werror -pedantic
LDLIBS = -lm

OBJ = main.o generator.o columns.o

all : people

people : $(OBJ)
	gcc -o people $(OBJ) $(LDLIBS)
generator_people.o : main.c generator.h columns.h
	gcc -c main.c $(CFLAGS)
generator.o : generator.c generator.h
	gcc -c generator.c $(CFLAGS)
columns.o : columns.c columns.h
	gcc -c columns.c $(CFLAGS)
.PHONY : clean
clean :
	-rm people people.csv $(OBJ)
