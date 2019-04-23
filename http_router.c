#include "http_router.h"

static void defaultRoute(HTTP_Request *req, HTTP_Response *res) {
    HTTP_SendHTMLFile(res, "./public/1_intro.html");
}


HTTP_Router *HTTP_CreateRouter() {
    HTTP_Router *router = malloc(sizeof(HTTP_Router));
    if(router == NULL) {
        return NULL;
    }
    router->route = defaultRoute;
    return router;
}


void HTTP_FreeRouter(HTTP_Router *router) {
    free(router);
}