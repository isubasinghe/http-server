#ifndef GAME_H



typedef struct {
    char *sessionid;
} GM_GamePlayer;

typedef struct {
    GM_GamePlayer Player1;
    GM_GamePlayer Player2;
} GM_GamePool;

#endif //GAME_H