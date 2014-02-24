CC=gcc
CFLAGS=-lpthread

all: dphil

dphil: dphil.c dphil_skeleton.c
	$(CC) $^ -o $@ $(CFLAGS)

clean:
	rm -f dphil
