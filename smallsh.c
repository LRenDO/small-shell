/*
* Author: Ren Demeis-Ortiz
* Course: CS 344 Sec. 400
* Description: Function definitions for shell functions
*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include "smallsh.h"
#include "commands.h"
#include "builtInCommands.h"
#include "signalHandlers.h"
#include "ioRedirect.h"

/* char* getInput(char* userInput, size_t bufferSize)
* Description:  This function prompts and gets input from user using :
*			symbol to prompt.  Returns NULL if error occurs and pointer
*			to string if successful.
*
* Parameters: 
*		userInput (char*) variable where userInput will be stored
*		bufferSize (size_t*) pointer to variable for size of userInput
* Returns:
*		userInput (char*) will be NULL if error occurs will point 
*					input string when successful
*/
char* getInput(char* userInput, size_t* bufferSizePtr)
{
	// Prompt User and Get Command Input
	printf(": ");
	fflush(stdout);
	int numChar = getline(&userInput, bufferSizePtr, stdin);

	// Check for std Error
	// without clearing errors getline will not read user input
	// and the loop will not terminate when SIGTSTP is signaled
	// Source: https://edstem.org/us/courses/16718/discussion/1065019
	if (ferror(stdin))
	{
		clearerr(stdin);
		return NULL;
	}

	// Check for Empty String 
	// to prevent errors when processing the string
	// into command struct
	if (strlen(userInput) < 1 || numChar == -1)
	{
		perror("Getline Error");
		return NULL;
	}

	return userInput;
}

/* void runChild(command* newCommand, sigact* ignore_action, sigact* switchMode_SIGTSTP, sigact* default_action);
* Description:  This function runs the child processes. It uses the
*			input/output redirection functions and executes commands.
*			It exits 1 on exec and file opening errors.  It also sets
*			the signal handlers for child processes. It sets SIGSTSP to
*			ignore for all child processes. It sets SIGINT to the default 
*			which terminates the process for the foreground child process 
*			only.
*
* Parameters:
*		newCommand (command*)
*		ignore_action (sigact*) pointer to sigaction struct that handles
*				by ignoring the signal
*		switchMode_SIGTSTP (sigact*) pointer that points to parent SIGTSTP
*				handler, so it can be changed for the child process
*		default_action (sigact*) pointer to sigaction struct that handles
*				by using the default action of the signal
* Returns:
*		userInput (char*) will be NULL if error occurs will point
*					input string when successful
*/
void runChild(command* newCommand, sigact* ignore_action, sigact* switchMode_SIGTSTP, sigact* default_action)
{
	// Set Signal handlers for Children to Ignore SIGSTP
	sigaction(SIGTSTP, ignore_action, switchMode_SIGTSTP);

	// If Running in Foreground Set SIGINT to Terminate Process
	if (foregroundOnly || !newCommand->inBackground)
	{
		sigaction(SIGINT, default_action, ignore_action);
	}

	// If Running in Background
	// Redirect stdin and stdout to /dev/null for all
	// background processes so they won't print to console
	// and they won't redirect input unless input arg is given.
	// Input redirection based on arguments happens after this
	// else clause
	else
	{
		printf("Background pid is %d\n", getpid());
		fflush(stdout);

		char* devNull = "/dev/null";
		if (newCommand->inputFile == NULL)
		{
			redirectStdin(devNull);
		}

		if (newCommand->outputFile == NULL)
		{
			redirectStdout(devNull);
		}
	}

	// If Input or Output File Arg was Entered Redirect stdin and stdout to Args
	if (newCommand->inputFile != NULL)
	{
		redirectStdin(newCommand->inputFile);
	}

	if (newCommand->outputFile != NULL)
	{
		redirectStdout(newCommand->outputFile);
	}

	// Execute the Command and Return an Error if Unsuccessful
	execvp(newCommand->commandArgs[0], newCommand->commandArgs);
	perror("Execute Error");
	exit(1);
}


/* void awaitFgChild(int childPid);
* Description: Waits on foreground child process with PID passed to end
*
* Parameters:
*		childPid (pid_t) child pid
* Returns:
*		status (int) the status or termination signal of process
*		
*/
int awaitFgChild(pid_t childPid)
{
	int status = 0;
	pid_t childStartPid = childPid;
	childPid = waitpid(childPid, &status, 0);

	// Wait Until Foreground Child is Returned
	// in case of interrupts uses childStartPid to continue to wait
	// since childPid will have not returned the pid
	while (childPid < 1)
	{
		childPid = waitpid(childStartPid, &status, 0);
	}

	// Print Termination Message and Set Status or Signal for Return 
	if (WIFSIGNALED(status))
	{
		printf("Foreground PID %d terminated by signal %d\n", childPid, WTERMSIG(status));
		status = WTERMSIG(status);
	}
	else if (WIFEXITED(status)) {
		status = WEXITSTATUS(status);
	}

	// Return Status or Signal
	return(status);
}


