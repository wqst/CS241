#
# Machine Problem #1
# CS 241
#

CC = gcc
INC = -I.
FLAGS = -W -Wall -g

all: part1 part2 part2_hacker doc/html

doc/html: part1.c part2.c part2_hacker.c libdictionary/libdictionary.c libdictionary/libdictionary.h
	doxygen doc/Doxyfile

part1: part1.o mp1-functions.o
	$(CC) $^ -o $@

part2: part2.o libdictionary/libdictionary.o
	$(CC) $^ -o $@

part2_hacker: part2_hacker.o libdictionary/libdictionary.o
	$(CC) $^ -o $@

part1.o: part1.c
	$(CC) -c $(FLAGS) $(INC) $< -o $@
	
mp1-functions.o: mp1-functions.c mp1-functions.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@

part2.o: part2.c libdictionary/libdictionary.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@

part2_hacker.o: part2_hacker.c libdictionary/libdictionary.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@

libdictionary/libdictionary.o: libdictionary/libdictionary.c libdictionary/libdictionary.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@

.PHONY : clean
clean:
	-rm -f *.o libdictionary/*.o part1 part2 part2_hacker
