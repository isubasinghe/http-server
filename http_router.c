#include "http_router.h"

static void defaultRoute(HTTP_Request *req, HTTP_Response *res) {
    if(!strcmp(req->Path, "/")) {
        if(!strcmp(req->Method, "POST")) {
            char *user = DT_HashTable_Gets(req->FormValues, "user", NULL);
            if(user == NULL) {
                HTTP_Redirect(res, "/");
            }else {
                HTTP_RedirectCookie(res, "/start", "user", user);
            }
            
        }else {
            HTTP_SendHTMLFile(res, "./public/1_intro.html");
        }
    }else {
        if(DT_HashTable_Has(req->Cookies, "user")) {
            if(!strcmp(req->Path, "/start")) {
                if(!strcmp(req->Method, "GET")) {
                    if(req->HasQueries) {
                        HTTP_Redirect(res, "/first");
                    }else {
                        HTTP_SendHTMLFile(res, "./public/2_start.html");
                    }
                }else {
                    HTTP_Redirect(res, "/gameover");
                }
            }else if(!strcmp(req->Path, "/first")) {
                if(!strcmp(req->Method, "GET")) {
                    HTTP_SendHTMLFile(res, "./public/4")
                }
            }else if(!strcmp(req->Path, "/gameover")) {
                HTTP_SendHTMLFile(res, "./public/7_gameover.html");
            }
        }else {
            HTTP_Redirect(res, "/");
        }
    }
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