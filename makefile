CFLAGS = -Wall -Werror -pedantic
LDLIBS = -lm
SRCDIR = src
OBJDIR = obj

OBJS =  $(OBJDIR)/main.o
OBJS += $(OBJDIR)/generator.o
OBJS += $(OBJDIR)/columns.o

$(info objs: $(OBJS))

all: run

run: build
	gcc -o $@ $(OBJS) $(LDLIBS) $(CFLAGS)

build: $(OBJS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	-mkdir -p $(OBJDIR)
	gcc -c $< -o $@


.PHONY : clean
clean :
	-rm -r run data.csv $(OBJDIR)
