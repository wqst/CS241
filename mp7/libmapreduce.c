/** @file libmapreduce.c */
/* 
 * CS 241
 * The University of Illinois
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>

#include "libmapreduce.h"
#include "libds/libds.h"


static const int BUFFER_SIZE = 2048;  /**< Size of the buffer used by read_from_fd(). */


/**
 * Adds the key-value pair to the mapreduce data structure.  This may
 * require a reduce() operation.
 *
 * @param key
 *    The key of the key-value pair.  The key has been malloc()'d by
 *    read_from_fd() and must be free()'d by you at some point.
 * @param value
 *    The value of the key-value pair.  The value has been malloc()'d
 *    by read_from_fd() and must be free()'d by you at some point.
 * @param mr
 *    The pass-through mapreduce data structure (from read_from_fd()).
 */
static void process_key_value(const char *key, const char *value, mapreduce_t *mr)
{
	//first we need to check if the key is already there.
	if(datastore_put(mr->ds, key, value) == 0) {
		//this key already exists and we need to update the existing one
		char success = 0;
		while(!success) {
			unsigned long revision;
			const char* existing = datastore_get(mr->ds, key, &revision);
			const char* next = mr->reduce_fcn(existing, value);
			success = datastore_update(mr->ds, key, next, revision);
			free((char*)existing);
			if(next != value && next != existing)
				free((char*)next);
			
		}
	}
	//at this point we will have successfully stored the data, so we need to free the key and value
	free((char*)key);
	free((char*)value);
}


/**
 * Helper function.  Reads up to BUFFER_SIZE from a file descriptor into a
 * buffer and calls process_key_value() when for each and every key-value
 * pair that is read from the file descriptor.
 *
 * Each key-value must be in a "Key: Value" format, identical to MP1, and
 * each pair must be terminated by a newline ('\n').
 *
 * Each unique file descriptor must have a unique buffer and the buffer
 * must be of size (BUFFER_SIZE + 1).  Therefore, if you have two
 * unique file descriptors, you must have two buffers that each have
 * been malloc()'d to size (BUFFER_SIZE + 1).
 *
 * Note that read_from_fd() makes a read() call and will block if the
 * fd does not have data ready to be read.  This function is complete
 * and does not need to be modified as part of this MP.
 *
 * @param fd
 *    File descriptor to read from.
 * @param buffer
 *    A unique buffer associated with the fd.  This buffer may have
 *    a partial key-value pair between calls to read_from_fd() and
 *    must not be modified outside the context of read_from_fd().
 * @param mr
 *    Pass-through mapreduce_t structure (to process_key_value()).
 *
 * @retval 1
 *    Data was available and was read successfully.
 * @retval 0
 *    The file descriptor fd has been closed, no more data to read.
 * @retval -1
 *    The call to read() produced an error.
 */
static int read_from_fd(int fd, char *buffer, mapreduce_t *mr)
{
	/* Find the end of the string. */
	int offset = strlen(buffer);

	/* Read bytes from the underlying stream. */
	int bytes_read = read(fd, buffer + offset, BUFFER_SIZE - offset);
	if (bytes_read == 0)
		return 0;
	else if(bytes_read < 0)
	{
		fprintf(stderr, "error \"%s\" in read.\n", strerror(errno));
		return -1;
	}

	buffer[offset + bytes_read] = '\0';

	/* Loop through each "key: value\n" line from the fd. */
	char *line;
	while ((line = strstr(buffer, "\n")) != NULL)
	{
		*line = '\0';

		/* Find the key/value split. */
		char *split = strstr(buffer, ": ");
		if (split == NULL)
			continue;

		/* Allocate and assign memory */
		char *key = malloc((split - buffer + 1) * sizeof(char));
		char *value = malloc((strlen(split) - 2 + 1) * sizeof(char));

		//printf("read %s, %s\n", key, value);

		strncpy(key, buffer, split - buffer);
		key[split - buffer] = '\0';

		strcpy(value, split + 2);

		/* Process the key/value. */
		process_key_value(key, value, mr);

		/* Shift the contents of the buffer to remove the space used by the processed line. */
		memmove(buffer, line + 1, BUFFER_SIZE - ((line + 1) - buffer));
		buffer[BUFFER_SIZE - ((line + 1) - buffer)] = '\0';
	}

	return 1;
}


