#ifndef HTTP_WRITER_H
#define HTTP_WRITER_H

#include <stdio.h>
#include <stdlib.h>
#include "http_status.h"

#define HTTP_SERVER_PROTO "HTTP/1.0"



typedef struct {
    char *Protocol;
    char *Data;
} HTTP_Response;


void HTTP_SetStatus(int status);
void HTTP_SetHeader(HTTP_Response *response, char *HeaderKey, char *HeaderValue);
void HTTP_SetCookie(HTTP_Response *response, char *CookieKey, char *CookieValue);

char *HTTP_GetStatusPhrase(int status);


#endif // HTTP_WRITER_H