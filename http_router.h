#ifndef HTTP_ROUTER_H
#define HTTP_ROUTER_H

typedef struct {
    void (*route)();
} HTTP_Router;


static void default_route() {
    
}

#endif // HTTP_ROUTER_H