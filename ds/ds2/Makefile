#
# CS 241
# The University of Illinois
#

CC = gcc
INC = -I.
FLAGS = -O2 -W -Wall

all: alloc.so contest-alloc.so mreplace mcontest tester-agents doc/html

doc/html:
	doxygen doc/Doxyfile

alloc.so: alloc.c
	$(CC) $^ $(FLAGS) -o $@ -shared -fPIC

contest-alloc.so: contest-alloc.c
	$(CC) $^ $(FLAGS) -o $@ -shared -fPIC -ldl

mreplace: mreplace.c
	$(CC) $^ $(FLAGS) -o $@

mcontest: mcontest.c
	$(CC) $^ $(FLAGS) -o $@ -ldl -lpthread

tester-agents: tester-1 tester-2 tester-3 tester-4 tester-5 tester-9

tester-1: testers/tester-1.c 
	$(CC) $^ $(FLAGS) -o $@

tester-2: testers/tester-2.c 
	$(CC) $^ $(FLAGS) -o $@

tester-3: testers/tester-3.c 
	$(CC) $^ $(FLAGS) -o $@

tester-4: testers/tester-4.c 
	$(CC) $^ $(FLAGS) -o $@

tester-5: testers/tester-5.c 
	$(CC) $^ $(FLAGS) -o $@

tester-9: testers/tester-9.c 
	$(CC) $^ $(FLAGS) -o $@
	
.PHONY : clean
clean:
	-rm -f *.o *.so mreplace mcontest tester-1 tester-2 tester-3 tester-4 tester-5 tester-9
	-rm -rf doc/html