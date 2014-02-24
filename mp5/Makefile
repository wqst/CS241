#
# CS 241
#

CC = gcc
INC = -I.
FLAGS = -Wall -Wextra -Werror -Wno-unused -g -Wno-unused-parameter

all: simulator queuetest doc/html

doc/html: doc/Doxyfile libpriqueue/libpriqueue.c libscheduler/libscheduler.c
	doxygen doc/Doxyfile

simulator: simulator.o libscheduler/libscheduler.o libpriqueue/libpriqueue.o
	$(CC) $^ -o $@

queuetest: queuetest.o libpriqueue/libpriqueue.o
	$(CC) $^ -o $@

queuetest.o: queuetest.c
	$(CC) -c $(FLAGS) $(INC) $< -o $@

libscheduler/libscheduler.o: libscheduler/libscheduler.c libscheduler/libscheduler.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@

libpriqueue/libpriqueue.o: libpriqueue/libpriqueue.c libpriqueue/libpriqueue.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@

simulator.o: simulator.c libscheduler/libscheduler.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@




.PHONY : clean
clean:
	rm -rf simulator queuetest *.o libscheduler/*.o libpriqueue/*.o doc/html
