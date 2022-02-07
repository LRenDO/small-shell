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
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <fcntl.h>
#include "smallsh.h"
#include "commands.h"
#include "builtInCommands.h"
#include "signalHandlers.h"


/* int runShell()
* Description: Runs shell original parent process
*
* Parameters: 
*		
* Sources: 
*		https://replit.com/@cs344/4forkexamplec#main.c
*		https://canvas.oregonstate.edu/courses/1884946/pages/exploration-process-api-monitoring-child-processes?module_item_id=21835973
*
*/


int runShell()
{
	char* userInput = NULL;
	int count = 0;
	command* newCommand = NULL;
	command* head = NULL;
	command* tail = NULL;
	size_t bufferSize = 0;
	int numChar = 0;
	foregroundOnly = 0;
	int lastFgJobStatus = 0;
	pid_t lastFgJobPid = 0;
	pid_t backgroundPid[100];
	for (int i = 0; i < 100; i++)
	{
		backgroundPid[i] = 0;
	}

	// Set Parent Handlers
	// Source: https://canvas.oregonstate.edu/courses/1884946/pages/exploration-signal-handling-api?module_item_id=21835981
	struct sigaction ignore_action = { { 0 } }, switchMode_SIGTSTP = { { 0 } }, default_action = {{ 0 }};
	default_action.sa_handler = SIG_DFL;

	// Parent Ignores SIGINT (CTRL C)
	ignore_action.sa_handler = SIG_IGN;
	sigaction(SIGINT, &ignore_action, NULL);
	// DELETE sigaction(SIGTSTP, &ignore_action, NULL);

	// SIGTSTP (CTRL Z) Signal Handler for Foreground Only Mode to Toggle On and Off
	switchMode_SIGTSTP.sa_handler = changeModeOnSIGTSTP;
	sigfillset(&switchMode_SIGTSTP.sa_mask);
	switchMode_SIGTSTP.sa_flags = 0; // DELTE SA_RESTART
	sigaction(SIGTSTP, &switchMode_SIGTSTP, NULL);



	do
	{

		printf(": ");
		fflush(stdout);

		numChar = getline(&userInput, &bufferSize, stdin);

		// Source: https://edstem.org/us/courses/16718/discussion/1065019
		if(ferror(stdin))
		{
			// DELETE perror("Getline Error");
			clearerr(stdin);
			continue;
		}

		if (strlen(userInput) < 1)
		{
			perror("numChar Getline");
			printf("NUMCHAR %d\n", numChar);
			printf("NUMCHAR %s\n", userInput);
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
			printStatus(lastFgJobStatus);
		}

		// If Built in exit Command
		else if (strncmp(newCommand->commandArgs[0], "exit", sizeof("exit")) == 0)
		{
			// Terminate any remaining processes
			prepareExit(backgroundPid, head, userInput);
			break;
		}

		// DELETE If time refactor to split process
		// Else Pass Command and Args to Exec or Process Input/Output
		// Source: Else clause adapted from https://canvas.oregonstate.edu/courses/1884946/pages/exploration-process-api-monitoring-child-processes?module_item_id=21835973
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
				// If Running in Background Redirect stdin and stdout to /dev/null
				else
				{
					printf("Background pid is %d\n", getpid());
					fflush(stdout);
					char* devNull = "/dev/null";
					if (newCommand->inputFile == NULL)
					{
						
						// DELETE sleep(15); 
						int toNullFd = open(devNull, O_RDONLY);
						if (toNullFd == -1) {
							printf("Cannot open \"%s\" for input", devNull);
							fflush(stdout);
							perror("");
							fflush(stderr);
							exit(EXIT_FAILURE);
						}

						// Redirect stdin to Input File
						int result = dup2(toNullFd, 0);
						if (result == -1) {
							perror("Input File Redirect Error");
							exit(EXIT_FAILURE);
						}
					}

					if (newCommand->outputFile == NULL)
					{
						// Open Output File
						int fromNullfd = open(devNull, O_WRONLY | O_CREAT | O_TRUNC, 0644);
						if (fromNullfd == -1) {
							printf("Cannot open \"%s\" for output", devNull);
							fflush(stdout);
							perror("");
							exit(EXIT_FAILURE);
						}

						// Redirect stdout to Output File
						int result = dup2(fromNullfd, 1);
						if (result == -1) {
							perror("Output File Redirect Error");
							exit(EXIT_FAILURE);
						}
					}
				}
				
				//Source: https://canvas.oregonstate.edu/courses/1884946/pages/exploration-processes-and-i-slash-o?module_item_id=21835982
				if (newCommand->inputFile != NULL)
				{
					int inputFd = open(newCommand->inputFile, O_RDONLY);
					if (inputFd == -1) {
						printf("Cannot open \"%s\" for input: ", newCommand->inputFile);
						fflush(stdout);
						perror("");
						exit(EXIT_FAILURE);
					}

					// Redirect stdin to Input File
					int result = dup2(inputFd, 0);
					if (result == -1) {
						perror("Input File Redirect Error");
						exit(EXIT_FAILURE);
					}

					
				}

				if (newCommand->outputFile != NULL)
				{
					// Open Output File
					int outputFd = open(newCommand->outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
					if (outputFd == -1) {
						printf("Cannot open \"%s\" for output: ", newCommand->inputFile);
						fflush(stdout);
						perror("");
						exit(1);
					}

					// Redirect stdout to Output File
					int result = dup2(outputFd, 1);
					if (result == -1) {
						perror("Output File Redirect Error");
						exit(1);
					}
				}
				// DELETE sleep(3);
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
						if (WIFSIGNALED(childStatus))
						{
							printf("Background PID %d is done: terminated by signal %d\n", childPid, WTERMSIG(childStatus));
							fflush(stdout);
						}
						else if (WIFEXITED(childStatus)) {
							printf("Background PID %d is done: exit value %d\n", childPid, WEXITSTATUS(childStatus));
							fflush(stdout);
						}

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