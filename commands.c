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
	// DELETE currCommand->outputFile = NULL;
	// DELETE currCommand->inputFile = NULL;
	currCommand->nextCommand = NULL;
	currCommand->inBackground = false;
	currCommand->numArgs = 0;
}

/*
* void replaceVariables(command* currCommand);
* Description:
*
* Parameters:
*
* Returns:
* Sources: realloc() https://www.geeksforgeeks.org/dynamic-memory-allocation-in-c-using-malloc-calloc-free-and-realloc/
*		   strstr() https://www.tutorialspoint.com/c_standard_library/c_function_strstr.htm
*		   strncat() https://www.man7.org/linux/man-pages/man3/strcat.3.html
*/
void replaceVariables(command* currCmd, int i)
{
	pid_t thisPid = getpid();
	size_t pidStrSize = 15 * sizeof(char);
	size_t argSize = sizeof(currCmd->commandArgs[i]);
	size_t bufferSize = pidStrSize + argSize;
	char* pidStr;
	char* argCopy;
	char* substringPtr;

	// Create Process ID String
	pidStr = calloc(15, sizeof(char));
	snprintf(pidStr, pidStrSize, "%d", thisPid);

	// Make a Copy of Arg
	argCopy = calloc(strlen(currCmd->commandArgs[i]) + 1, sizeof(char));
	strcpy(argCopy, currCmd->commandArgs[i]);

	// Find First Variable
	substringPtr = strstr(argCopy, "$$");
	if (substringPtr != NULL)
	{
		memset(currCmd->commandArgs[i], '\0', strlen(currCmd->commandArgs[i])); 
	}


	// Find Remaining Variables 
	while (substringPtr != NULL)
	{
		// Resize currCmd->commandArgs[i]
		bufferSize = bufferSize + pidStrSize;
		currCmd->commandArgs[i] = realloc(currCmd->commandArgs[i], bufferSize);

		// Save Ending and Replace Position of First $ With Null Terminator
		char* ending = &substringPtr[2];
		substringPtr[0] = '\0';

		// Create String From Prefix and Process ID
		if (argCopy[0] != '\0')
		{
			strncat(currCmd->commandArgs[i], argCopy, strlen(argCopy));
		}
		strncat(currCmd->commandArgs[i], pidStr, strlen(pidStr));

		// Check Remaining Characters for Variable and Mark Next Variable
		substringPtr = strstr(ending, "$$");
		if (substringPtr != NULL)
		{
			substringPtr[0] = '\0';
		}

		if (ending[0] != '\0')
		{
			strncat(currCmd->commandArgs[i], ending, strlen(ending));
		}
		
	}

	free(argCopy);
	free(pidStr);

}


