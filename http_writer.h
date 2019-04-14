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
void HTTP_SetHeader(HTTP_Response *response, char *headerKey, char *headerValue);
void HTTP_SetCookie(HTTP_Response *response, char *cookieKey, char *cookieValue);

char *HTTP_GetStatusPhrase(int status);


#endif // HTTP_WRITER_H