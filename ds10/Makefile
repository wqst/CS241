# CS 241
# University of Illinois

CC = gcc
INC = -I. -Ilibdslocal/
FLAGS = -g -W -Wall
LIBS = -lpthread

ds10: ds10.c
	$(CC) $(FLAGS) $^ -o $@ $(LIBS)	

clean:
	rm -rf *.o *.d ds10 doc/html *~
