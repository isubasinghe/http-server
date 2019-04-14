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


typedef struct {
    char **HeaderKeys;
    char **HeaderValues;
    size_t Num;
} HTTP_Headers;

typedef struct {
    char **QueryKeys;
    char **QueryValues;
    size_t Num;
} HTTP_Queries;

typedef struct {
    char *Method;
    char *Path;
    char *Version;
    HTTP_Headers *Headers;
    char *Body;
} HTTP_Request;

HTTP_Request *HTTP_ParseRequest(char *buffer, size_t len);

HTTP_Request *HTTP_ParseRequestLine(HTTP_Request *request, char *buffer, size_t len, char **next);

char HTTP_HeadersPresent(char *buffer);

void HTTP_ParseURLQueries(HTTP_Request *request, char *path);

void HTTP_FreeRequest(HTTP_Request *request);

#endif // HTTP_PARSER