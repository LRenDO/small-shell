/*
* Author: Ren Demeis-Ortiz
* Course: CS 344 Sec. 400
* Description: Header file with declarations for DELETE functions
* Sources:
*/
#include <stdbool.h>

#ifndef commands_H
#define commands_H
struct commands
{
	// DELETE char* command;
	char* commandArgs[513];
	// DELETE char* inputFile;
	// DELETE char* outputFile;
	bool inBackground;
	int numArgs;
	struct commands* nextCommand;

};

typedef struct commands command;

void initializeCommand(command* currCommand);
void replaceVariables(command* currCommand, int i);
void parseVariables(command* currCommand);
command* createCommand(char* input);
void deconstructCommands(command* currCommand);

// DELETE void printCommand(command* currCommand);
void printCommandArgs(command* currCommand);
// DELETE void printInputFile(command* currCommand);
// DELETE void printOutputFile(command* currCommand);
void printInBackground(command* currCommand);
void printNumArgs(command* currCommand);
void printNextCommand(command* currCommand);
void printCommands(command* currCommand);

#endif