/*  int removePid(pid_t backgroundPid[100], int i, int count);
* Description: Removes PID from background list to preserve space
*			in array and prevent unneeded termination calls at end
*			of program.
*
* Parameters:
*		backgroundPid[] (pid_t []) array of background PIDs to 
*				to collect
*		i (int) index of the PID to be removed
*		count (int) count of the number of PID's in backgroundPid
* Returns:
*		count (int) number of PID's in backgroundPid after removal
*
*/
int removePid(pid_t backgroundPid[100], int i, int count)
{
	if (count > 1)
	{
		// Move Last PID to Current Place and Decrement Count
		count--;
		backgroundPid[i] = backgroundPid[count];
	}
	else
	{
		// There is Only 1 Process. Reset to O
		count--;
		backgroundPid[count] = 0;
	}
	
	return count;
}


/* int checkBgStatus(int backgroundPid[], pid_t childPid, int count)
* Description: Checks for any background process that have finished
*			and prints their exit status or termination signal.
* Parameters:
*		backgroundPid[] (pid_t []) array of background PIDs to
*				to collect
*		count (int) count of the number of PID's in backgroundPid
* Returns:
*		count (int) number of PID's in backgroundPid after removal
*
*/
int checkBgStatus(int backgroundPid[], int count)
{
	pid_t childPid;
	int childStatus;
	int i = 0;
	while (backgroundPid[i] > 0 && i < 100)
	{
		childPid = waitpid(backgroundPid[i], &childStatus, WNOHANG);

		if (childPid < 0)
		{
			perror("Error checking for background process status");

		}
		else if (childPid > 0)
		{
			printBgStatus(childPid, childStatus);
			count = removePid(backgroundPid, i, count);
		}
		i++;
	}

	return count;
}


/* int runShell()
* Description: Loops for user input, processes that input into command structs,
*			splits to child processes either in foreground or background, and 
*			then executes the commands.  Built in commands (cd, exit status)
*			happen before the fork and occur in the parent process.  Blocks
*			background processes on SIGTSTP (ctrl z) and allows it again when 
*			STGSTP is used again.
*
* Sources: 
*		https://replit.com/@cs344/4forkexamplec#main.c
*		https://canvas.oregonstate.edu/courses/1884946/pages/exploration-process-api-monitoring-child-processes?module_item_id=21835973
*
*/
int runShell()
{
	char* userInput = NULL;
	size_t bufferSize = 0;
	int bgCount = 0;						// Number of backgroundPid[] elements
	command* newCommand = NULL;
	command* head = NULL;
	command* tail = NULL;
	foregroundOnly = 0;
	int lastFgStatus = 0;
	pid_t backgroundPid[100];
	for (int i = 0; i < 100; i++)
	{
		backgroundPid[i] = 0;
	}

	sigact ignore_action = { { 0 } }, switchMode_SIGTSTP = { { 0 } }, default_action = { { 0 } };
	setHandlers(&ignore_action, &switchMode_SIGTSTP, &default_action);

	do
	{	
		// ----------------------------------------
		// Get and Process User Input
		// ----------------------------------------
		// if NULL there was an error with getline
		userInput = getInput(userInput, &bufferSize);
		if (userInput == NULL) continue;

		// Process Command into Command Struct
		// to allow for execution
		newCommand = createCommand(userInput);
		
		// Add to List of Command Structs
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

		// If Space, New Line, or Comment Prompt User Again
		if (newCommand->commandArgs[0] == NULL)	continue;


		// ----------------------------------------
		// Execute Built In Commands
		// ----------------------------------------
		// If Built in cd Command Execute It
		// change directory to home or path argument
		else if (strncmp(newCommand->commandArgs[0], "cd", sizeof("cd")) == 0)
		{
			changeDirectory(newCommand);
		}

		// If Built in status Command Execute It
		// print last foreground command status
		else if (strncmp(newCommand->commandArgs[0], "status", sizeof("status")) == 0)
		{
			printStatus(lastFgStatus);
		}

		// If Built in exit Command Execute It
		// terminate all child processes, free memory and exit.
		else if (strncmp(newCommand->commandArgs[0], "exit", sizeof("exit")) == 0)
		{
			prepareExit(backgroundPid, head, userInput);
			break;
		}

		// ----------------------------------------
		// Fork a Child and Execute Commands
		// ----------------------------------------
		// Source: Parent/Child Process https://canvas.oregonstate.edu/courses/1884946/pages/exploration-process-api-monitoring-child-processes?module_item_id=21835973
		else
		{

			int   childStatus;
			pid_t childPid = fork();

			if (childPid == -1) {
				perror("fork() failed");
				exit(1);
			}

			// ----------------------------------------
			// Child Process
			// ----------------------------------------
			else if (childPid == 0) 
			{
				runChild(newCommand, &ignore_action, &switchMode_SIGTSTP, &default_action);
			}

			// ----------------------------------------
			// Parent Process
			// ----------------------------------------
			else {
				// Run Child Process in Background
				if (!foregroundOnly && newCommand->inBackground)
				{
					backgroundPid[bgCount] = childPid;
					bgCount++;
					childPid = waitpid(childPid, &childStatus, WNOHANG);
				}

				// Run Child Process in Foreground
				else
				{
					lastFgStatus = awaitFgChild(childPid);
				}

				bgCount = checkBgStatus(backgroundPid, bgCount);

			}
			
		}

	} while (true);
	
	return 0;
}