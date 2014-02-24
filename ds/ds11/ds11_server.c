#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "ds11_aux.h"

/* Server code - This server is able to handle just one connection at
   the time */

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define WELCOME "Welcome to the CS241 Reverse Echo Service\n"
#define PROT_CHOOSE "Please, choose your message format\n"

#define PROT_ASCII 1
#define PROT_BIN 2

int read_to_nl(int fd, char ** buffer, int * bufsize) {
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

void reverse_string(char * str, int len){
	char * start = str, * end = start + len -1;
	while(start < end){
		char tmp = *start;
		*start = *end;
		*end = tmp;
		++start;
		--end;
	}
}

int handle_ascii(int fd, char ** buf, int * buf_size)
{
	int len = read_to_nl(fd, buf, buf_size);
	if( len < 0 ) return len;
	
	reverse_string(*buf, len);
	len = write_all(fd, *buf, len +1);
	
	return len;
}

int handle_bin(int fd, char ** buf, int * buf_size)
{
	int len;
	int ret;
	int bread = read_all(fd, (char *)&len, sizeof(int));
	if(bread < 0) return bread;
	if(*buf_size < len) {
		*buf = realloc(*buf, len);
		*buf_size = len;
	}
	bread = read_all(fd, *buf, len);
	reverse_string(*buf, len);
	ret = write_all(fd, (char *)&len, sizeof(int));
	if(ret < 0) return ret;
	ret = write_all(fd, *buf, len);
	return ret;
}

int main(int argc, char ** args)
{

	int port;
	int ssock, csock;
	struct sockaddr_in params;
	struct sockaddr client;
	int client_len;

	char * mesg = NULL;
	int buf_size = 0;
	
	if(argc < 2){
		fprintf(stderr, "Usage: %s <port_num>\n");
	}

	port = strtol(args[1], NULL, 10);
	ssock = socket(AF_INET, SOCK_STREAM, 0);

	params.sin_family = AF_INET;
	params.sin_port = htons(port);
	params.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if(bind(ssock, (struct sockaddr *)&params, sizeof(struct sockaddr_in)) < 0) {
		perror("Unable to bind\n");
		return 1;
	}
	
	if(listen(ssock, 1) < 0) {
		perror("Unable to listen\n");
		return 1;
	}
	
	/* Each loop is a different connection */
	while(1) {
		int mesg_len;
		int service;
		int active;
		csock = accept(ssock, &client, &client_len);
		if(csock < 0) continue;
		if(write_all(csock, WELCOME, strlen(WELCOME)) < 0 ||
		   write_all(csock, PROT_CHOOSE, strlen(PROT_CHOOSE)) < 0) 
		{
			close(csock);
			continue;
		}
		
		if( (mesg_len = read_to_nl(csock, &mesg, &buf_size)) <= 0) {
			close(csock);
			continue;
		}
		
		/* Provide service with selected format */
		if(!strncmp(mesg, "ASCII", MIN(5, mesg_len)))
			service = PROT_ASCII;
		else service = PROT_BIN;
		active = 1;
		
		while(active) {
			switch (service) {
			case PROT_ASCII :
				if(handle_ascii(csock, &mesg, &buf_size) < 0)
					active = 0;
				break;
			case PROT_BIN :
				if(handle_bin(csock, &mesg, &buf_size) < 0)
					active = 0;
				break;
			}
		}
	}
}