/**
 * Initialize the mapreduce data structure, given a map and a reduce
 * function pointer.
 */
void mapreduce_init(mapreduce_t *mr, 
                    void (*mymap)(int, const char *), 
                    const char *(*myreduce)(const char *, const char *))
{
	mr->map_fcn = mymap;
	mr->reduce_fcn = myreduce;
	mr->ds = malloc(sizeof(datastore_t));
	datastore_init(mr->ds);
}

void* monitor(void* data) {
	int efd = ((monitor_data_t*)data)->efd;
	int open_pipes = ((monitor_data_t*)data)->num_pieces;
	mapreduce_t* mr = ((monitor_data_t*)data)->mr;

	int initial_pipes = open_pipes;

	//make buffers
	char* buffers[open_pipes];
	int buffer_index[open_pipes];
	int assigned_buffers = 0;
	int i;
	for(i = 0; i < open_pipes; i++) {
		buffers[i] = malloc(BUFFER_SIZE + 1);
		bzero(buffers[i], BUFFER_SIZE + 1);
		buffers[i][0] = '\0';	//read_from_fd expects buffer[0] to be \0
		buffer_index[i] = -1;
	}
	while(open_pipes) {	//this loop should continue until all map() functions are complete and there are no fds ready
		struct epoll_event ev;
		bzero(&ev.data, sizeof(ev.data));
		//wait for an available pipe
		
		epoll_wait(efd, &ev, 1, -1);
		//look for this fd's buffer
		int assigned = -1;
		for(i = 0; i < assigned_buffers; i++)
			if(buffer_index[i] == ev.data.fd) {
				assigned = i;
				break;
			}
		//if they're all taken, reserve the first open one
		if(assigned == -1) {
			buffer_index[assigned_buffers] = ev.data.fd;
			assigned = assigned_buffers;
			assigned_buffers++;
		}

		//read from the buffer and assign accordingly
		if(read_from_fd(ev.data.fd, buffers[assigned], mr) == 0) {
			epoll_ctl(efd, EPOLL_CTL_DEL, ev.data.fd, NULL);
			open_pipes--;
		}
	}
	for(i = 0; i < initial_pipes; i++)
		free(buffers[i]);
	free(data);
	return NULL;
}

/**
 * Starts the map() processes for each value in the values array.
 * (See the MP description for full details.)
 */
void mapreduce_map_all(mapreduce_t *mr, const char **values)
{
	int inputs = 0;
	int i;
	for(i = 0; values[i] != NULL; i++) {
		inputs++;
	}
	int fd[inputs][2];
	int* remaining = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	*remaining = inputs;

	int efd = epoll_create(inputs);
	struct epoll_event ev[inputs];

	for(i = 0; i < inputs; i++) {
		pipe(fd[i]);
		int read_fd = fd[i][0];
		int write_fd = fd[i][1];
		pid_t pid = fork();
		if(pid == 0) {
			close(read_fd);
			mr->map_fcn(write_fd, values[i]);
			*remaining = *remaining - 1;
			close(write_fd);
			//can't leak memory from here!
			mapreduce_destroy(mr);
			exit(0);
		}
		else {
			close(write_fd);
		}
		bzero(&ev[i].data, sizeof(ev[i].data));
		ev[i].events = EPOLLIN;
		ev[i].data.fd = read_fd;
		epoll_ctl(efd, EPOLL_CTL_ADD, read_fd, &ev[i]);
	}
	monitor_data_t* data = malloc(sizeof(monitor_data_t));
	//add things to data here
	data->efd = efd;
	data->num_pieces = inputs;
	data->mr = mr;
	pthread_create(&mr->monitor, NULL, monitor, data);
}

/**
 * Blocks until all the reduce() operations have been completed.
 * (See the MP description for full details.)
 */
void mapreduce_reduce_all(mapreduce_t *mr)
{
	pthread_join(mr->monitor, NULL);
}


/**
 * Gets the current value for a key.
 * (See the MP description for full details.)
 */
const char *mapreduce_get_value(mapreduce_t *mr, const char *result_key)
{
	return datastore_get(mr->ds, result_key, NULL);
}


/**
 * Destroys the mapreduce data structure.
 */
void mapreduce_destroy(mapreduce_t *mr)
{
	datastore_destroy(mr->ds);
	free(mr->ds);
}
