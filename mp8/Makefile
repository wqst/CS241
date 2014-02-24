# CS 241
# University of Illinois

CC = gcc
INC = -I. -Ilibs
FLAGS = -g -W -Wall
LIBS = -lpthread

all: server doc/html

doc/html: server.c libs/libdictionary.c libs/libhttp.c queue.c doc/Doxyfile
	doxygen doc/Doxyfile

server: libdictionary.o libhttp.o queue.o server.c
	$(CC) $(FLAGS) $(INC) $^ -o $@ $(LIBS)

libdictionary.o: libs/libdictionary.c libs/libdictionary.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)

libhttp.o: libs/libhttp.c libs/libhttp.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)

queue.o: queue.c queue.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)

clean:
	$(RM) -r *.o server doc/html