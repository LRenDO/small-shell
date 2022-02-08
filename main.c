/*
* Author: Ren Demeis-Ortiz
* Course: CS 344 Sec. 400
* Description: This program runs a mini shell and allows programs to be run in the
*			background by using & at the end of the command.  Additionally it allows
*			for foreground tasks to terminate by ctrl-c and sets a foreground only
*			mode which can be toggled by ctrl z. It supports redirecting stdin and
*			stdout by using < and > respectively. It does not check for command
*			syntax errors, but returns error messages for attempts at executing 
*			programs. 
*			
*			Command syntax: 
*			command [arg1 arg2 ...] [< input_file] [> output_file] [&]
*/
#include "smallsh.h"

int main()
{
	runShell();
	return 0;
}
