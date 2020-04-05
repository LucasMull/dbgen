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
	gcc -c $< -o $@


.PHONY : clean
clean :
	-rm run data.csv $(OBJS)

#all : generate

#generate : $(OBJS)
#	gcc -o generate $(OBJS) $(LDLIBS)
#main.o : main.c generator.h columns.h
#	gcc -c main.c $(CFLAGS)
#generator.o : generator.c generator.h
#	gcc -c generator.c $(CFLAGS)
#columns.o : columns.c columns.h
#	gcc -c columns.c $(CFLAGS)
#.PHONY : clean
#clean :
#	-rm generate data.csv $(OBJS)
