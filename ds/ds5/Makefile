#
# CS 241
# The University of Illinois
#

CC = gcc
INC = -I.
FLAGS = -Wall -Wextra -Werror -Wno-unused -lpthread -pthread -lm -g

TARGETS = $(patsubst %.c, %, $(wildcard *.c))

all: $(TARGETS)

%: %.c
	$(CC) $(INC) $(FLAGS) $^ -o $@

clean:
	-rm -f $(TARGETS)
