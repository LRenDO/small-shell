/*
* Author: Ren Demeis-Ortiz
* Course: CS 344 Sec. 400
* Description: Function definitions for Built in Command functions
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
#include "builtInCommands.h"

/*
* void changeDirectory(command* currCommand);
* Description:
*
* Parameters:
*
* Returns:
* Sources: chdir() https://man7.org/linux/man-pages/man2/chdir.2.html
*/
void changeDirectory(command* currCommand)
{
	// DELETE printf("cd\n");
	// DELETE char direct[256];
	// DELETE getcwd(direct, sizeof(direct));
	// DELETE printf("%s\n", direct);

	// If No Arguments Go To Home Directory
	// Source: https://stackoverflow.com/questions/9493234/chdir-to-home-directory
	if (currCommand->commandArgs[1] == NULL)
	{
		if (chdir(getenv("HOME")) != 0)
		{
			perror("Error changing to home directory");
			fflush(stderr);
		}
	}
	// If Arguments Go To Argument Path
	else
	{
		if (chdir(currCommand->commandArgs[1]) != 0)
		{
			perror("Error changing directory");
			fflush(stderr);
		}
		
	}

	// DELETE memset(direct, NULL, sizeof(direct));
	// DELETE getcwd(direct, sizeof(direct));
	// DELETE printf("%s\n", direct);
}

void getStatus() 
{
	// If No Foreground Processes Exit Status 0
	// Else If Signal Return Signal
	// Else If Exit Status Return Exit Status
	printf("status\n");
}

void prepareExit()
{
	// Kill All Processes Started

	printf("exit\n");
}
