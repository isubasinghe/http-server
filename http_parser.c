#include "http_parser.h"




size_t first_occur(char *str, char ch, size_t len) {
    size_t index = -1;
    for(size_t i=0; i < len; i++) {
        if(str[i] == ch) {
            return i;
        }
    }
    return index;
}

HTTP_Request *HTTP_ParseRequest(char *buffer, size_t len) {
    buffer[len-1] = 0;
    HTTP_Request *request = malloc(sizeof(HTTP_Request));

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
    request->Method = method;
    request->Path = path;
    request->Version = version;

    printf("%s\n", method);
    printf("%s\n", path);
    printf("%s\n", version);

    // size_t lf_count = 0;
    // size_t i = 0;
    // while(1) {
    //     if(version[i] == 0) {
    //         break;
    //     }else if(version[i] == LF) {
    //         lf_count++;
    //         i++;
    //     }else if(version[i] == SPACE) {
    //         i++;
    //     }else if(version[i] == CR) {
    //         i++;
    //     }else {
    //         break;
    //     }
    // }
    return request;

}


void HTTP_FreeRequest(HTTP_Request *request) {
    free(request);
}