#ifndef HTTP_PARSER
#define HTTP_PARSER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "dt_hashtable.h"

#define SPACE 32
#define CR 13
#define LF 10
#define QUERY_INDICATOR "?"
#define QUERY_SEP "="
#define QUERY_AND "&"
#define QUERY_MAX 128
#define HEADERS_MAX 128
#define COOKIES_MAX 128
#define FORM_MAX 128
#define EMPTY_BODY ""
#define COOKIE_STR "Cookie"
#define CLEN_HEADER_KEY "Content-Length"
#define CONTENT_TYPE "Content-Type"
#define FORM_TYPE "application/x-www-form-urlencoded"


typedef struct {
    char *Method;
    char *Path;
    char *Version;
    char *Body;
    char HasQueries;
    DT_HashTable *Queries;
    DT_HashTable *Headers;
    DT_HashTable *Cookies;
    DT_HashTable *FormValues;
} HTTP_Request;

HTTP_Request *HTTP_ParseRequest(char *buffer, size_t len);

void HTTP_FreeRequest(HTTP_Request *request);

#endif // HTTP_PARSER