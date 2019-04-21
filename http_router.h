#ifndef HTTP_ROUTER_H
#define HTTP_ROUTER_H

#include "http_parser.h"
#include "http_writer.h"

typedef struct {
    void (*route)(HTTP_Request *req, HTTP_Response *res);
} HTTP_Router;

HTTP_Router *HTTP_CreateRouter();

void HTTP_FreeRouter(HTTP_Router *router);


#endif // HTTP_ROUTER_H