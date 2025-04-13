#include "fork.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <errno.h> 
#include <ctype.h>

#define MAX_INPUT_SIZE 10000

void print_history(Command *head);
Command *run_command(int index, Command *head);
int isInteger(const char *str);
void freeMemory(Command *head);
void run_inbuilt(Command * data);
int insertNode(Command **head, Command *data);
void printCommand(Command *data);
void printTokens(char *tokens[]);
Command *dataCopy(const Command *src);
void freeCommand(Command *cmd);


int main()
{
    char input[MAX_INPUT_SIZE];
    char *tokens[129];
    int num_command = 0;
    Command *head = NULL;
    Command *data = NULL;
    char *full_input; 


	while(1)
	{
		//we print 262$ to the screen and then we read the user input
		//=========================
        printf("262$");
        fflush(stdout);

        if (!fgets(input, sizeof(input), stdin)) {
             if (feof(stdin)) {
                break;
            } else if (ferror(stdin)) {
                exit(EXIT_FAILURE);
            }
        }
        input[strcspn(input, "\n")] = 0;
		//if an empthy line is inputed, simple continue the while loop
	if (input[0] == '\0') {
		continue;
	}

	full_input = strdup(input);	
		//=========================
		//take our input and create tokens for each arguments, we will set this to our argument parameter for our command
		//=========================
        int i = 0;
        char *token = strtok(input, " ");
        while (token != NULL && i < 128) {
            tokens[i++] = token;
            token = strtok(NULL, " ");
        }
	tokens[i] = NULL;

		//if the first token is exit, we simply quit the program - we can do more sanitizing but this is fine
		if (strcmp(tokens[0], "exit") == 0) {
            freeMemory(head);
            exit(0);
        }

		//check if we have over 127 arguments, then we simply continue without adding to history
		if (i > 128) {
			fprintf(stderr, "error: %s\n", "too many arguments");
            continue;
        }
		//=========================

		//if our command is not history we create our Struct and then we run either our cd command or call_exe for valid commands
		if(strcmp(tokens[0], "history") != 0)
		{
			data = malloc(sizeof(Command));
			if (data == NULL) {
				fprintf(stderr, "Memory allocation failed\n");
				continue;
			}
			data->command = strdup(tokens[0]);
			free(full_input);
			data->arguments = malloc(sizeof(char*) * (i+1));
			data->arguments_length = i;
			for (int j = 0; j < i; j++) {
				data->arguments[j] = strdup(tokens[j]);
			}
			data->arguments[i] = NULL;
			data->index = num_command++;
			data->next = NULL;
			insertNode(&head, data);
			run_inbuilt(data);
		}
		//we don't create a struct since we won't be appending to linked list, just call deal with our history function
		else
		{
			if(i-1 == 0){
				print_history(head);
			}
			else if(i > 0 && strcmp(tokens[i-1], "-c") == 0){
				freeMemory(head);
				num_command = 0;
				head = NULL;
			}
			else if(isInteger(tokens[1]) && i-1 == 1){
				int arg = atoi(tokens[1]);
				data = run_command(arg, head);
				if(data == NULL){
					fprintf(stderr, "error: %s\n", "Index in history list does not exist");
					continue;
				}
				run_inbuilt(data);
				data->index = num_command++;
				insertNode(&head, data);
			}
			else{
				printf("Invalid arg for history: history|| history[-c] || history[index]\n");
			}
		}


		//free data every iteration and set it to null to prepare for enxt input
		free(data);
		data = NULL;

	
	}
}

//runs cd and calls call_exe
void run_inbuilt(Command *data) {
    if (strcmp(data->arguments[0], "cd") == 0 && data->arguments_length == 2) {
        if (chdir(data->arguments[1]) != 0) {
            fprintf(stderr, "cd error: %s\n", strerror(errno));
        }
    } else {
       int status =  call_exe(data);
       if(WEXITSTATUS(status)){
	    int exit_status = WEXITSTATUS(status);
	    fprintf(stderr, "error: %s\n", strerror(exit_status)); 
	}
	else{
	    return;
	}
    }
}

void freeMemory(Command *head) {
    Command *temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp->command);
        for (int i = 0; i < temp->arguments_length; i++) {
            free(temp->arguments[i]);
        }
        free(temp->arguments);
        free(temp);
    }
}

