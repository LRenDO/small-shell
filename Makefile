##****************************************************************************
## Program Name: makefile
## Author: Ren Demeis-Ortiz
## Description: Makefile that builds, cleans, debugs, and zips project files.
## Credit: Adapted from work in CS162
##		   Adapted from CS162 Module one makefile lecture unless source info
##         below says otherwise.
##         https://oregonstate.instructure.com/courses/1738874/files/76040054
##****************************************************************************

#----------#
# Complier #
#----------#
CXX = gcc

#------------------------------------------------#
# CXX Flags                                      #
# Credit: Harlan James                           #
# Source: https://oregonstate.instructure.com/   # 
# courses/1738874/files/76040124/download?wrap=1 #
#------------------------------------------------#
CFLAGS = -std=gnu99 -Wall -g -pedantic

#----------------------#
# Executable File Name #
#----------------------#
PROJ = smallsh

#-------------#
# Main  File  #
#-------------#
MAIN = main.c

#---------------#
# Source  Files #
#---------------#
SRCS = smallsh.c
SRCS += commands.c
SRCS += builtInCommands.c
SRCS += signalHandlers.c

#---------------#
# Header  Files #
#---------------#
HDRS = smallsh.h
HDRS += commands.h
HDRS += builtInCommands.h
HDRS += signalHandlers.h

#---------------#
# Test  Files   #
#---------------#
TESTF = main_test.c

#------------------------------------#
# Additional Files to Include in Zip #
#------------------------------------#
ADD = README.txt
ADD += Makefile

#------------------#
# Name of Zip File #
#------------------#
ZNAME = demeisol_program2.zip

#------------------------------------------------#
# Valgrind Options                               #
# Credit: Harlan James                           #
# Source: https://oregonstate.instructure.com/   # 
# courses/1738874/files/76040124/download?wrap=1 #
#------------------------------------------------#
VALOPT = --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes 
#------------------------------------------------#
# Object Files                                   #         
# Credit: Harlan James                           #
# Source: https://oregonstate.instructure.com/   # 
# courses/1738874/files/76040124/download?wrap=1 #
#------------------------------------------------#
OBJS = $(SRCS:.cpp=.o)

#------------------------------------------------#
# Compile Object Files                           #         
# Credit: Harlan James                           #
# Source: https://oregonstate.instructure.com/   # 
# courses/1738874/files/76040124/download?wrap=1 #
#------------------------------------------------#
%.o: %.c %.h
	${CXX} ${CFLAGS} -c $(@:.o=.c)

#-------------------#
# Link Object Files #
#-------------------#
${PROJ}: ${OBJS} ${HDRS}
	${CXX} ${CFLAGS} ${MAIN} ${OBJS} -o ${PROJ}

#------------------------------------------------#
# Debug                                          #
# Credit: Harlan James                           #
# Source: https://oregonstate.instructure.com/   # 
# courses/1738874/files/76040124/download?wrap=1 #
#------------------------------------------------#
.PHONY: valdebug gdbdebug test clean deepclean zip

valdebug: 
	valgrind ${VALOPT} ./${PROJ} movies_sample_1.csv
	
gdbdebug: 
	gdb ${VALOPT} ./${PROJ} movies_sample_1.csv
	
#----------------------------#
# Compile and Link Test File #
#----------------------------#
test:
	${CXX} ${CFLAGS} -o ${PROJ} ${TESTF} ${SRCS} 
	
#------------------------------------------------------#
# Remove Object Files and Executable                   #
# Credit: Code used from https://ftp.gnu.org/          #
# old-gnu/Manuals/make-3.79.1/html_chapter/make_2.html #
#------------------------------------------------------#
clean:
	-rm ${PROJ}

#------------------------------------------------------#
# Remove All Files and Executable                      #
# Credit: Code used from https://ftp.gnu.org/          #
# old-gnu/Manuals/make-3.79.1/html_chapter/make_2.html #
#------------------------------------------------------#
deepclean:
	-rm ${SRCS} ${MAIN} ${HDRS} ${PROJ} ${ADD}

#------------------------------------------------------#
# Zip Project Files                                    #
#------------------------------------------------------#
zip:
	zip -D ${ZNAME} ${SRCS} ${MAIN} ${HDRS} ${ADD}
