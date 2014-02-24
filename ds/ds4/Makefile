#
# CS 241
# The University of Illinois
#

CC = gcc
INC = -I.
FLAGS += -W -Wall -g -O0

all: mshell

mshell: mshell.c
	$(CC) $(INC) $(FLAGS) $^ -o $@

clean:
	-rm -f mshell
