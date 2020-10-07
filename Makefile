CFLAGS = -Wall -Werror -pedantic
LDLIBS =
CC = gcc

SRCDIR = src
OBJDIR = obj
INCLUDEDIR = include
LIBDIR = lib
LIB = $(LIBDIR)/libdbgen.a
EXEC = dbgen

OBJS =  $(OBJDIR)/main.o \
	$(OBJDIR)/data_fetch.o \
	$(OBJDIR)/parser.o \
	$(OBJDIR)/start_col.o

MAIN =  main.c
MAIN_O = $(OBJDIR)/main.o

.PHONY : clean all debug

all: $(EXEC)

$(EXEC): build
	$(CC) -o $@ $(OBJS) $(LDLIBS)

build: mkdir $(MAIN_O) $(OBJS) $(LIB)

mkdir:
	-mkdir -p $(OBJDIR) $(LIBDIR)

$(MAIN_O): $(MAIN)
	$(CC) -I$(INCLUDEDIR) -c $< -o $@ $(CFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -I$(INCLUDEDIR) -c $< -o $@ $(CFLAGS)

$(LIB):
	-ar rcs $@ $(OBJS)

debug : $(MAIN) $(SRCDIR)/*.c
	$(CC) -g -I$(INCLUDEDIR) $(MAIN) $(SRCDIR)/*.c -o debug.out $(CFLAGS)
clean :
	-rm -rf $(OBJDIR) *.csv

purge : clean
	-rm -rf $(EXEC) $(LIBDIR) debug.out
