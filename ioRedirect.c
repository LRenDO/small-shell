/*
* Author: Ren Demeis-Ortiz
* Course: CS 344 Sec. 400
* Description: Function definitions for Input Output Redirection
* Sources: https://canvas.oregonstate.edu/courses/1884946/pages/exploration-signal-handling-api?module_item_id=21835981
*/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "ioRedirect.h"


/*
* void redirectStdin(char* inputFile);
* Description:  Redirects stdin to filename passed. Exits
*		process on failure.
* Parameters:
*		inputFile (char*) file name of file to redirect stdin to
*
* Sources: https://canvas.oregonstate.edu/courses/1884946/pages/exploration-processes-and-i-slash-o?module_item_id=21835982
*/
void redirectStdin(char* inputFile)
{
	int inputFd = open(inputFile, O_RDONLY);
	if (inputFd == -1) {
		printf("Cannot open \"%s\" for input: ", inputFile);
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

/*
* void redirectStdout(char* inputFile);
* Description:  Redirects stdout to filename passed. Exits
*		process on failure.
* Parameters:
*		outputFile (char*) file name of file to redirect stdout to
*
* Sources: https://canvas.oregonstate.edu/courses/1884946/pages/exploration-processes-and-i-slash-o?module_item_id=21835982
*/
void redirectStdout(char* outputFile)
{
	// Open Output File
	int outputFd = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (outputFd == -1) 
	{
		printf("Cannot open \"%s\" for output: ", outputFile);
		fflush(stdout);
		perror("");
		exit(EXIT_FAILURE);
	}

	// Redirect stdout to Output File
	int result = dup2(outputFd, 1);
	if (result == -1) 
	{
		perror("Output File Redirect Error");
		exit(EXIT_FAILURE);
	}
}