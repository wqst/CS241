/* 
 * CS 241
 * The University of Illinois
 */
 
/** @file libhttp.c*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include "libhttp.h"

#define MAX_SIZE 4096

/* Allows up to 10 MB requests */
#define MAX_BODY_LEN (10 * 1024 * 1024) 

static const int INITIAL_BUFFER_SIZE = 1024;

/** 
 *   Reads an HTTP request from the file descriptor fd and parses it
 *   filling the http_t structure with: request status; request
 *   headers; request body (if any).
 *
 *   @param http an pointer to an http_t structure to be filled with
 *   the data of the HTTP request.
 *
 *   @param fd a file descriptor where the HTTP stream is read from.
 *
 *   @return the total length of the HTTP request in bytes or -1 if no
 *   HTTP request could be processed. This happens in case: the
 *   connection has been closed; the length of the request exceeds the
 *   limits or the data stream ends prematurely.
 */
int http_read(http_t *http, int fd)
{

	int size = INITIAL_BUFFER_SIZE;
	char * buf = (char *)malloc(size + 1);
	int bread = 0;
	char * body = NULL, * status;
	char * pair;
	long body_len = 0;
	const char * len_key;

	/* Init http fields */
	http->body = NULL;
	http->status = NULL;
	http->len = 0;
	dictionary_init(&http->header);

	/* Read until the end of the header */
	do{
		if(size - bread == 0)
			buf = realloc(buf, (size <<= 1) + 1); 
		int bytes = read(fd, buf + bread, size - bread);
		if(bytes <= 0) break;
		bread += bytes;
		buf[bread] = 0;
	} while(!(body = strstr(buf, "\r\n\r\n")) && size <= MAX_SIZE);

	if(!body){
		bread = -1;
		goto out;
	}

	*body = 0;
	body += 4;
	
	status = strstr(buf, "\r\n");
	http->status = strndup(buf, status - buf);

	for(pair = strtok(buf, "\r"); (pair = strtok(NULL, "\r")) != NULL; ){
		char * col;
		++pair;
		col = strchr(pair, ':');
		if(col){
			char * key, * value;
			*(col++) = 0;
			if(*col == ' ') ++col;
			if(dictionary_add(&http->header, 
				       (key = strdup(pair)),
				       (value = strdup(col))
				   ) == KEY_EXISTS)
			{
				free(key);
				free(value);
			}
		}
	}

	len_key = dictionary_get(&http->header, "Content-Length");
	if(!len_key)
		goto out;

	body_len = strtol(len_key, NULL, 10);

	/* Now read the body */
	while((bread - (body - buf)) < body_len && size <= MAX_BODY_LEN){
		if(size - bread == 0)
			buf = realloc(buf, (size <<= 1)); 
		int bytes = read(fd, buf + bread, size - bread);
		if(bytes <= 0) break;
		bread += bytes;
	} 

	if(bread < body_len){
		http_free(http);
		bread = -1;
		goto out;
	}

	http->len = body_len;
	http->body = strndup(body, body_len);

out:
	free(buf);

	return bread;
}

/**
 *   Returns the value of a HTTP header with the given key.
 *
 *   @param http a pointer to an http_t structure to retrieve the headers from.
 *
 *   @param key the searched key.
 *
 *   @return a null-terminated string containing the value for the
 *   given key, or NULL if the HTTP request did not contain the
 *   searched header.
 */
const char *http_get_header(http_t *http, char *key)
{
	return dictionary_get(&http->header, key);
}

/**
 *   Returns the status of a HTTP request, i.e. the fist line which
 *   terminates with "\r\n".
 *
 *   @param http a pointer to an http_t structure to retrieve the
 *   status from.
 *
 *   @return a null-terminated string containing the value of the HTTP
 *   status, or NULL if it was unable to retrieve the status of the
 *   HTTP request.
 */
const char *http_get_status(http_t *http)
{
	return http->status;
}

/**
 *   Returns the body of a HTTP request.
 *
 *   @param http a pointer to an http_t structure to retrieve the body
 *   from.
 *
 *   @param length if specified, *length gets filled with the total
 *   length in bytes of the HTTP body
 *
 *   @return a null-terminated string containing the value of the body
 *   of a HTTP request, or NULL if the HTTP request did not contain a
 *   body.
 */
const char *http_get_body(http_t *http, size_t *length)
{
	if(length) *length = http->len;
	return http->body;
}

/**
 *   Deallocates the resources used for a HTTP request.
 *
 *   @param http a pointer to an http_t structure to deallocate.
 */
void http_free(http_t *http)
{
	if(http->body) {
		free(http->body);
		http->body = NULL;
	}
	if(http->status) {
		free(http->status);
		http->status = NULL;
	}
	dictionary_destroy_all(&http->header);
}
