#
# CS 241
# The University of Illinois
#

CC = gcc
INC = -I.
FLAGS += -W -Wall -g -O2 -pthread

all: prodcons doc/html

prodcons: ds8.c
	$(CC) $(INC) $(FLAGS) $^ -o $@

doc/html:
	doxygen doc/Doxyfile

clean:
	-rm -rf prodcons *~ doc/html