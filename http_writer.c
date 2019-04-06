#include "http_writer.h"

char *HTTP_GetStatusPhrase(int status) {
    switch(status) {
        case HTTP_OK:
            return "OK";
            break;
        case HTTTP_BAD_REQUEST:
            return "Bad Request";
            break;
        default:
            return NULL;
    }
}