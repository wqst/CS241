/* 
 * CS 241
 * The University of Illinois
 */
 
/** @file ds11_aux.h */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#ifndef _DS11_AUX_
#define _DS11_AUX_

#define INIT_BUF 100

/**
 * @brief Perform a complete I/O operation on a file descriptor
 * 
 * @param fd the file descriptor we are performing the I/O operation on
 * @param buf a buffer used to hold the data that are required for the I/O
 * @param len amount of data to transfer in the I/O operation
 * @param op I/O operation to perform
 *
 * @returns 0 if the whole I/O operation has been completed; A
 * negative number in case of error
 */
int _do_all(int fd, char * buf, int len, 
	    ssize_t (*op)(int fd, void * buf, size_t len));

/**
 * @brief Perform a complete write operation. See _do_all for the
 * explaination of the parameters
 */
int write_all(int fd, char * buffer, int len);

/**
 * @brief Perform a complete read operation. See _do_all for the
 * explaination of the parameters
 */
int read_all(int fd, char * buffer, int len);

#endif
