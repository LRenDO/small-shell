/*
* Author: Ren Demeis-Ortiz
* Course: CS 344 Sec. 400
* Description: Header file with declarations for Signal Handlers
* Sources: 
*	sig_atomic_t https://stackoverflow.com/questions/42232257/shell-in-c-only-respond-to-sigtstp-after-pressing-enter-on-keyboard
*/

#ifndef signalHandlers_H
#define signalHandlers_H
#include <signal.h>
volatile sig_atomic_t foregroundOnly;

void changeModeOnSIGTSTP(int sigNum);

#endif