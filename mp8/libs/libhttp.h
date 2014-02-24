#ifndef _LIBHTTP_H_
#define _LIBHTTP_H_

#include "libdictionary.h"

typedef struct 
{
	
	char * status;
	char * body;
	long len;
	dictionary_t header;
	
} http_t;


int http_read(http_t *http, int fd);

const char *http_get_body(http_t *http, size_t *length);
const char *http_get_header(http_t *http, char *key);
const char *http_get_status(http_t *http);

void http_free(http_t *http);


#endif
