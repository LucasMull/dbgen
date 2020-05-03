CFLAGS = -Wall -Werror -pedantic
LDLIBS = -lm
SRCDIR = src
OBJDIR = obj

OBJS =  $(OBJDIR)/main.o
OBJS += $(OBJDIR)/data_fetch.o
OBJS += $(OBJDIR)/manipulate_data.o

MAIN = $(OBJDIR)/main.o

.PHONY : clean all debug

all: run

run: build
	gcc -o $@ $(OBJS) $(LDLIBS)

build: mkdir $(MAIN) $(OBJS)

mkdir:
	-mkdir -p $(OBJDIR)

$(MAIN): main.c
	gcc -c $< -o $@ $(CFLAGS)
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	gcc -c $< -o $@ $(CFLAGS)

debug : main.c $(SRCDIR)/*.c
	gcc -g main.c $(SRCDIR)/*.c -o debug.out $(CFLAGS)
clean :
	-rm -rf run data.csv $(OBJDIR) debug.out
