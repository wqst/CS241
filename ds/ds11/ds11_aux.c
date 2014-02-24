/* 
 * CS 241
 * The University of Illinois
 */
 
/** @file ds11_aux.c */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "ds11_aux.h"

/**
 * @brief Perform a complete I/O operation on a file descriptor
 * 
 * @param fd the file descriptor we are performing the I/O operation on
 * @param buf a buffer used to hold the data that are required for the I/O
 * @param len amount of data to transfer in the I/O operation
 * @param op I/O operation to perform
 *
 * @returns 0 if the whole I/O operation has been completed; A negative number in case of error
 */
int _do_all(int fd, char * buf, int len, 
	    ssize_t (*op)(int fd, void * buf, size_t len)) {
	int todo = len;
	while(todo){
		int bytes = op(fd, buf + len - todo, todo);
		if(bytes <= 0) break;
		todo -= bytes;
	}
	return -todo;
}

/**
 * @brief Perform a complete write operation. See _do_all for the
 * explaination of the parameters
 */
int write_all(int fd, char * buffer, int len) {
	return _do_all(fd, buffer, len, (ssize_t (*)(int fd, void * buf, size_t len)) write);
}

/**
 * @brief Perform a complete read operation. See _do_all for the
 * explaination of the parameters
 */
int read_all(int fd, char * buffer, int len) {
	return _do_all(fd, buffer, len, read);
}

/**
 * @brief Perform a read operation on a file descriptor until a
 * newline character ('\n') is detected. The message is stored into
 * buffer. The memory required to store the data is allocated if
 * needed. The variable bufsize is updated accordingly.
 *
 * @param fd file descriptor on which the operation is being performed
 * 
 * @param buffer pointer to a buffer that will store the data. If a
 * realloc operation is performed, this variable is updated with the
 * new position of the buffer
 *
 * @param bufsize pointer to the size of the output buffer. If a
 * realloc operation is performed, this variable is updated with the
 * new size of the buffer
 *
 * @return the length of the retrieved message, excluding the trailing
 * newline character
 */
int read_io_to_nl(int fd, char ** buffer, int * bufsize) {
	int bread = 0;
	int size = (*bufsize > 1)?(*bufsize-1):(INIT_BUF);
	char * end = NULL;
	if(size +1 > *bufsize) 
		*buffer = realloc(*buffer, size + 1);
	do {
		if(size - bread == 0)
			*buffer = realloc(*buffer, (size <<= 1) + 1);
		int bytes = read(fd, *buffer + bread, size - bread);
		if(bytes <= 0) {
			goto out;
		}
		bread += bytes;
		(*buffer)[bread] = 0;
	} while(!(end = strchr(*buffer, '\n')));

out:
	*bufsize = size;
	return (end - *buffer);
}
