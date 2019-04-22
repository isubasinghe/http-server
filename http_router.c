#include "http_router.h"

static void defaultRoute(HTTP_Request *req, HTTP_Response *res) {
    char *user = DT_HashTable_Gets(req->Cookies, "user", NULL);
    printf("PATH: <%s>\n", req->Path);
    printf("METHOD: <%s>\n", req->MethodStr);
    printf("BODY: <%s>\n", req->Body);

    if(user != NULL) {
        printf("USER: <%s>\n", user);
    }

    if(!strcmp(req->Path, "/")) {
        if(req->Method == HTTP_GET) {
            HTTP_SendHTMLFile(res, "./public/1_intro.html");
        }else {
            char *user_form = DT_HashTable_Gets(req->FormValues, "user", NULL);
            if(user_form) {
                printf("USER_FORM: <%s>\n", user_form);
                HTTP_RedirectCookie(res, "/start", "user", user_form);
            }else {
                printf("ERROR USER FORM NOT PARSED\n");
            }
            
        }
        
    }else if(!strcmp(req->Path, "/start")) {
        if(req->Method == HTTP_GET) {
            if(req->HasQueries) {
                HTTP_Redirect(res, "/firstturn");
            }else {
                HTTP_SendHTMLFile(res, "./public/2_start.html");
            }
        }
    }else if(!strcmp(req->Path, "/firstturn")) {
        HTTP_SendHTMLFile(res, "./public/3_first_turn.html");
    }

    printf("**********\n");


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