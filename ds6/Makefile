.PHONY: all clean sol std debug doc

BUILTINS = strtok strtok_r
FLAGS    = $(addprefix -fno-builtin-, $(BUILTINS))
FLAGS   += -std=c99 -Wall -Wextra -Werror -Wno-unused -pthread

all: ds6.o debug std doc
	gcc $(FLAGS) -o ds6 test.o ds6.o

sol: ds6_sol

debug: test.c ds6.c
	gcc -g $(FLAGS) -o ds6_debug test.c ds6.c

ds6_sol: test.o ds6_sol.o
	gcc -O3 $(FLAGS) -o ds6_sol test.o ds6_sol.o

std: test.o
	gcc -O3 -std=c99 -pthread -Wall -Wextra -Werror -Wno-unused -O3 -o ds6_std test.o

test.o: test.c
	gcc -O3 -c $(FLAGS) test.c

ds6.o: ds6.c
	gcc -O3 -c $(FLAGS) ds6.c

ds6_sol.o: ds6_sol.c
	gcc -O3 -c $(FLAGS) ds6_sol.c

#doc: test.c ds6.c doc/Doxyfile doc/main.dox doc/ds6.html
#	doxygen doc/Doxyfile

clean:
	rm -f *.o ds6 ds6_sol ds6_std ds6_debug 
	rm -rf ds6_debug.dSYM
#	rm -rf doc/html

