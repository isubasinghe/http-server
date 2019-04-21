#ifndef HTTP_WRITER_H
#define HTTP_WRITER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/sendfile.h>

#include "http_status.h"

#define HTTP_SERVER_PROTO "HTTP/1.0 "
#define HTTP_STATUS_LEN 4


#define HTTP_200_RESPONSE "HTTP/1.0 200 OK\r\n" \
"Content-Type: text/html; charset=utf-8\r\n" \
"Content-Length: %ld\r\n\r\n"

#define HTTP_200_COOKIE_RESPONSE "HTTP/1.0 200 OK\r\n" \
"Set-Cookie: %s=%s\r\n" \
"Content-Type: text/html; charset=utf-8\r\n" \
"Content-Length: %ld\r\n\r\n"

#define HTTP_301_REDIRECT "HTTP/1.0 301 Moved Permanently\r\n" \
"Location: %s\r\n" \
"Connection-Type: keep-alive\r\n" \
"Content-Length: 0\r\n\r\n" \

#define HTTP_301_COOKIE_REDIRECT "HTTP/1.0 301 Moved Permanently\r\n" \
"Set-Cookie: %s=%s\r\n" \
"Location: %s\r\n" \
"Connection-Type: keep-alive\r\n" \
"Content-Length: 0\r\n\r\n"

typedef struct {
    char *Protocol;
    size_t ProtocolLen;
    char *StatusCode;
    size_t CodeLen;
    char *Phrase;
    size_t PhraseLen;
    char **HeaderKeys;
    char **HeaderValues;
    size_t NumHeaders;
    char *Body;
    int __sock;
} HTTP_Response;


HTTP_Response *HTTP_CreateResponse();

void HTTP_SetStatus(int status);
void HTTP_SetHeader(HTTP_Response *response, char *headerKey, char *headerValue);
void HTTP_SetCookie(HTTP_Response *response, char *cookieKey, char *cookieValue);

void HTTP_SendHTMLFile(HTTP_Response *response, char *fname);
void HTTP_SendHTMLFileCookie(HTTP_Response *response, char *fname, char *key, char *value);

void HTTP_Redirect(HTTP_Response *response, char *path);
void HTTP_RedirectCookie(HTTP_Response *response, char *path, char *key, char *value);

void HTTP_EndResponse(HTTP_Response *response);

void HTTP_FreeResponse(HTTP_Response *response);

char *HTTP_GetStatusPhrase(int status);


#endif // HTTP_WRITER_H