/*
* Author: Ren Demeis-Ortiz
* Course: CS 344 Sec. 400
* Description: Function definitions for DELETE functions
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
* Description:
*
 Parameters: 
*		requires foreground
* Sources:
*
*/
char* getInput(char* userInput, size_t* bufferSizePtr)
{
	// Prompt User and Get Command Input
	printf(": ");
	fflush(stdout);
	int numChar = getline(&userInput, bufferSizePtr, stdin);

	// Check for std Error
	// without clearing errors getline will not read user input
	// and the loop will not terminate
	// Source: https://edstem.org/us/courses/16718/discussion/1065019
	if (ferror(stdin) || numChar == -1)
	{
		clearerr(stdin);
		return NULL;
	}

	// Check for Empty String 
	// to prevent errors when processing the string
	// into command struct
	if (strlen(userInput) < 1)
	{
		perror("Getline Error");
		return NULL;
	}

	return userInput;
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
	int count = 0;						// Number of backgroundPid[] elements
	command* newCommand = NULL;
	command* head = NULL;
	command* tail = NULL;
	foregroundOnly = 0;
	int lastFgJobStatus = 0;
	pid_t lastFgJobPid = 0;
	pid_t backgroundPid[100];
	for (int i = 0; i < 100; i++)
	{
		backgroundPid[i] = 0;
	}


	// Set Parent Handlers
	// which will allow for different signaling options
	// for parent and child
	// Source: https://canvas.oregonstate.edu/courses/1884946/pages/exploration-signal-handling-api?module_item_id=21835981
	struct sigaction ignore_action = { { 0 } }, switchMode_SIGTSTP = { { 0 } }, default_action = {{ 0 }};
	default_action.sa_handler = SIG_DFL;

	// Parent Ignores SIGINT (CTRL C)
	ignore_action.sa_handler = SIG_IGN;
	sigaction(SIGINT, &ignore_action, NULL);

	// SIGTSTP (CTRL Z) Signal Handler for Foreground-Only Mode
	// allows for to toggle on and off
	switchMode_SIGTSTP.sa_handler = changeModeOnSIGTSTP;
	sigfillset(&switchMode_SIGTSTP.sa_mask);
	switchMode_SIGTSTP.sa_flags = 0;
	sigaction(SIGTSTP, &switchMode_SIGTSTP, NULL);



	do
	{
		// Get User Input
		userInput = getInput(userInput, &bufferSize);
		if (userInput == NULL) continue;

		// Process Command into Command Struct
		// to allow for execution
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

		// If Space, New Line, or Comment Prompt User Again
		if (newCommand->commandArgs[0] == NULL)
		{
			continue;
		}

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
			printStatus(lastFgJobStatus);
		}

		// If Built in exit Command Execute It
		// terminate all child processes, free memory and exit.
		else if (strncmp(newCommand->commandArgs[0], "exit", sizeof("exit")) == 0)
		{
			prepareExit(backgroundPid, head, userInput);
			break;
		}

		// Fork a Child and Execute Commands
		// Source: Forking Chid Process https://canvas.oregonstate.edu/courses/1884946/pages/exploration-process-api-monitoring-child-processes?module_item_id=21835973
		else
		{

			int   childStatus;
			pid_t childPid = fork();

			if (childPid == -1) {
				perror("fork() failed");
				exit(1);
			}
			// This is child
			else if (childPid == 0) {
				
				// Set Signal handlers for Children to Ignore SIGSTP
				sigaction(SIGTSTP, &ignore_action, &switchMode_SIGTSTP);

				// If Running in Foreground Set SIGINT to Terminate Process
				if (foregroundOnly || !newCommand->inBackground)
				{
					sigaction(SIGINT, &default_action, &ignore_action);
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
			// This is Parent
			else {
				// The parent process executes this branch
				// DELETE printf("Child pid: %d\n", childPid);

				// Run in Background
				if (!foregroundOnly && newCommand->inBackground)
				{
					backgroundPid[count] = childPid;
					count++;
					childPid = waitpid(childPid, &childStatus, WNOHANG);
					// DELETE printf("In the parent process waiting on background waitpid returned value %d\n", childPid);
				}
				// Run in Foreground
				else
				{
					childPid = waitpid(childPid, &childStatus, 0);
					while (childPid < 1)
					{
						childPid = waitpid(childPid, &childStatus, 0);
					}

					if (WIFSIGNALED(childStatus))
					{
						// DELETE printf("Foreground PID %d terminated by signal %d\n", childPid, WTERMSIG(childStatus));
						printf("Foreground PID %d terminated by signal %d\n", childPid, WTERMSIG(childStatus));
						lastFgJobStatus = WTERMSIG(childStatus);
					}
					else if (WIFEXITED(childStatus)) {
						// printf("Foreground PID %d exited normally with status %d\n", childPid, WEXITSTATUS(childStatus)); // DELETE
						lastFgJobStatus = WEXITSTATUS(childStatus);
					}

					// Update Last Foreground JobPID
					lastFgJobPid = childPid;
				}

				int i = 0;
				while (backgroundPid[i] > 0)
				{
					childPid = waitpid(backgroundPid[i], &childStatus, WNOHANG);

					if (childPid < 0)
					{
						perror("Error checking for background process status");

					}
					else if (childPid > 0)
					{
						printBgStatus(childPid, childStatus);

						// Remove PID from Background List
						if (count > 1)
						{
							count--;
							backgroundPid[i] = backgroundPid[count];
						}
						else
						{
							count--;
							backgroundPid[count] = 0;
						}
						
					}
					i++;
				}


			}
			
		}

	} while (true);
	
	return 0;
}