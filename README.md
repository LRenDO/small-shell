# smallsh Program

This program runs a mini shell and allows programs to be run in the
background by using & at the end of the command.  Additionally, it allows
for foreground tasks to terminate by ctrl-c and sets a foreground only
mode which can be toggled by ctrl z. It supports redirecting stdin and
stdout by using < and > respectively. It does not check for command
syntax errors, but returns error messages for attempts at executing 
programs. Command syntax below.
```
command [arg1 arg2 ...] [< input_file] [> output_file] [&]
```
Note: Highly recommend to create a separate directory for testing. Know
	that if you use cd the working directory will be changed and you may 
	change file and directory contents in that directory depending on the  
	commands you run.


## Requirements

Linux

gcc (std=gnu99)


## Create Executable

Navigate to directory where files are saved

Use command below:
```
make
```
## Run the Program

Navigate to directory where movie files are saved

Create Executable as described above

Use command below:
```
smallsh
```

## Removing Executable Only

Navigate to directory where files are saved

Use command below:
```
make clean
```

## Removing All Files (except files and directories created during execution)

Navigate to directory where files are saved

Use command below:
```
make deepclean
```