/*
* void parseVariables(command* currCommand);
* Description:
*
* Parameters:
*
* Returns:
* Sources: realloc() https://www.geeksforgeeks.org/dynamic-memory-allocation-in-c-using-malloc-calloc-free-and-realloc/
*		   strstr() https://www.tutorialspoint.com/c_standard_library/c_function_strstr.htm
*		   strcat() https://www.man7.org/linux/man-pages/man3/strcat.3.html		   
*/
void parseVariables(command* currCommand)
{
	int i = 0;

	// Check each argument
	while (currCommand->commandArgs[i] != NULL)
	{
		replaceVariables(currCommand, i);
		// DELETE replaceVariables(currCommand->commandArgs[i]);
		i++;
	}

	/* DELETE block// Update Input File
	if (currCommand->inputFile != NULL)
	{
		replaceVariables(currCommand->inputFile);
	}

	// Update Output File
	if (currCommand->outputFile != NULL)
	{
		replaceVariables(currCommand->outputFile);
	}*/
	
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
		return currCommand;
	}

	// Remove New Line Character
	currValue[strcspn(currValue, "\n")] = '\0';

	// Add Valid Command Term as First Arg
	// DELETE currCommand->command = calloc(strlen(currValue) + 1, sizeof(char));
	// DELETE strcpy(currCommand->command, currValue);
	currCommand->commandArgs[i] = calloc(strlen(currValue) + 1, sizeof(char));
	strcpy(currCommand->commandArgs[i], currValue);
	i++;
	currValue = strtok_r(NULL, " ", &saveptr);
	
	while (currValue != NULL)
	{	
		// Remove New Line Character
		currValue[strcspn(currValue, "\n")] = '\0';

		/*// DELETE??? Add Input File
		if (strncmp(currValue, "<", sizeof("<")) == 0)
		{
			currCommand->commandArgs[i] = calloc(strlen(currValue) + 1, sizeof(char));
			strcpy(currCommand->commandArgs[i], currValue);
			i++;

			currValue = strtok_r(NULL, " ", &saveptr);
			currValue[strcspn(currValue, "\n")] = '\0';
			currCommand->inputFile = calloc(strlen(currValue) + 1, sizeof(char));
			strcpy(currCommand->inputFile, currValue);
		}

		// DELETE??? Add Output File
		else if (strncmp(currValue, ">", sizeof(">")) == 0)
		{
			currCommand->commandArgs[i] = calloc(strlen(currValue) + 1, sizeof(char));
			strcpy(currCommand->commandArgs[i], currValue);
			i++;

			currValue = strtok_r(NULL, " ", &saveptr);
			currValue[strcspn(currValue, "\n")] = '\0';
			currCommand->outputFile = calloc(strlen(currValue) + 1, sizeof(char));
			strcpy(currCommand->outputFile, currValue);
		}*/
		
		// Add Command Arguments
		if (strncmp(currValue, "", sizeof("")) != 0)
		{
			currCommand->commandArgs[i] = calloc(strlen(currValue) + 1, sizeof(char));
			strcpy(currCommand->commandArgs[i], currValue);
			i++;
		}

		currValue = strtok_r(NULL, " ", &saveptr);


	}

	// Add Background State
	if (currCommand->commandArgs[i-1][0] == '&')
	{
		// DELETE free(currCommand->commandArgs[i - 1]);
		// DELETE currCommand->commandArgs[i-1] = NULL;
		currCommand->inBackground = true;
	}

	// Add Number of Arguments
	currCommand->numArgs = i;

	// Parse Process ID Variable
	parseVariables(currCommand);

	// DELETE Add Command
	// DELETE currCommand->command = calloc(strlen(currCommand->commandArgs[0]) + 1, sizeof(char));
	// DELETE strcpy(currCommand->command, currCommand->commandArgs[0]);
	printCommands(currCommand); // DELETE

	return currCommand;
}

void deconstructCommands(command* currCommand)
{
	while (currCommand != NULL)
	{
		command* emptyCommand = currCommand;
		// DELETE free(currCommand->command);
		// DELETE free(currCommand->inputFile);
		// DELETE free(currCommand->outputFile);

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

// DELETE void printCommand(command* currCommand)
// DELETE {
// DELETE 	printf("%s\n", currCommand->command);
// DELETE }

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

/* DELETE
void printInputFile(command* currCommand)
{
	printf("%s\n", currCommand->inputFile);
}

void printOutputFile(command* currCommand)
{
	printf("%s\n", currCommand->outputFile);
}
*/

void printInBackground(command* currCommand)
{
	printf("Start in Background: %d\n", currCommand->inBackground);
}

void printNumArgs(command* currCommand)
{
	printf("numArgs: %d\n", currCommand->numArgs);
}

void printNextCommand(command* currCommand)
{
	if (currCommand->nextCommand != NULL)
	{
		printf("%s\n", currCommand->nextCommand->commandArgs[0]);
	}
	else
	{
		printf("No Next Command\n");
	}
}


void printCommands(command* currCommand)
{
	// DELETE printCommand(currCommand);
	printCommandArgs(currCommand);
	// DELETE printInputFile(currCommand);
	// DELETE printOutputFile(currCommand);
	printInBackground(currCommand);
	printNumArgs(currCommand);
	printNextCommand(currCommand);
}