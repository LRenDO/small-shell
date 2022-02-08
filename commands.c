/*
* Author: Ren Demeis-Ortiz
* Course: CS 344 Sec. 400
* Description: Function definitions for DELETE functions
*/
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include "commands.h"

/*
* void initializeCommand(command* currCommand);
* Description: Initializes the struct instance passed. Each member initialized
*		to NULL, 0, or false.  Allocates memory for commandArgs array of pointers.
*
* Parameters:
*			currCommand (command *) command to be printed
*/
void initializeCommand(command* currCommand)
{
	size_t commArgSize = 513 * sizeof(currCommand->commandArgs[0]);
	memset(currCommand->commandArgs, NULL, commArgSize);
	currCommand->outputFile = NULL;
	currCommand->inputFile = NULL;
	currCommand->nextCommand = NULL;
	currCommand->inBackground = false;
	currCommand->numArgs = 0;
}

/*
* char* replaceVariables(char* input);
* Description:  Replaces $$ variable symbol with process ID. Handles strings
*			without variables.
*
* Parameters:
*		input (char*) string check for and replace variables.
*
* Returns:
* Sources: realloc() https://www.geeksforgeeks.org/dynamic-memory-allocation-in-c-using-malloc-calloc-free-and-realloc/
*		   strstr() https://www.tutorialspoint.com/c_standard_library/c_function_strstr.htm
*		   strncat() https://www.man7.org/linux/man-pages/man3/strcat.3.html
*/
char* replaceVariables(char* input)
{
	pid_t thisPid = getpid();
	size_t pidStrSize = 15 * sizeof(char);
	size_t argSize = sizeof(input);
	size_t bufferSize = pidStrSize + argSize;
	char* pidStr;
	char* argCopy;
	char* substringPtr;
	  
	// Create String from Process ID
	// that way it can be concatenated with the rest of the string
	pidStr = calloc(15, sizeof(char));
	snprintf(pidStr, pidStrSize, "%d", thisPid);

	// Make a Copy of the Argument
	// building the new arg with the input pointer
	// making a copy allows for clearing of the current input
	// and manipulation of the copy as the new arg is built
	argCopy = calloc(strlen(input) + 1, sizeof(char));
	strcpy(argCopy, input);

	// Get Pointer for First Variable '$$' to Replace
	substringPtr = strstr(argCopy, "$$");

	// If No Variable is Found Return Without Replacing
	if (substringPtr == NULL)
	{
		return input;
	}
	// Else a Variable is Found
	// Clear Argument
	else
	{
		memset(input, '\0', strlen(input));
	}


	// Build New Argument with Expanded Variables
	while (substringPtr != NULL)
	{
		// Resize Input to Size of the Current Argument + Space for PID String
		// to make sure there is no overflow or memory access issues
		bufferSize = bufferSize + pidStrSize;
		input = realloc(input, bufferSize);

		// Save Ending After Variable and Insert Null Terminator Where $$ Appears
		// to mark the end of the string to copy into the new argument
		char* ending = &substringPtr[2];
		substringPtr[0] = '\0';

		// Create String Including Variable
		// by combining new argument created so far and process ID string
		// If String Does Not Start with Variable Add Beginning of Argument Copy
		if (argCopy[0] != '\0')
		{
			strncat(input, argCopy, strlen(argCopy));
		}
		strncat(input, pidStr, strlen(pidStr));

		// Check Remaining Characters for Variable and Mark Next Variable
		// when there are no variables left is how the loop terminates
		substringPtr = strstr(ending, "$$");
		if (substringPtr != NULL)
		{
			substringPtr[0] = '\0';
		}


		// If Partial String is Left after Variable was Replaced
		// add the substring after the $$ and before the next $$ 
		// or the end of string
		if (ending[0] != '\0')
		{
			strncat(input, ending, strlen(ending));
		}

	}

	free(argCopy);
	free(pidStr);

	return input;
}
/*
* void parseVariables(command* currCommand);
* Description: Calls replaceVariables() on all the arguments and files
*			in the command struct instance passed.
*
* Parameters:
*			currCommand (command *) command to be printed
* Sources: realloc() https://www.geeksforgeeks.org/dynamic-memory-allocation-in-c-using-malloc-calloc-free-and-realloc/
*		   strstr() https://www.tutorialspoint.com/c_standard_library/c_function_strstr.htm
*		   strcat() https://www.man7.org/linux/man-pages/man3/strcat.3.html		   
*/
void parseVariables(command* currCommand)
{
	int i = 0;

	// Replace all variables in each argument
	while (currCommand->commandArgs[i] != NULL)
	{
		currCommand->commandArgs[i] = replaceVariables(currCommand->commandArgs[i]);
		i++;
	}

	// Replace all variables in Input File Argument
	if (currCommand->inputFile != NULL)
	{
		currCommand->inputFile = replaceVariables(currCommand->inputFile);
	}

	//  Replace all variables in Input File Argument
	if (currCommand->outputFile != NULL)
	{
		currCommand->outputFile = replaceVariables(currCommand->outputFile);
	}
	
}

