all: parmake doc/html

doc/html: parmake.c
	doxygen doc/Doxyfile 2> /dev/null
	cp doc/flow.png doc/html/

parmake: parmake.o queue.o parser.o rule.o
	gcc -pthread -Wall parmake.o queue.o parser.o rule.o -o parmake

parmake.o: parmake.c
	gcc -g -Wall -c parmake.c -o parmake.o

queue.o: queue.c queue.h
	gcc -g -Wall -c queue.c -o queue.o

parser.o: parser.c parser.h
	gcc -g -Wall -c parser.c -o parser.o

rule.o: rule.c rule.h
	gcc -g -Wall -c rule.c -o rule.o

clean:
	rm -rf *.o parmake doc/html
