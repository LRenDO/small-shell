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
*		https://canvas.oregonstate.edu/courses/1884946/pages/exploration-process-api-monitoring-child-processes?module_item_id=21835973
*
*/


int runShell()
{
	printf("pid: %d\n", getpid()); //DELETE after comparing with $$ output
	char* userInput = NULL;
	int lastFgJobStatus = 0;
	pid_t lastFgJobPid = 0;
	pid_t backgroundPid[100];
	for(int i=0; i<100; i++)
	{
		backgroundPid[i] = -1;
	}
	int count = 0;
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
			printStatus(lastFgJobStatus);
		}

		// If Built in exit Command
		else if (strncmp(newCommand->commandArgs[0], "exit", sizeof("exit")) == 0)
		{
			// Terminate any remaining processes
			prepareExit();
			break;
		}

		// Else Pass Command and Args to Exec or Process Input/Output
		// Source: Else clause adapted from https://canvas.oregonstate.edu/courses/1884946/pages/exploration-process-api-monitoring-child-processes?module_item_id=21835973
		else
		{
			printf("Parent process's pid = %d\n", getpid());

			int   childStatus;
			pid_t childPid = fork();

			if (childPid == -1) {
				perror("fork() failed");
				exit(1);
			}
			// This is child
			else if (childPid == 0) {

				printf("Child process's pid = %d\n", getpid());
				
				if (newCommand->inputFile != NULL)
				{
					printf("do input stuff\n");
				}

				if (newCommand->outputFile != NULL)
				{
					printf("do output stuff\n"); // DELETE
				}

				sleep(15);  // DELETE
				execvp(newCommand->commandArgs[0], newCommand->commandArgs);
				perror("Execute Error");
				exit(EXIT_FAILURE);
			}
			// This is Parent
			else {
				// The parent process executes this branch
				printf("In parent Child's pid = %d\n", childPid);

				// If Running in Background
				if (newCommand->inBackground)
				{
					backgroundPid[count] = childPid;
					count++;
					childPid = waitpid(childPid, &childStatus, WNOHANG);
					printf("In the parent process waiting on background waitpid returned value %d\n", childPid);
				}
				else
				{
					childPid = waitpid(childPid, &childStatus, 0);

					if (WIFEXITED(childStatus)) {
						printf("Child %d exited normally with status %d\n", childPid, WEXITSTATUS(childStatus));
					}
					else {
						printf("Child %d exited abnormally due to signal %d\n", childPid, WTERMSIG(childStatus));
					}

					// Update Last Foreground Job Status and PID
					lastFgJobPid = childPid;
					lastFgJobStatus = childStatus;
				}

				int i = 0;
				while (backgroundPid[i] > 0)
				{
					childPid = waitpid(backgroundPid[i], &childStatus, WNOHANG);

					if (childPid < 0)
					{
						perror("Error checking for background process status");
						fflush(stderr);

					}
					else if (childPid > 0)
					{
						if (WIFEXITED(childStatus))
						{
							printf("Background Child %d exited normally with status %d\n", childPid, WEXITSTATUS(childStatus));
						}
						else
						{
							printf("Background Child %d exited abnormally due to signal %d\n", childPid, WTERMSIG(childStatus));
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
							backgroundPid[count] = -1;
						}
						
					}
					i++;
				}


			}
			printf("The process with pid %d is returning to loop\n", getpid());
			
		}

	} while (true);
	
	free(userInput);
	deconstructCommands(head);

	return 0;
}