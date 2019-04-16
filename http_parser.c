#include "http_parser.h"





HTTP_Request *HTTP_ParseRequestLine(HTTP_Request *request, char *buffer, size_t len, char **next) {
    buffer[len-1] = 0;

    char *saveptr = NULL;
    char *method = strtok_r(buffer, " ", &saveptr);
    if(method == NULL) {
        free(request);
        return NULL;
    }
    char *path = strtok_r(NULL, " ", &saveptr);
    if(path == NULL) {
        free(request);
        return NULL;
    }
    char *version = strtok_r(NULL, " ", &saveptr);
    if(version == NULL) {
        free(request);
        return NULL;
    }

    char curr = version[0];
    size_t start_i = 0;

    while(curr != 0) {
        curr = version[start_i];
        if(curr == SPACE) {
            version[start_i] = 0;
        }else if (curr == CR) {
            version[start_i] = 0;
        }else if(curr == LF) {  
            version[start_i] = 0;
            break;
        }
        start_i++;
    }

    if(curr == 0) {
        free(request);
        return NULL;
    }

    (*next) = &version[start_i+1];

    request->Method = method;
    request->Path = path;
    request->Version = version;



    return request;
}

char HTTP_HeadersPresent(char *buffer) {
    return 0;
}



HTTP_Request *HTTP_ParseRequest(char *buffer, size_t len) {
    buffer[len-1] = 0;

    char *next = NULL;

    HTTP_Request *request = malloc(sizeof(HTTP_Request));

    request = HTTP_ParseRequestLine(request, buffer, len, &next);
    if(request == NULL) {
        return NULL;
    }
    return request;

}


void HTTP_FreeRequest(HTTP_Request *request) {
    free(request);
}