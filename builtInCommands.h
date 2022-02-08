/*
* Author: Ren Demeis-Ortiz
* Course: CS 344 Sec. 400
* Description: Header file with declarations for Built In Command functions
*/

#ifndef builtInCommands_H
#define builtInCommands_H
#include <sys/types.h>
#include "commands.h"

void changeDirectory(command* currCommand);
void printStatus(int status);
void printBgStatus(pid_t childPid, int status);
void prepareExit(pid_t pidArray[100], command* head, char* userInput);

#endif