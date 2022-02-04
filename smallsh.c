/*
* Author: Ren Demeis-Ortiz
* Course: CS 344 Sec. 400
* Description: Function definitions for DELETE functions
* Sources:
*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include "smallsh.h"
#include "commands.h"

bool checkInput(char* input)
{
	if (input[0] == '\n')
	{
		return false;
	}
	else if (input[0] == '#')
	{
		return false;
	}

	return true;
}

/* int runShell()
* Description: Runs shell original parent process
*
* Parameters: 
*		
* Sources: 
*		https://replit.com/@cs344/4forkexamplec#main.c
*
*/


int runShell()
{
	char* userInput;
	command* newCommand = NULL;
	command* head = NULL;
	command* tail = NULL;
	// DELETE userInput = malloc(2049 * sizeof(char));
	size_t bufferSize = 0;
	int numChar = 0;
	int isValid = true;

	do
	{
		
		printf(": ");
		fflush(stdout);

		// DELETE memset(userInput, '\0', bufferSize);
		numChar = getline(&userInput, &bufferSize, stdin);

		// Process Command
		newCommand = createCommand(userInput);
		if (head == NULL)
		{
			head = newCommand;
			tail = newCommand;
		}
		else
		{
			tail->nextCommand = newCommand;
			tail = newCommand;
		}
		// DELETE printCommands(newCommand);

		// If whitespace or Comment Prompt again
		if (newCommand->command == NULL)
		{
			continue;
		}

		// DELETE newCommand->command[strcspn(newCommand->command, "\n")] = '\0';


		// If Built in cd Command
		else if (strncmp(newCommand->command, "cd", sizeof("cd")) == 0)
		{
			printf("cd\n");
		}

		// If Built in status Command
		else if (strncmp(newCommand->command, "status", sizeof("status")) == 0)
		{
			printf("status\n");
		}

		// If Built in exit Command
		else if (strncmp(newCommand->command, "exit", sizeof("exit")) == 0)
		{
			printf("exit\n");
			break;
		}

		// Else Pass Command and Args to exec
		else
		{
			printf("fork and execute with exec\n");
		}

	} while (true);
	
	free(userInput);
	deconstructCommands(head);

	return 0;
}