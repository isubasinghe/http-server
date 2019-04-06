#ifndef HTTP_PARSER
#define HTTP_PARSER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#define SPACE 32
#define CR 13
#define LF 10


enum ReadState {RequestLine = 1, Headers = 2, Entity = 3, Error =  4};

typedef struct {
    char *Method;
    char *Path;
    char *Version;
    char *Body;
} HTTP_Request;

HTTP_Request *HTTP_ParseRequest(char *buffer, size_t len);

void HTTP_FreeRequest(HTTP_Request *request);

#endif // HTTP_PARSER