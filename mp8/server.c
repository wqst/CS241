/** @file server.c */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <queue.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "queue.h"
#include "libhttp.h"
#include "libdictionary.h"

//length 324
const char *HTTP_404_CONTENT = "<html><head><title>404 Not Found</title></head><body><h1>404 Not Found</h1>The requested resource could not be found but may be available again in the future.<div style=\"color: #eeeeee; font-size: 8pt;\">Actually, it probably won't ever be available unless this is showing up because of a bug in your program. :(</div></html>";
//length 205
const char *HTTP_501_CONTENT = "<html><head><title>501 Not Implemented</title></head><body><h1>501 Not Implemented</h1>The server either does not recognise the request method, or it lacks the ability to fulfill the request.</body></html>";

const char *HTTP_200_STRING = "OK";
const char *HTTP_404_STRING = "Not Found";
const char *HTTP_501_STRING = "Not Implemented";

//length 307
char *HTTP_501_RESPONSE_KEEPALIVE = "HTTP/1.1 501 Not Implemented\r\nContent-Type: text/html\r\nContent-Length: 205\r\nConnection: Keep-Alive\r\n\r\n<html><head><title>501 Not Implemented</title></head><body><h1>501 Not Implemented</h1>The server either does not recognise the request method, or it lacks the ability to fulfill the request.</body></html>";
//length 302
char *HTTP_501_RESPONSE_NOKEEPALIVE = "HTTP/1.1 501 Not Implemented\r\nContent-Type: text/html\r\nContent-Length: 205\r\nConnection: close\r\n\r\n<html><head><title>501 Not Implemented</title></head><body><h1>501 Not Implemented</h1>The server either does not recognise the request method, or it lacks the ability to fulfill the request.</body></html>";

//length 420 (lololololol)
char *HTTP_404_RESPONSE_KEEPALIVE = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: 324\r\nConnection: Keep-Alive\r\n\r\n<html><head><title>404 Not Found</title></head><body><h1>404 Not Found</h1>The requested resource could not be found but may be available again in the future.<div style=\"color: #eeeeee; font-size: 8pt;\">Actually, it probably won't ever be available unless this is showing up because of a bug in your program. :(</div></html>";
//length 415
char *HTTP_404_RESPONSE_NOKEEPALIVE = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: 324\r\nConnection: close\r\n\r\n<html><head><title>404 Not Found</title></head><body><h1>404 Not Found</h1>The requested resource could not be found but may be available again in the future.<div style=\"color: #eeeeee; font-size: 8pt;\">Actually, it probably won't ever be available unless this is showing up because of a bug in your program. :(</div></html>";

typedef struct _thread_data {
	int* fd;
	char* kill;
} thread_data;

queue_t* threads = NULL;
queue_t* sockets = NULL;

char* should_kill = NULL;

void signal_callback_handler(int signum) {
	printf("caught signal %d\n", signum);
	if(signum != 2) {
		printf("but who really gives a shit?\n");
		return;
	}
	queue_t* to_free = malloc(sizeof(queue_t));
	queue_init(to_free);
	printf("closing sockets\n");
	if(sockets != NULL) {
		while(queue_size(sockets) != 0) {
			int* curr = queue_dequeue(sockets);
			shutdown(*curr, SHUT_RDWR);
			queue_enqueue(to_free, curr);
		}
	}
	printf("joining threads\n");
	*should_kill = 1;
	if(threads != NULL) {
		while(queue_size(threads) != 0) {
			pthread_t* curr = queue_dequeue(threads);
			pthread_join(*curr, NULL);
			queue_enqueue(to_free, curr);
		}
	}
	//free memory here
	while(queue_size(to_free) != 0)
		free(queue_dequeue(to_free));

	free(should_kill);
	queue_destroy(threads);
	queue_destroy(sockets);
	queue_destroy(to_free);
	free(threads);
	free(sockets);
	free(to_free);
	exit(signum);
}

/**
 * Processes the request line of the HTTP header.
 * 
 * @param request The request line of the HTTP header.  This should be
 *                the first line of an HTTP request header and must
 *                NOT include the HTTP line terminator ("\r\n").
 *
 * @return The filename of the requested document or NULL if the
 *         request is not supported by the server.  If a filename
 *         is returned, the string must be free'd by a call to free().
 */
char* process_http_header_request(const char *request)
{
	// Ensure our request type is correct...
	if (strncmp(request, "GET ", 4) != 0)
		return NULL;

	// Ensure the function was called properly...
	assert( strstr(request, "\r") == NULL );
	assert( strstr(request, "\n") == NULL );

	// Find the length, minus "GET "(4) and " HTTP/1.1"(9)...
	int len = strlen(request) - 4 - 9;

	// Copy the filename portion to our new string...
	char *filename = malloc(len + 1);
	strncpy(filename, request + 4, len);
	filename[len] = '\0';

	// Prevent a directory attack...
	//  (You don't want someone to go to http://server:1234/../server.c to view your source code.)
	if (strstr(filename, ".."))
	{
		free(filename);
		return NULL;
	}

	return filename;
}

