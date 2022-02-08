/*
* Author: Ren Demeis-Ortiz
* Course: CS 344 Sec. 400
* Description: Function definitions for Signal Handlers
* Sources: https://canvas.oregonstate.edu/courses/1884946/pages/exploration-signal-handling-api?module_item_id=21835981
*/
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "signalHandlers.h"



// DELETE Add SIGCHLD if time! https://stackoverflow.com/questions/7171722/how-can-i-handle-sigchld


/*
* void changeModeOnSIGTSTP(int sigNum);
* Description:  Toggles global variable foregroundOnly on SIGSTP signal. If it was
*			0 it toggles to 1 and if it was 1 it toggles to 0. This allows parent to
*			track entering and exiting foreground only mode.
*
* Parameters: 
*		requires foregroundOnly (volatile sig_atomic_t) variable declared in header
*
* Returns:
*		foregroundOnly (volatile sig_atomic_t) variable is toggled between 1 and 0. 
*		1 for on and 0 for off.
* Sources: sig_atomic_t https://stackoverflow.com/questions/42232257/shell-in-c-only-respond-to-sigtstp-after-pressing-enter-on-keyboard
*/
void changeModeOnSIGTSTP(int sigNum)
{
	int bytes = -1;
	
	if (!foregroundOnly)
	{
		int length = 50;
		foregroundOnly = 1;
		char message[] = "Entering foreground-only mode (& is now ignored)\n";
		while (bytes == -1 || bytes < length)
		{
			bytes = write(STDOUT_FILENO, message, length);
		}
	}
	else
	{
		int length = 32;
		foregroundOnly = 0;
		char message[] = "Exiting foreground - only mode\n";
		while ( bytes == -1 || bytes < length) 
		{
			bytes = write(STDOUT_FILENO, message, length);
		}
	}
}