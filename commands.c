/*
* Author: Ren Demeis-Ortiz
* Course: CS 344 Sec. 400
* Description: Function definitions for DELETE functions
* Sources:
*/
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include "commands.h"

void initializeCommand(command* currCommand)
{
	size_t commArgSize = 513 * sizeof(currCommand->commandArgs[0]);
	memset(currCommand->commandArgs, NULL, commArgSize);
	currCommand->outputFile = NULL;
	currCommand->inputFile = NULL;
	currCommand->nextCommand = NULL;
	currCommand->inBackground = false;
}


/*
* void createCommand(command* currCommand);
* Description:
*
* Parameters:
*		
* Returns:
* Sources: https://replit.com/@cs344/studentsc#student_info1.txt
*		   Adapted from CS344 Assignment 1 & 2 
*		   strcspn() https://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input
*/
command* createCommand(char* input)
{
	printf("pid: %d\n", getpid()); //DELETE after comparing with $$ output
	int i = 0;
	char* saveptr;
	command* currCommand = malloc(sizeof(command));
	initializeCommand(currCommand);
	
	// Get Command Term
	char* currValue = strtok_r(input, " ", &saveptr);

	// If Invalid Make Command Null and Return
	if (currValue[0] == '\n' || currValue[0] == '#')
	{
		currCommand->command = NULL;
		return currCommand;
	}

	// Remove New Line Character
	currValue[strcspn(currValue, "\n")] = '\0';

	// Add Valid Command Term
	currCommand->command = calloc(strlen(currValue) + 1, sizeof(char));
	strcpy(currCommand->command, currValue);
	currValue = strtok_r(NULL, " ", &saveptr);
	
	while (currValue != NULL)
	{	
		// Remove New Line Character
		currValue[strcspn(currValue, "\n")] = '\0';

		// Add Input File
		if (strncmp(currValue, "<", sizeof("<")) == 0)
		{
			currValue = strtok_r(NULL, " ", &saveptr);
			currCommand->inputFile = calloc(strlen(currValue) + 1, sizeof(char));
			strcpy(currCommand->inputFile, currValue);
		}

		// Add Output File
		else if (strncmp(currValue, ">", sizeof(">")) == 0)
		{
			currValue = strtok_r(NULL, " ", &saveptr);
			currCommand->outputFile = calloc(strlen(currValue) + 1, sizeof(char));
			strcpy(currCommand->outputFile, currValue);
		}
		
		// Add Command Arguments
		else if (strncmp(currValue, "", sizeof("")) != 0)
		{
			// Add Process ID for $$
			if (strncmp(currValue, "$$", sizeof("$$")) == 0)
			{
				pid_t thisPid = getpid();
				currCommand->commandArgs[i] = calloc(15, sizeof(char));
				size_t bufferSize = sizeof(currCommand->commandArgs[i]);
				snprintf(currCommand->commandArgs[i], bufferSize, "%d", thisPid);
			}
			else
			{
				currCommand->commandArgs[i] = calloc(strlen(currValue) + 1, sizeof(char));
				strcpy(currCommand->commandArgs[i], currValue);
			}
			i++;
		}

		currValue = strtok_r(NULL, " ", &saveptr);


	}

	// Add Background State
	if (currCommand->commandArgs[i-1][0] == '&')
	{
		free(currCommand->commandArgs[i - 1]);
		currCommand->commandArgs[i-1] = NULL;
		currCommand->inBackground = true;
	}

	return currCommand;
}

void deconstructCommands(command* currCommand)
{
	while (currCommand != NULL)
	{
		command* emptyCommand = currCommand;
		free(currCommand->command);
		free(currCommand->inputFile);
		free(currCommand->outputFile);

		for (int i = 0; i < 513; i++)
		{
			if (currCommand->commandArgs[i] == NULL)
			{
				break;
			}
			free(currCommand->commandArgs[i]);
		}
		currCommand = currCommand->nextCommand;
		free(emptyCommand);
	}
	
}

void printCommand(command* currCommand)
{
	printf("%s\n", currCommand->command);
}

void printCommandArgs(command* currCommand)
{
	for (int i = 0; i < 513; i++)
	{
		if (currCommand->commandArgs[i] == NULL)
		{
			break;
		}
		printf("%s ", currCommand->commandArgs[i]);
	}
	printf("\n");
}

void printInputFile(command* currCommand)
{
	printf("%s\n", currCommand->inputFile);
}

void printOutputFile(command* currCommand)
{
	printf("%s\n", currCommand->outputFile);
}

void printInBackground(command* currCommand)
{
	printf("%d\n", currCommand->inBackground);
}

void printNextCommand(command* currCommand)
{
	if (currCommand->nextCommand != NULL)
	{
		printf("%s\n", currCommand->nextCommand->command);
	}
	else
	{
		printf("No Next Command\n");
	}
}


void printCommands(command* currCommand)
{
	printCommand(currCommand);
	printCommandArgs(currCommand);
	printInputFile(currCommand);
	printOutputFile(currCommand);
	printInBackground(currCommand);
	printNextCommand(currCommand);
}