void* process(void* data) {
	thread_data* info = (thread_data*)data;
	//read the header with the handy dandy libhttp
	char keepalive = 1;
	while(keepalive && !*info->kill) {
		//add a 10 second timeout
		/*
		struct timeval tv;
		bzero(&tv, sizeof(struct timeval));
		tv.tv_sec = 10;
		setsockopt(*info->fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(struct timeval));
		*/

		http_t header;
		bzero(&header, sizeof(http_t));
		int bytes_read = http_read(&header, *info->fd);
		if(bytes_read == -1)
			continue;

		printf("=======data received, http_read returned %d\n", bytes_read);

		printf("received request, analyzing \"%s\"\n", http_get_status(&header));
		char* filename = process_http_header_request(http_get_status(&header));
		if(!strcasecmp(http_get_header(&header, "Connection"), "Keep-Alive"))
			keepalive = 1;
		else
			keepalive = 0;
		
		char* response;
		char free_response = 0;
		int response_length = 0;
		if(filename == NULL) {
			//501 error
			printf("filename invalid, triggering 501 error\n");
			if(keepalive) {
				response = HTTP_501_RESPONSE_KEEPALIVE;
				response_length = 307;
			}
			else {
				response = HTTP_501_RESPONSE_NOKEEPALIVE;
				response_length = 302;
			}
		} else {
			printf("client requested file \"%s\"\n", filename);
			if(!strcmp(filename, "/")) {
				//printf("caught \"/\"\n");
				filename = realloc(filename, (strlen("index.html") + 1) * sizeof(char));
				sprintf(filename, "index.html");
			}
			//assemble filename
			char* file_to_open = malloc(sizeof(char) * (strlen(filename) + strlen("web/") + 1));
			sprintf(file_to_open, "web/%s", filename);
			
			//printf("opening \"%s\"\n", file_to_open);

			FILE* file = fopen(file_to_open, "r");
			if(file == NULL) {
				//404 error
				printf("file not found, triggering 404 error\n");
				if(keepalive) {
					response = HTTP_404_RESPONSE_KEEPALIVE;
					response_length = 420;
				}
				else {
					response = HTTP_404_RESPONSE_NOKEEPALIVE;
					response_length = 415;
				}
			} else {
				//200 OK, go on to read the file now
				char* connection_type;
				if(keepalive)
					connection_type = "Keep-Alive";
				else
					connection_type = "close";
				//examine file extension
				char* file_extension = strrchr(file_to_open, '.');
				char* mime_type;
				if(!strcmp(file_extension, ".html"))
					mime_type = "text/html";
				else if(!strcmp(file_extension, ".css"))
					mime_type = "text/css";
				else if(!strcmp(file_extension, ".jpg"))
					mime_type = "image/jpeg";
				else if(!strcmp(file_extension, ".png"))
					mime_type = "image/png";
				else
					mime_type = "text/plain";

				//printf("file found, reading from disk\n");
				fseek(file, 0, SEEK_END);
				int length = ftell(file);
				fseek(file, 0, SEEK_SET);
				char buffer[length + 1];

				fread(buffer, length, 1, file);
				fclose(file);
				//printf("file as read is:\n%s\n----\n", buffer);
				//the header shouldn't be over 110 characters for our purposes
				char* response_header = malloc(sizeof(char) * 110);
				sprintf(response_header, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %d\r\nConnection: %s\r\n\r\n", mime_type, length, connection_type);
				response = malloc(sizeof(char) * (length + strlen(response_header)));
				strcpy(response, response_header);
				
				//now copy the data into the header
				char* start = response + sizeof(char) * strlen(response);
				memcpy(start, buffer, length);
				response_length = sizeof(char) * (length + strlen(response_header));
				free(response_header);
				free_response = 1;
			}
			free(file_to_open);
		}
		//printf("writing response:\n%s\n========\n", response);
		//now actually write the response
		send(*info->fd, response, response_length, 0);
		if(free_response)
			free(response);
		free(filename);
		http_free(&header);
	}

	//cleanup!
	close(*info->fd);
	free(data);
	return NULL;
}

int main(int argc, char **argv)
{
	char* port = "0";
	if(argc == 2) {
		port = argv[1];
	}
	else {
		printf("usage: ./server port\n");
		return 1;
	}

	should_kill = malloc(sizeof(char));
	*should_kill = 0;

	//register signal handler
	signal(SIGINT, signal_callback_handler);

	//create socket
	int listenfd = socket(AF_INET, SOCK_STREAM, 0);

	//initialize the naming struct
	struct addrinfo hints, *result;
	memset(&hints, 0x00, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	getaddrinfo(NULL, port, &hints, &result);

	bind(listenfd, result->ai_addr, result->ai_addrlen);
	free(result);

	//start with 10 simultaneous connections
	listen(listenfd, 10);
	struct sockaddr_in test;
	socklen_t len = sizeof(test);
	if(getsockname(listenfd, (struct sockaddr*)&test, &len) == -1)
		perror("getsockname\n");
	else
		printf("listening on port %d\n", ntohs(test.sin_port));

	//create a queue in which to store all the pthreads so we can correctly join them
	threads = malloc(sizeof(queue_t));
	sockets = malloc(sizeof(queue_t));
	queue_init(threads);
	queue_init(sockets);

	//main loop
	while(1) {
		int connfd = accept(listenfd, NULL, NULL);

		int* fd = malloc(sizeof(int));
		*fd = connfd;

		pthread_t* server_thread = malloc(sizeof(pthread_t));
		thread_data* data = malloc(sizeof(thread_data));
		data->fd = fd;
		data->kill = should_kill;
		pthread_create(server_thread, NULL, process, data);
		queue_enqueue(threads, server_thread);
		queue_enqueue(sockets, fd);
	}

	return 0;
}
