CFLAGS = -Wall -Werror -pedantic
LDLIBS = -lm
SRCDIR = src
OBJDIR = obj

OBJS =  $(OBJDIR)/main.o
OBJS += $(OBJDIR)/data_fetch.o
OBJS += $(OBJDIR)/manipulate_data.o

MAIN = $(OBJDIR)/main.o

#remove comment to activate debug
#$(info objs: $(OBJS))

.PHONY : clean all debug

all: run

run: build
	gcc -o $@ $(OBJS) $(LDLIBS) $(CFLAGS)

build: mkdir $(MAIN) $(OBJS)

mkdir:
	-mkdir -p $(OBJDIR)

$(MAIN): main.c
	gcc -c $< -o $@
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	gcc -c $< -o $@

debug : build
	gcc -g $(OBJS) -o debug.out
clean :
	-rm -rf run data.csv $(OBJDIR) agencies debug.out
