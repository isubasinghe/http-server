#ifndef HTTP_ROUTER_H
#define HTTP_ROUTER_H

typedef struct {
    void (*route)();
} HTTP_Router;

HTTP_Router *HTTP_CreateRouter();



#endif // HTTP_ROUTER_H