/*
* bool isBuiltIn(char * input);
* Description: Returns true if the string passed is a built in command
*			(cd, status, exit). 
*
* Parameters:
*		input (char*) string to be checked if it is a built in command
*
* Returns:
*		builtIn (bool) true if it is (cd, status, exit). False otherwise.
*/
bool isBuiltIn(char * input)
{
	bool builtIn = false;

	if (strncmp(input, "cd", strlen(input)) == 0)
	{
		builtIn = true;
	}

	if (strncmp(input, "status", strlen(input)) == 0)
	{
		builtIn = true;
	}

	if (strncmp(input, "exit", strlen(input)) == 0)
	{
		builtIn = true;
	}

	return builtIn;
	
}

/*
* void setBackgroundFlag(command* currCommand, int i);
* Description: Sets the background flag of command struct instance passed.
*			If arguments end with &, it is removed. If the argument is not
*			a built in (cd, status, exit) command, it sets the background
*			flag to true.
*
* Parameters:
*			currCommand (command *) command to set background flag for
*			i (int) number of commands in commandArgs[]
*/
void setBackgroundFlag(command* currCommand, int i)
{
	bool hasBgFlag = strcmp(currCommand->commandArgs[i - 1], "&") == 0;
	bool builtIn = isBuiltIn(currCommand->commandArgs[0]);
	
	// Add Background Flag and Remove Background Symbol '&'
	if (hasBgFlag)
	{
		free(currCommand->commandArgs[i - 1]);
		currCommand->commandArgs[i - 1] = NULL;
		currCommand->inBackground = true;
	}
	
	// Remove Background Symbol '&'
	// built in commands cd, status, exit do not run in background
	else if (hasBgFlag && builtIn)
	{
		free(currCommand->commandArgs[i - 1]);
		currCommand->commandArgs[i - 1] = NULL;
	}
}

