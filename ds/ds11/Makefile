#
# CS 241
# The University of Illinois
#

CC = gcc
FLAGS += -W -g -O0 -I. -lm

all: ds11_client ds11_server doc/html

doc/html:
	doxygen doc/Doxyfile ds11_aux.c

ds11_server: ds11_server.c ds11_aux.c
	$(CC) $(FLAGS) $^ -o $@

ds11_client: ds11_client.c ds11_aux.c
	$(CC) $(FLAGS) $^ -o $@

clean:
	-rm -f ds11_server ds11_client -r doc/html
