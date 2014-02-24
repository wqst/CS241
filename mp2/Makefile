#
# CS 241
# The University of Illinois
#

CC = gcc
INC = -I.
FLAGS += -O3 -Wextra -Wall -Werror
FLAGS += -Wno-unused
TESTERS = $(patsubst %.c, %, $(wildcard testers/*))

WARNING =	\e[1;31mWARNING: \e[1;33mYou are building MP2 using an unofficial set of configuration\
\nflags designed to work on linux builds not identical to the EWS machines.\
\nPassing testers, or even having code that compiles under these flags\
\nshould not be taken as an indication of correctness.  You \e[1;31m_MUST_\e[1;33m test\
\nyour code on the EWS linux machines to ensure correctness, no credit\
\nwill be granted for code that works with these flags but fails to\
\nbuild or run correctly on EWS.\n\e[0m

.PHONY: all
all: alloc.so contest-alloc.so mreplace mcontest tester-agents doc/html

.PHONY: unofficial
unofficial: | unoffical_updateFlags all
	@printf "$(WARNING)"
	
.PHONY: unofficial_updateFlags
unoffical_updateFlags:
	$(eval FLAGS = $(subst -Wno-unused,,$(subst -Werror,,$(FLAGS))))

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

tester-agents: part1 part2

part1: part1/tester-1 part1/tester-2 part1/tester-3 part1/tester-4

part2: $(subst testers, part2, $(TESTERS))

part1/%: testers/%.c
	$(CC) $^ $(FLAGS) -o $@

part2/%: testers/%.c
	$(CC) $^ $(FLAGS) -DPART2 -o $@
	
.PHONY : clean
clean:
	-rm -f *.o *.so mreplace mcontest part1/* part2/*
	-rm -rf doc/html
