// DO NOT MODIFY!
#ifndef FORK_H
#define FORK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#define MAX_ARGS 128

typedef struct Command{
	char * command; // the command typed into the shell
	char ** arguments; // an array of NULL terminated char pointers. 
                        // NOTE: argv[0] should be the name of the command. 
                        // the remainder of argv[…] being the command line arguments typed after the command. 
                        // argv[] must have NULL following its last argument member.
	struct Command * next; // the next command in the list		
	int index; // the order in which the command was entered
	int arguments_length; // this variable indicates the length of the arguments array excluding the ending NULL pointer.
}Command;

int call_exe(Command *);
#endif
