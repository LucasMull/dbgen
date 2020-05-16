CFLAGS = -Wall -Werror -pedantic
LDLIBS = -lm
CC = gcc

SRCDIR = src
OBJDIR = obj

OBJS =  $(OBJDIR)/main.o
OBJS += $(OBJDIR)/data_fetch.o
OBJS += $(OBJDIR)/manipulate_data.o

MAINC =  multip.c
MAIN = $(OBJDIR)/main.o

.PHONY : clean all debug

all: run

run: build
	$(CC) -o $@ $(OBJS) $(LDLIBS)

build: mkdir $(MAIN) $(OBJS)

mkdir:
	-mkdir -p $(OBJDIR)

$(MAIN): $(MAINC)
	$(CC) -c $< -o $@ $(CFLAGS)
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

debug : $(MAINC) $(SRCDIR)/*.c
	$(CC) -g $(MAINC) $(SRCDIR)/*.c -o debug.out $(CFLAGS)
clean :
	-rm -rf run data.csv $(OBJDIR) debug.out
