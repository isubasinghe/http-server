#ifndef GAME_H

#define SESSION_LEN 129 // 128 + 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dt_warray.h"

#define CAN_START 1
#define CANT_BUSY 2

#define KEYWORD_ADDED 1
#define KEYWORD_NOTREADY 2
#define KEYWORD_ERROR 3
#define KEYWORD_MATCH 4

#define NOTIFY_NONE 0
#define NOTIFY_P1 1
#define NOTIFY_P2 2

#define JSON_START "{\"keywords\": ["
#define JSON_END "]}"

typedef struct {
    char SessionID[SESSION_LEN];
    char Active;
    DT_WArray *Keywords;
} GM_GamePlayer;

typedef struct {
    GM_GamePlayer *Player1;
    GM_GamePlayer *Player2;
    char Ready;
    char Notify;
} GM_GamePool;

void GM_FreePlayer(GM_GamePlayer *player);
GM_GamePlayer *GM_CreatePlayer();

GM_GamePool *GM_CreateGame();
char GM_StartPlayer(GM_GamePool *pool, char *session);
char GM_AddKeyword(GM_GamePool *pool, char *session, char *keyword);
void GM_QuitPlayer(GM_GamePool *pool, char *session);
void GM_FreeGame(GM_GamePool *pool);

char *GM_GetKeywordsJSON(GM_GamePool *pool, char *session);

char *GM_GetKeywordsJSON(GM_GamePool *pool, char *session) {

    DT_WArray *array = NULL;
    if((!strcmp(pool->Player1->SessionID, session)) && (session != NULL)) {
        array = pool->Player1->Keywords;
    }else if((!strcmp(pool->Player2->SessionID, session)) && (session != NULL)) {
        array = pool->Player2->Keywords;
    }

    size_t allocate = sizeof(JSON_START) + sizeof(JSON_END);
    if(array) {
        for(size_t i = 0; i < array->written; i++) {
            allocate += strlen(array->data[i]) + 4; // two for quotes, one for space and one for comma
        }
    }
    

    char *keywords = malloc(allocate);
    memset(keywords, 0, allocate);
    if(keywords == NULL) {
        return NULL;
    }

    memcpy(keywords, JSON_START, sizeof(JSON_START));

    if(array) {
        for(size_t i=0; i < array->written-1; i++) {
            strcat(keywords, "\"");
            strcat(keywords, array->data[i]);
            strcat(keywords, "\"");
            strcat(keywords, ",");
        }
        if(array->written) {
            strcat(keywords, "\"");
            strcat(keywords, array->data[array->written-1]);
            strcat(keywords, "\"");
        }
    }

    strcat(keywords, JSON_END);

    return keywords;
}

char GM_StartPlayer(GM_GamePool *pool, char *session) {
    char assigned = 0;
    if(!(pool->Player1->Active)) {
        if(strcmp(pool->Player2->SessionID, session) != 0) {
            printf("ADDED PLAYER 1\n");
            pool->Player1->Active = 1;
            memcpy(pool->Player1->SessionID, session, SESSION_LEN);
            assigned = 1;
            pool->Player1->Keywords = DT_CreateWArray();
        }
    }
    if (!(pool->Player2->Active) &&(!assigned)) {
        if(strcmp(pool->Player1->SessionID, session) != 0) {
            printf("ADDED PLAYER 2\n");
            pool->Player2->Active = 1;
            memcpy(pool->Player2->SessionID, session, SESSION_LEN);
            assigned = 1;
            pool->Player2->Keywords = DT_CreateWArray();
        }
    }

    if((pool->Player1->Active) && (pool->Player2->Active)) {
        printf("READY TO GAME\n");
        pool->Ready = 1;
    }
    if(assigned) {
        return CAN_START;
    }else {
        return CANT_BUSY;
    }
}

char GM_AddKeyword(GM_GamePool *pool, char *session, char *keyword) {
    
    if(!strcmp(pool->Player1->SessionID, session)) {
        if(!pool->Player1->Active) {
            return KEYWORD_MATCH;
        }
        printf("Keyword %s added to player1\n", keyword);
        DT_WArray_Append(pool->Player1->Keywords, strdup(keyword));
        for(int i=0; i < pool->Player2->Keywords->written; i++) {
            if(!strcmp(pool->Player2->Keywords->data[i], keyword)) {
                pool->Player1->Active = 0;
                pool->Player2->Active = 0;
                pool->Ready = 0;
                pool->Notify = NOTIFY_P2;
                return KEYWORD_MATCH;
            }
        }
        return KEYWORD_ADDED;
    }else if(!strcmp(pool->Player2->SessionID, session)) {
       if(!pool->Player2->Active) {
            return KEYWORD_MATCH;
        }
        printf("Keyword %s added to player2\n", keyword);
        DT_WArray_Append(pool->Player2->Keywords, strdup(keyword));
        for(int i=0; i < pool->Player1->Keywords->written; i++) {
            if(!strcmp(pool->Player1->Keywords->data[i], keyword)) {
                pool->Player1->Active = 0;
                pool->Player2->Active = 0;
                pool->Ready = 0;
                pool->Notify = NOTIFY_P1;
                return KEYWORD_MATCH;
            }
        }
        return KEYWORD_ADDED;
    } else {
        return KEYWORD_ERROR;
    }
    return KEYWORD_ERROR;
}

void GM_QuitPlayer(GM_GamePool *pool, char *session) {
    printf("QUIT PLAYER\n");
    if(!strcmp(session, pool->Player1->SessionID)) {
        pool->Player1->Active = 0;
        pool->Ready = 0;
        return;
    }

    if(!strcmp(session, pool->Player2->SessionID)) {
        pool->Player2->Active = 0;
        pool->Ready = 0;
        return;
    }

}

GM_GamePlayer *GM_CreatePlayer() {
    GM_GamePlayer *player = malloc(sizeof(GM_GamePlayer));
    memset(player->SessionID, 0, SESSION_LEN);
    player->Active = 0;
    player->Keywords = DT_CreateWArray();
    if(player->Keywords == NULL) {
        GM_FreePlayer(player);
        return NULL;
    }
    return player;
}

void GM_FreePlayer(GM_GamePlayer *player) {
    if(player->Keywords) {
        DT_FreeWArray(player->Keywords);
    }
    free(player);
}



GM_GamePool *GM_CreateGame() {
    GM_GamePool *pool = malloc(sizeof(GM_GamePool));
    if(pool == NULL) {
        return NULL;
    }
    pool->Notify = NOTIFY_NONE;
    memset(pool, 0, sizeof(GM_GamePool));

    pool->Player1 = GM_CreatePlayer();
    if(pool->Player1 == NULL) {
        GM_FreeGame(pool);
    }

    pool->Player2 = GM_CreatePlayer();
    if(pool->Player2 == NULL) {
        GM_FreeGame(pool);
    }

    return pool;
}   

void GM_FreeGame(GM_GamePool *pool) {
    if(pool->Player1) {
        GM_FreePlayer(pool->Player1);
    }
    if(pool->Player2) {
        GM_FreePlayer(pool->Player2);
    }
    free(pool);
}



#endif //GAME_H