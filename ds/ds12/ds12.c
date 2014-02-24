#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>

/*Aux functions*/

typedef struct __HTTPResponse
{
    void *vptrResponse;
    size_t length;
} HTTPResponse;

HTTPResponse getResponseString(char *sContentType, void *vptrContent, size_t iContentLength)
{
    HTTPResponse objHTTPResponse;
    objHTTPResponse.vptrResponse = malloc(iContentLength + 400);
    
    sprintf(objHTTPResponse.vptrResponse, "HTTP/1.1 200 OK\r\nServer: CS/241\r\nMIME-version: 1.0\r\nContent-type: %s\r\nContent-Length: %d\r\n\r\n", sContentType, (int)iContentLength);
    objHTTPResponse.length = strlen((char*)objHTTPResponse.vptrResponse) + iContentLength;
    memcpy(objHTTPResponse.vptrResponse + strlen((char*)objHTTPResponse.vptrResponse), vptrContent, iContentLength);
    
    return objHTTPResponse;
}

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s [port number]\n", argv[0]);
		return 1;
	}

	int port = atoi(argv[1]);
	if (port <= 0 || port >= 65536)
	{
		fprintf(stderr, "Illegal port number.\n");
		return 1;
	}

    /*YOUR IMPLEMENTATION GOES HERE*/
    
	return 0;
}
