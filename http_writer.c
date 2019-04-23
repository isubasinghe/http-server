#include "http_writer.h"


HTTP_Response *HTTP_CreateResponse() {
    HTTP_Response *response = malloc(sizeof(HTTP_Response));
    response->Protocol = HTTP_SERVER_PROTO;
    response->ProtocolLen = strlen(HTTP_SERVER_PROTO);
    if(response == NULL) {
        return NULL;
    }
    memset(response, 0, sizeof(HTTP_Response));

    response->StatusCode = "200 ";
    response->CodeLen = HTTP_STATUS_LEN;

    response->Phrase = HTTP_GetStatusPhrase(200);
    response->PhraseLen = strlen(response->Phrase);


    return response;
}

char *HTTP_GetStatusPhrase(int status) {
    switch(status) {
        case HTTP_OK:
            return "OK \r\n";
            break;
        case HTTTP_BAD_REQUEST:
            return "Bad Request \r\n";
            break;
        default:
            return NULL;
    }
}

void HTTP_SendHTMLFile(HTTP_Response *res, char *fname) {
    struct stat s;
    if(!stat(fname, &s)) {
        dprintf(res->__sock, HTTP_200_RESPONSE, s.st_size);
        int fd = open(fname, O_RDONLY);
        sendfile(res->__sock, fd, NULL, s.st_size);
    }
}

void HTTP_SendHTMLFileCookie(HTTP_Response *res, char *fname, char *key, char *value) {
    struct stat s;
    if(!stat(fname, &s)) {
        dprintf(res->__sock, HTTP_200_COOKIE_RESPONSE, key, value, s.st_size);
        int fd = open(fname, O_RDONLY);
        sendfile(res->__sock, fd, NULL, s.st_size);
    }
}

void HTTP_SendJSON(HTTP_Response *res, char *json) {
    dprintf(res->__sock, HTTP_200_JSON_RESPONSE, (long int)strlen(json), json);
}

void HTTP_Redirect(HTTP_Response *res, char *path) {
    dprintf(res->__sock, HTTP_301_REDIRECT, path);
}

void HTTP_RedirectCookie(HTTP_Response *res, char *path, char *key, char *value) {
    dprintf(res->__sock, HTTP_301_COOKIE_REDIRECT, key, value, path);
}

void HTTP_EndResponse(HTTP_Response *response) {
    send(response->__sock, response->Protocol, response->ProtocolLen, MSG_DONTWAIT);  
    send(response->__sock, response->StatusCode, response->CodeLen, MSG_DONTWAIT);
    send(response->__sock, response->Phrase, response->PhraseLen, MSG_DONTWAIT);
}


void HTTP_FreeResponse(HTTP_Response *response) {
    free(response);
}