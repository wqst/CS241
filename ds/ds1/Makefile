.PHONY: all clean doc 

BUILTINS = strlen strcmp strcpy strcat strchr strrchr strpbrk strsep
FLAGS    = $(addprefix -fno-builtin-, $(BUILTINS))
CFLAGS   = -g -std=gnu99 -Wall -Wextra -Werror -Wno-unused
TARGETS  = part1 part2 part1-std doc

ifneq (,$(wildcard part1-sol.c))
	TARGETS += part1-sol
endif

ifneq (,$(wildcard part2-sol.c))
	TARGETS += part2-sol
endif

all: $(TARGETS)

part1: part1-test.o part1.o part1-extra.o
	gcc $(FLAGS) -o part1 part1-test.o part1.o part1-extra.o

part1-std: part1-test.o
	gcc -o part1-std part1-test.o

part1-sol: part1-test.o part1-sol.o
	gcc $(FLAGS) -o part1-sol part1-test.o part1-sol.o

part2: part2.o part2-test.o
	gcc -g -o part2 part2.o part2-test.o

part2-sol: part2-sol.o part2-test-sol.o
	gcc -o part2-sol part2-sol.o part2-test-sol.o

part1-extra.o: part1-extra.c
	gcc $(FLAGS) $(CFLAGS) -c part1-extra.c

part1-test.o: part1-test.c
	gcc $(FLAGS) $(CFLAGS) -c part1-test.c

part1.o: part1.c
	gcc $(FLAGS) $(CFLAGS) -c part1.c

part1-sol.o: part1-sol.c
	gcc $(FLAGS) $(CFLAGS) -c part1-sol.c

part2.o: part2.h part2.c
	gcc $(CFLAGS) -c part2.c

part2-sol.o: part2-sol.h part2-sol.c
	gcc $(CFLAGS) -c part2-sol.c

part2-test.o: part2-test.c
	gcc $(CFLAGS) -c part2-test.c

part2-test-sol.o: part2-test.c
	gcc $(CFLAGS) -DSOL -o part2-test-sol.o -c part2-test.c

doc: part1-test.c part1.c part1-extra.c doc/Doxyfile doc/main.dox doc/ds1.html
	doxygen doc/Doxyfile

clean:
	rm -f *.o part1 part1-std part1-sol part2 part2-sol
	rm -rf doc/html