void printTokens(char *tokens[]) {
    if (tokens == NULL) {
        printf("No tokens to display.\n");
        return;
    }
    printf("Tokens:\n");
    for (int i = 0; tokens[i] != NULL; i++) {
        printf("  Token %d: %s\n", i, tokens[i]);
    }
}

Command * run_command(int index, Command *head)
{
	Command *current = head;
	int i = 0;
	while (current != NULL)
    {	
		if(i == index){
			return dataCopy(current);
		}
        current = current->next;
		i++;
    }
	return NULL;
}

Command * dataCopy(const Command *src) {
    if (src == NULL) return NULL;

    Command *copy = malloc(sizeof(Command));
    if (copy == NULL) {
        fprintf(stderr, "Failed to allocate memory for command copy.\n");
        return NULL;
    }

    copy->command = strdup(src->command);
    if (copy->command == NULL) {
        free(copy);
        fprintf(stderr, "Failed to duplicate command string.\n");
        return NULL;
    }

    copy->arguments = malloc(sizeof(char*) * src->arguments_length);
    if (copy->arguments == NULL) {
        free(copy->command);
        free(copy);
        fprintf(stderr, "Failed to allocate memory for arguments copy.\n");
        return NULL;
    }

    for (int i = 0; i < src->arguments_length; i++) {
        copy->arguments[i] = strdup(src->arguments[i]);
        if (copy->arguments[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free(copy->arguments[j]);
            }
            free(copy->arguments);
            free(copy->command);
            free(copy);
            fprintf(stderr, "Failed to duplicate an argument string.\n");
            return NULL;
        }
    }
    copy->arguments[copy->arguments_length] = NULL;

    copy->arguments_length = src->arguments_length;
    copy->index = src->index;
    copy->next = NULL;

    return copy;
}

void print_history(Command *head) {
    Command *current = head;

    while (current != NULL) {
        printf("%d: ", current->index);
        if (current->arguments_length > 0) { 
            for (int i = 0; i < current->arguments_length; i++) {
                printf("%s", current->arguments[i]); 
                if (i < current->arguments_length - 1) {
                    printf(" "); 
                }
            }
        }
        printf("\n"); 
        current = current->next;
    }
}

int insertNode(Command **head, Command *data) {
    if (head == NULL || data == NULL) {
        return -1;
    }

    Command *newNode = malloc(sizeof(Command));
    if (newNode == NULL) {
        return -1;
    }

    newNode->command = strdup(data->command);
    newNode->arguments = malloc(sizeof(char*) * data->arguments_length);
    if (newNode->arguments == NULL) {
        free(newNode->command);
        free(newNode);
        return -1;
    }

    for (int i = 0; i < data->arguments_length; i++) {
        newNode->arguments[i] = strdup(data->arguments[i]);
    }
    newNode->arguments_length = data->arguments_length;
    newNode->index = data->index;
    newNode->next = NULL;

    static int commandCount = 0; 

    if (commandCount == 100) {

		newNode -> index = newNode -> index -1;
        Command *toDelete = *head;
        *head = (*head)->next;
        freeCommand(toDelete);
        commandCount--; 

    }

    if (*head == NULL) {
        *head = newNode;
    } else {
        Command *current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
    commandCount++;

	Command *current = *head;
	int newIndex = 0;
	while (current != NULL) {
		current->index = newIndex;
		newIndex++;
		current = current->next;
	}
    return 0;
}

void freeCommand(Command *cmd) {
    if (cmd == NULL) return;
    free(cmd->command);
    for (int i = 0; i < cmd->arguments_length; i++) {
        free(cmd->arguments[i]);
    }
    free(cmd->arguments);
    free(cmd);
}


void printCommand(Command *data) {
    if (data == NULL) {
        printf("Command data is NULL\n");
        return;
    }

    printf("Command Index: %d\n", data->index);
    printf("Command Text: %s\n", data->command);
    printf("Number of Arguments (%d):\n", data->arguments_length);

    for (int i = 0; i < data->arguments_length; i++) {
        printf("  Arg %d: %s\n", i, data->arguments[i]);
    }
}


int isInteger(const char *str) {
    if (str == NULL || *str == '\0') {
        return 0;
    }
    if (*str == '+' || *str == '-') {
        str++;
    }
    if (*str == '\0') {
        return 0; 
    }
    while (*str) {
        if (!isdigit((unsigned char)*str)) {
            return 0;
        }
        str++;
    }
    return 1; 
}

