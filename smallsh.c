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
#include "builtInCommands.h"


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
	printf("pid: %d\n", getpid()); //DELETE after comparing with $$ output
	char* userInput = NULL;
	command* newCommand = NULL;
	command* head = NULL;
	command* tail = NULL;
	size_t bufferSize = 0;
	int numChar = 0;

	do
	{
		
		printf(": ");
		fflush(stdout);

		numChar = getline(&userInput, &bufferSize, stdin);
		if (numChar < 1)
		{
			continue;
		}

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

		// If whitespace or Comment Prompt again
		if (newCommand->commandArgs[0] == NULL)
		{
			continue;
		}

		// If Built in cd Command
		else if (strncmp(newCommand->commandArgs[0], "cd", sizeof("cd")) == 0)
		{
			changeDirectory(newCommand);
		}

		// If Built in status Command
		else if (strncmp(newCommand->commandArgs[0], "status", sizeof("status")) == 0)
		{
			getStatus();
		}

		// If Built in exit Command
		else if (strncmp(newCommand->commandArgs[0], "exit", sizeof("exit")) == 0)
		{
			prepareExit();
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