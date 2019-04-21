#include "http_router.h"

static void default_route(HTTP_Request *req, HTTP_Response *res) {
    if(!strcmp(req->Path, "/")) {
        if(!strcmp(req->Method, "POST")) {
            HTTP_Redirect(res, "/start");
        }else {
            HTTP_SendHTMLFile(res, "./public/1_intro.html");
        }
    }else {
        HTTP_SendHTMLFile(res, "./public/2_start.html");
    }
}


HTTP_Router *HTTP_CreateRouter() {
    HTTP_Router *router = malloc(sizeof(HTTP_Router));
    if(router == NULL) {
        return NULL;
    }
    router->route = default_route;
    return router;
}