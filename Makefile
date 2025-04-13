CC = gcc -std=gnu99
OPTS = -Wall -Werror
DEBUG = -g

SRCDIR = ./src
OBJDIR = ./obj
INCDIR = ./inc

INCLUDE=$(addprefix -I,$(INCDIR))
CFLAGS=$(OPTS) $(INCLUDE) $(DEBUG)
OBJECTS=$(addprefix $(OBJDIR)/,shell.o fork.o)

all: cs262sh

cs262sh: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^
$(OBJDIR)/shell.o: $(SRCDIR)/shell.c $(INCDIR)/fork.h
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm cs262sh
	rm $(OBJDIR)/shell.o
