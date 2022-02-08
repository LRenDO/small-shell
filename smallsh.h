/*
* Author: Ren Demeis-Ortiz
* Course: CS 344 Sec. 400
* Description: Header file with declarations for smallsh functions
* Sources: 
*/
# include <sys/types.h>
# include "commands.h"
# include "signalHandlers.h"

#ifndef smallsh_H
#define smallsh_H

char* getInput(char* userInput, size_t* bufferSize);
void runChild(command* newCommand, sigact* ignore_action, 
				sigact* switchMode_SIGTSTP, sigact* default_action);
int awaitFgChild(pid_t childPidPtr);
int removePid(pid_t backgroundPid[100], int i, int count);
int checkBgStatus(int backgroundPid[], int count);
int runShell();

#endif