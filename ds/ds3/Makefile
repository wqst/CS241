#
# CS 241
# The University of Illinois
#

CC = gcc
FLAGS = -O2 -W -Wall

all: ptr_acrobats doc/html

doc/html:
	doxygen doc/Doxyfile

ptr_acrobats: ptr_acrobats.c
	$(CC) $< $(FLAGS) -o $@

clean:
	-rm -f ptr_acrobats
	-rm -rf doc/html
