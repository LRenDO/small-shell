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
#include <signal.h>
#include <errno.h>
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

}

/*
* void changeDirectory(command* currCommand);
* Description:
*
* Parameters:
*
* Returns:
* Sources: chdir() https://man7.org/linux/man-pages/man2/chdir.2.html
*/
void printStatus(int status) 
{
	// If No Foreground Processes Exit Status 0
	if (status == 0 || status == 1)
	{
		printf("exit value %d\n", status);
		fflush(stdout);
	}
	else
	{
		printf("terminated by signal %d\n", status);
		fflush(stdout);
	}
}

/*
* void prepareExit(pid_t pidArray[]);
* Description:
*
* Parameters:
*
* Returns:
* Sources: https://softwareengineering.stackexchange.com/questions/281880/best-way-to-signal-all-child-processes-to-terminate-using-c
*/
void prepareExit(pid_t pidArray[100])
{
	int i = 0;
	int childStatus;
	pid_t childPid;

	// Terminate Remaining Processes
	while (pidArray[i] != 0 && i < 100) {
		printf("Kill Child Process %d\n", pidArray[i]);  // Delete
		if (kill(pidArray[i], SIGTERM) == -1 && errno != ESRCH) 
		{			
			exit(EXIT_FAILURE);
		}
		childPid = waitpid(pidArray[i], &childStatus, 0) > 0;
		printf("Child Process %d on status %d\n", pidArray[i], WTERMSIG(childStatus));  // Delete
		i++;
	}

	
		

	printf("exit preparation complete\n");  // Delete
}
