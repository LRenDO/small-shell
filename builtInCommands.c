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

void changeDirectory(command* currCommand)
{
	printf("cd\n");
}

void getStatus() 
{
	printf("status\n");
}

void prepareExit()
{
	printf("exit\n");
}
