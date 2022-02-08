/*
* Author: Ren Demeis-Ortiz
* Course: CS 344 Sec. 400
* Description: Header file with declarations for command functions
*			and struct.
*/
#include <stdbool.h>

#ifndef commands_H
#define commands_H
typedef struct commands
{
	char* commandArgs[513];
	char* inputFile;
	char* outputFile;
	bool inBackground;
	int numArgs;
	struct commands* nextCommand;

}command;

void initializeCommand(command* currCommand);
char* replaceVariables(char* input);
void parseVariables(command* currCommand);
bool isBuiltIn(char* input);
void setBackgroundFlag(command* currCommand, int i);
command* createCommand(char* input);
void deconstructCommands(command* currCommand);

// Print Functions Used for Debugging Only
void printCommandArgs(command* currCommand);
void printInputFile(command* currCommand);
void printOutputFile(command* currCommand);
void printInBackground(command* currCommand);
void printNumArgs(command* currCommand);
void printNextCommand(command* currCommand);
void printCommands(command* currCommand);

#endif