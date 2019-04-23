#include <stdio.h>
#include <stdlib.h>



#include "http_parser.h"
#include "http_writer.h"
#include "http_server.h"

#include "game.h"



// Our Model
static GM_GamePool *game;

void generateGameSession(char *session, size_t len) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int charset_len = sizeof(charset) -1;
    for(size_t i=0; i < len; i++) {
        session[i] = charset[rand()%charset_len];
    }
}


// Our Controller
void gameRouter(HTTP_Request *req, HTTP_Response *res) {
    if(req->Method == HTTP_GET) {
        if(req->HasQueries) {
            if(DT_HashTable_Has(req->Queries, "start")) {
                char session[SESSION_LEN];
                session[SESSION_LEN-1] = 0;
                generateGameSession(session, SESSION_LEN-1);
                if(GM_StartPlayer(game, session) == CAN_START) {
                    HTTP_SendHTMLFileCookie(res, "./public/3_first_turn.html", "sessionid", session);
                }else {
                    HTTP_SendHTMLFile(res, "./public/8_server_busy.html");
                }
                
                return;
            }
        }else {
            char path_keywords = !strcmp(req->Path, "/keywords");
            if(DT_HashTable_Has(req->Cookies, "user") && (!path_keywords)) {
                HTTP_SendHTMLFile(res, "./public/2_start.html");
            }else {
                if(path_keywords) {
                    char *keywords = GM_GetKeywordsJSON(game, DT_HashTable_Gets(req->Cookies, "sessionid", NULL));
                    HTTP_SendJSON(res, keywords);
                }else {
                    HTTP_SendHTMLFile(res, "./public/1_intro.html");
                }
            }
            
        }
    }else if(req->Method == HTTP_POST) {
        char *username = DT_HashTable_Gets(req->FormValues, "user", NULL);
        char *sessionid = DT_HashTable_Gets(req->Cookies, "sessionid", NULL);
        char *quit = DT_HashTable_Gets(req->FormValues, "quit", NULL);
        char *guess = DT_HashTable_Gets(req->FormValues, "guess", NULL);

        if(username) {
            HTTP_SendHTMLFileCookie(res, "./public/2_start.html", "user", username);
            return;
        }

        
        if(quit) {
            if(sessionid) {
                GM_QuitPlayer(game, sessionid);
            }
            HTTP_SendHTMLFile(res, "./public/7_gameover.html");
            return;
        }

        
        if(guess) {
            if(game->Ready) {
                if(sessionid) {
                    char *keyword = DT_HashTable_Gets(req->FormValues, "keyword", NULL);
                    if(GM_AddKeyword(game, sessionid, keyword) == KEYWORD_ADDED) {
                        char *keywords = GM_GetKeywordsJSON(game, sessionid);
                        free(keywords);
                        HTTP_SendHTMLFile(res, "./public/4_accepted.html");
                    }else if(GM_AddKeyword(game, sessionid, keyword) == KEYWORD_MATCH) {
                        HTTP_SendHTMLFileCookie(res, "./public/6_endgame.html", "round", "1");
                    }else {

                    }
                    
                }else {
                    //Should never ever get here, 
                    // to be presented with the option to guess, this session id 
                    // would be present because to guess the Start button was pressed
                    // which generates a sessionid
                    HTTP_Redirect(res, "/");
                }
            }else {
                if(game->Notify) {
                    game->Notify = NOTIFY_NONE;
                    HTTP_SendHTMLFileCookie(res, "./public/6_endgame.html", "round", "1");
                }else {
                    HTTP_SendHTMLFile(res, "./public/5_discarded.html");
                }
            }
        }

    }
    return;
}

int main(int argc, char *argv[]) {
    if(argc != 3) {
        printf("Usage: %s <server_ip> <server_port>\n", argv[0]);
        return 1;
    }

    game = GM_CreateGame();

    HTTP_Router *router = HTTP_CreateRouter();
    router->route = gameRouter;

    HTTP_Server *server = HTTP_CreateServer();
    // Sorta have MVC going here, kinda cool
    HTTP_SetRouter(server, router);

    if(HTTP_StartServer(server, argv[1], atoi(argv[2]))) {
        HTTP_FreeServer(server);
    }
    
    return 0;
}