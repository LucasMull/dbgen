CFLAGS = -Wall -Werror -pedantic
LDLIBS = -lm
SRCDIR = src
OBJDIR = obj

OBJS =  $(OBJDIR)/main.o
OBJS += $(OBJDIR)/generator.o
OBJS += $(OBJDIR)/columns.o

MAIN = $(OBJDIR)/main.o

$(info objs: $(OBJS))

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


.PHONY : clean
clean :
	-rm -r run data.csv $(OBJDIR)
