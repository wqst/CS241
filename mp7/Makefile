# CS 241
# University of Illinois

CC = gcc
INC = -I. -Ilibdslocal/
FLAGS = -g -W -Wall
LIBS = -lpthread

OBJECTS = libds.o libmapreduce.o


all: alice.txt test1 test2 test3 test4 test5 test6 doc/html

alice.txt:
	wget https://courses.engr.illinois.edu/cs241/mp/alice.txt

doc/html: libmapreduce.c
	doxygen doc/Doxyfile
	cp doc/reduce.jpg doc/html/

test1: $(OBJECTS) test1.c
	$(CC) $(FLAGS) $^ -o $@ $(LIBS)

test2: $(OBJECTS) test2.c
	$(CC) $(FLAGS) $^ -o $@ $(LIBS)

test3: $(OBJECTS) test3.c
	$(CC) $(FLAGS) $^ -o $@ $(LIBS)

test4: $(OBJECTS) test4.c
	$(CC) $(FLAGS) $^ -o $@ $(LIBS)

test5: $(OBJECTS) test5.c
	$(CC) $(FLAGS) $^ -o $@ $(LIBS)

test6: $(OBJECTS) test6.c
	$(CC) $(FLAGS) $^ -o $@ $(LIBS)

libds.o: libds/libds.c libds/libds.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)

libmapreduce.o: libmapreduce.c libmapreduce.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)



clean:
	rm -rf *.o *.d test1 test2 test3 test4 test5 test6 doc/html *~
