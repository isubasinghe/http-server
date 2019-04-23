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
                // Generate a session id for a game "set" where a "set" of games has 2 rounds
                generateGameSession(session, SESSION_LEN-1);
                if(GM_StartPlayer(game, session) == CAN_START) {
                    HTTP_SendHTMLFileCookie(res, "./public/3_first_turn.html", "sessionid", session);
                }else {
                    // Two active clients are present, show the server busy page
                    HTTP_SendHTMLFile(res, "./public/8_server_busy.html");
                }
                
                return;
            }
        }else {
            // A JSON API to send keyword data, this data is identified via 
            // the sesisonid
            char path_keywords = !strcmp(req->Path, "/keywords");

            // This is the main path, we we return start.html
            if(DT_HashTable_Has(req->Cookies, "user") && (!path_keywords)) {
                HTTP_SendHTMLFile(res, "./public/2_start.html");
            }else {
                // Send a JSON payload to be rendered dynamically in our frontend
                if(path_keywords) {
                    char *keywords = GM_GetKeywordsJSON(game, DT_HashTable_Gets(req->Cookies, "sessionid", NULL));
                    HTTP_SendJSON(res, keywords);
                    free(keywords);
                }else {
                    HTTP_SendHTMLFile(res, "./public/1_intro.html");
                }
            }
            
        }
    
    // Deal with POST requests here
    }else if(req->Method == HTTP_POST) {

        char *username = DT_HashTable_Gets(req->FormValues, "user", NULL);
        char *sessionid = DT_HashTable_Gets(req->Cookies, "sessionid", NULL);
        char *quit = DT_HashTable_Gets(req->FormValues, "quit", NULL);
        char *guess = DT_HashTable_Gets(req->FormValues, "guess", NULL);

        if(username) {
            HTTP_SendHTMLFileCookie(res, "./public/2_start.html", "user", username);
            return;
        }

        // Quit the player from the current game, this operation needs to occur
        // in order for other clients to play
        if(quit) {
            if(sessionid) {
                GM_QuitPlayer(game, sessionid);
            }
            HTTP_SendHTMLFileCookie(res, "./public/7_gameover.html", "round", "; expires=Thu, 01 Jan 1970 00:00:00 GMT");
            return;
        }

        
        if(guess) {
            if(game->Ready) {
                if(sessionid) {
                    char *keyword = DT_HashTable_Gets(req->FormValues, "keyword", NULL);
                    if(keyword == NULL) {
                        // Account for empty keyword, http parser parses strlen(keyword)==0 as NULL
                        // We shouldnt add this to the keywords, because empty input 
                        // should be sanitized out?
                        HTTP_SendHTMLFile(res, "./public/4_accepted.html");
                        return;
                    }
                    // Add a keyword to the player identified by the current session
                    if(GM_AddKeyword(game, sessionid, keyword) == KEYWORD_ADDED) {
                        HTTP_SendHTMLFile(res, "./public/4_accepted.html");
                    }else if(GM_AddKeyword(game, sessionid, keyword) == KEYWORD_MATCH) {
                        // We send a cookie to the frontend alerting that the player has reached 
                        // round 1, the frontend then fetches the picture for round 2
                        HTTP_SendHTMLFileCookie(res, "./public/6_endgame.html", "round", "1");
                    }

                }else {
                    //Should never ever get here, 
                    // to be presented with the option to guess, this session id 
                    // would be present because to guess the Start button was pressed
                    // which generates a sessionid
                    HTTP_Redirect(res, "/");
                }
            }else {
                // The game is in a notify state, that is the other player
                // has matched keywords and the current player wasnt alerted about it yet.
                if(game->Notify) {
                    if(sessionid) {
                        if(!strcmp(game->NotifySesssionID, sessionid)) {
                            // All players have been made aware that the current round is over
                            game->Notify = NOTIFY_NONE;
                            // We send a cookie to the frontend alerting that the player has reached 
                            // round 1, the frontend then fetches the picture for round 2
                            HTTP_SendHTMLFileCookie(res, "./public/6_endgame.html", "round", "1");
                            return;
                        }
                    }
                    
                    
                }
                // The game is in a notify state, wanting to let the session id
                // know that the player has quit
                if(game->NotifyQuit) {
                    if(sessionid) {
                        if(!strcmp(game->NotifyQuitSessionID, sessionid)) {
                            game->NotifyQuit = NOTIFY_QUIT_NONE;
                            HTTP_SendHTMLFile(res, "./public/7_gameover.html");
                            return;
                        }
                    }
                }
                // Discard the input, because the player isnt ready
                HTTP_SendHTMLFile(res, "./public/5_discarded.html");
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

    GM_FreeGame(game);
    
    return 0;
}