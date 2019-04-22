#include <stdio.h>
#include <stdlib.h>



#include "http_parser.h"
#include "http_writer.h"
#include "http_server.h"

#define SESSION_LEN 129 // 128 + 1


void generateGameSession(char *session, size_t len) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int charset_len = sizeof(charset) -1;
    for(size_t i=0; i < len; i++) {
        session[i] = charset[rand()%charset_len];
    }
}

void gameRouter(HTTP_Request *req, HTTP_Response *res) {
    if(req->Method == HTTP_GET) {
        if(req->HasQueries) {
            if(DT_HashTable_Has(req->Queries, "start")) {
                // Notify game that player has quite (starting a new game means exiting old one)
                if(DT_HashTable_Has(req->Cookies, "sessionid")) {

                }
                char session[SESSION_LEN];
                session[SESSION_LEN-1] = 0;
                generateGameSession(session, SESSION_LEN-1);
                HTTP_SendHTMLFileCookie(res, "./public/3_first_turn.html", "sessionid", session);
                return;
            }
        }else {
            if(DT_HashTable_Has(req->Cookies, "user")) {
                HTTP_SendHTMLFile(res, "./public/2_start.html");
            }else {
                HTTP_SendHTMLFile(res, "./public/1_intro.html");
            }
            
        }
    }else if(req->Method == HTTP_POST) {
        char *username = DT_HashTable_Gets(req->FormValues, "user", NULL);
        if(username) {
            HTTP_SendHTMLFileCookie(res, "./public/2_start.html", "user", username);
            return;
        }

        char *quit = DT_HashTable_Gets(req->FormValues, "quit", NULL);
        if(quit) {
            HTTP_SendHTMLFile(res, "./public/7_gameover.html");
            return;
        }

        char *guess = DT_HashTable_Gets(req->FormValues, "guess", NULL);
        if(guess) {
            printf("GUESSED: <%s>\n", guess);
            HTTP_SendHTMLFile(res, "./public/4_accepted.html");
        }
    }
    return;
}

int main(int argc, char *argv[]) {

    HTTP_Router *router = HTTP_CreateRouter();
    router->route = gameRouter;

    HTTP_Server *server = HTTP_CreateServer();
    HTTP_SetRouter(server, router);

    if(HTTP_StartServer(server, "127.0.0.1", 8080)) {
        HTTP_FreeServer(server);
    }
    
    return 0;
}