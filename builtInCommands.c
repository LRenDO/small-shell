/*
* Author: Ren Demeis-Ortiz
* Course: CS 344 Sec. 400
* Description: Function definitions for Built in Command functions
*			cd, exit, status as well as a print background status
*			function.
*/
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include "commands.h"
#include "builtInCommands.h"

/*
* void changeDirectory(command* currCommand);
* Description: When there are two arguments the second is the relative
*
* Parameters:
*		currCommand (command*) command struct instance to get command
*					from. 
*			commandArgs[0]
*			commandArgs[1] must be directory, relative or full path
*
* Sources: chdir() https://man7.org/linux/man-pages/man2/chdir.2.html
*/
void changeDirectory(command* currCommand)
{

	// If No Path Arguments Go To Home Directory
	// Source: https://stackoverflow.com/questions/9493234/chdir-to-home-directory
	if (currCommand->commandArgs[1] == NULL)
	{
		if (chdir(getenv("HOME")) != 0)
		{
			perror("Error changing to home directory");
			fflush(stderr);
		}
	}
	// If Path Arguments Go To Argument Path
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
* void printStatus(int status);
* Description:  Prints the status passed with message.
*
* Parameters:
*		status (int) status to be printed
*
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
* void printBgStatus(int status);
* Description: Prints the pid and status passed with message.
*
* Parameters:
*		childPid (pid_t) pid to be printed in message
*		status (int) status to be printed in message
*/
void printBgStatus(pid_t childPid, int status)
{
	if (WIFSIGNALED(status))
	{
		printf("Background PID %d is done: terminated by signal %d\n", childPid, WTERMSIG(status));
		fflush(stdout);
	}
	else if (WIFEXITED(status)) {
		printf("Background PID %d is done: exit value %d\n", childPid, WEXITSTATUS(status));
		fflush(stdout);
	}
}

/*
* void prepareExit(pid_t pidArray[100], command* head, char * userInput);
* Description: Prepares for exit by sending SIGTERM to all background 
*			child processes still running. Waits for them to end and
*			calls printBgStatus. Then frees allocated memory.
			Finally, it exits successfully.
*
* Parameters:
*		head (command*) head of list of command structs used
*		userInput (char*) pointer used to get user input
*
* Returns:  Exits program without returning
* Sources: https://softwareengineering.stackexchange.com/questions/281880/best-way-to-signal-all-child-processes-to-terminate-using-c
*/
void prepareExit(pid_t pidArray[100], command* head, char * userInput)
{
	int i = 0;
	int childStatus;
	pid_t childPid;

	// Terminate Remaining Processes
	while (pidArray[i] != 0 && i < 100) {
		if (kill(pidArray[i], SIGTERM) == -1 && errno != ESRCH) 
		{			
			exit(EXIT_FAILURE);
		}
		childPid = waitpid(pidArray[i], &childStatus, 0);

		printBgStatus(childPid, childStatus);

		i++;
	}

	// Free Pointers to Allocated Memory
	free(userInput);
	deconstructCommands(head);
	
	exit(EXIT_SUCCESS);

}
