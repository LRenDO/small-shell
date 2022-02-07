/*
* Author: Ren Demeis-Ortiz
* Course: CS 344 Sec. 400
* Description: Header file with declarations for Built In Command functions
* Sources:
*/

#ifndef builtInCommands_H
#define builtInCommands_H
#include <sys/types.h>
#include "commands.h"

void changeDirectory(command* currCommand);
void printStatus(int status);
void prepareExit(pid_t pidArray[]);

#endif