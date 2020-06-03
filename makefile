CFLAGS = -Wall -Werror -pedantic
LDLIBS = -lm
CC = gcc

SRCDIR = src
OBJDIR = obj

OBJS =  $(OBJDIR)/main.o
OBJS += $(OBJDIR)/data_fetch.o
OBJS += $(OBJDIR)/parser.o
OBJS += $(OBJDIR)/start_col.o

HEADER = config.h
HEADER += dbgen.h

MAINC =  main.c
MAIN = $(OBJDIR)/main.o

.PHONY : clean all debug

all: dbgen

dbgen: build
	$(CC) -o $@ $(OBJS) $(LDLIBS)

build: mkdir $(MAIN) $(OBJS)

mkdir:
	-mkdir -p $(OBJDIR)

$(MAIN): $(MAINC) $(HEADER)
	$(CC) -c $< -o $@ $(CFLAGS)
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

debug : $(MAINC) $(SRCDIR)/*.c
	$(CC) -g $(MAINC) $(SRCDIR)/*.c -o debug.out $(CFLAGS)
clean :
	-rm -rf dbgen data.csv $(OBJDIR) debug.out