/*
* void createCommand(char * input);
* Description: Creates a command struct from string and returns a pointer to it.
*			Additonally, variables $$ are expanded to PID by the end.
*
* Parameters:
*		input (char*) string to be parsed into a command struct instance.
*				String may include '/n' newline character.
* Returns:
*		currCommand (command*) pointer to the newly created struct instance
*		currCommand->commandArgs[0] (char*) will be set to NULL 
*				if command is a space, newline or a comment
* Sources: https://replit.com/@cs344/studentsc#student_info1.txt
*		   Adapted from CS344 Assignment 1 & 2 
*		   strcspn() https://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input
*/
command* createCommand(char* input)
{
	int i = 0;
	char* saveptr;
	command* currCommand = malloc(sizeof(command));
	initializeCommand(currCommand);
	
	// Get Command Term
	char* currValue = strtok_r(input, " ", &saveptr);

	// If Space, Newline or Comment Make Command Null and Return
	if (currValue[0] == '\n' || currValue[0] == '#')
	{
		return currCommand;
	}
	
	while (currValue != NULL)
	{	

		// Remove New Line Character If Exists
		// can't be done after loop because it will miss
		// last argument if its an input or output file 
		currValue[strcspn(currValue, "\n")] = '\0';

		// Add Input File
		if (strncmp(currValue, "<", sizeof("<")) == 0)
		{
			currValue = strtok_r(NULL, " ", &saveptr);
			currValue[strcspn(currValue, "\n")] = '\0';
			currCommand->inputFile = calloc(strlen(currValue) + 1, sizeof(char));
			strcpy(currCommand->inputFile, currValue);
		}

		// Add Output File
		else if (strncmp(currValue, ">", sizeof(">")) == 0)
		{
			currValue = strtok_r(NULL, " ", &saveptr);
			currValue[strcspn(currValue, "\n")] = '\0';
			currCommand->outputFile = calloc(strlen(currValue) + 1, sizeof(char));
			strcpy(currCommand->outputFile, currValue);
		}
		
		// Add Command Arguments
		else if (strncmp(currValue, "", sizeof("")) != 0)
		{
			currCommand->commandArgs[i] = calloc(strlen(currValue) + 1, sizeof(char));
			strcpy(currCommand->commandArgs[i], currValue);
			i++;
		}

		currValue = strtok_r(NULL, " ", &saveptr);


	}

	setBackgroundFlag(currCommand, i);

	// Add Number of Arguments
	currCommand->numArgs = i;

	// Parse and Replace $$ Variable with PID
	parseVariables(currCommand);


	return currCommand;
}

/*
* void deconstructCommands(command* currCommand);
* Description: Frees all memory for list of command struct instances.
*
* Parameters:
*			currCommand (command *) head of list of command structs
*/
void deconstructCommands(command* currCommand)
{
	while (currCommand != NULL)
	{
		command* emptyCommand = currCommand;
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


/*
* void printCommandArgs(command* currCommand);
* Description: Prints command arguments of command struct instance passed.
*
* Parameters:
*			currCommand (command *) command to be printed
*/
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

/*
* void printInputFile(command* currCommand);
* Description: Prints input file argument of command struct instance passed.
*
* Parameters:
*			currCommand (command *) command to be printed
*/
void printInputFile(command* currCommand)
{
	printf("%s\n", currCommand->inputFile);
}

/*
* void printOutputFile(command* currCommand);
* Description: Prints output file argument of command struct instance passed.
*
* Parameters:
*			currCommand (command *) command to be printed
*/
void printOutputFile(command* currCommand)
{
	printf("%s\n", currCommand->outputFile);
}


/*
* void printInBackground(command* currCommand);
* Description:  Prints background flag of command struct passed.
*
* Parameters:
*			currCommand (command *) command to be printed
*/
void printInBackground(command* currCommand)
{
	printf("Start in Background: %d\n", currCommand->inBackground);
}

/*
* void printNumArgs(command* currCommand);
* Description: Prints number of arguments of command struct passed.  Is used 
*			only in createCommand for ease of debugging.  If all reference to 
*			numArgs is removed it doesn't affect function.
*
* Parameters:
*			currCommand (command *) command to be printed
*/
void printNumArgs(command* currCommand)
{
	printf("numArgs: %d\n", currCommand->numArgs);
}

/*
* void printNextCommand(command* currCommand);
* Description: Prints next command first command argument if it exists.
*			otherwise prints message that it doesn't exist.
*
* Parameters:
*			currCommand (command *) command to be printed
*/
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


/*
void printCommands(command* currCommand);
* Description: Prints command arguments, input file and output file if they exist,
*			the background flag, number of arguments and the next command.
*
* Parameters: 
*			currCommand (command *) command to be printed
*/
void printCommands(command* currCommand)
{
	printf("\n\n");
	printf("------------\n");
	printCommandArgs(currCommand);
	if (currCommand->inputFile != NULL)
	{
		printInputFile(currCommand);
	}
	if (currCommand->outputFile != NULL)
	{
		printOutputFile(currCommand);
	}
	printInBackground(currCommand);
	printNumArgs(currCommand);
	printNextCommand(currCommand);
	printf("------------\n\n");
}