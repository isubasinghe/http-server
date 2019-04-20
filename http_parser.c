#include "http_parser.h"

static void parseQuery(HTTP_Request *req, char *buffer) {
    char *saveptr = NULL;
    char *key = strtok_r(buffer, QUERY_SEP, &saveptr);
    if(key != NULL) {
        char *value = strtok_r(NULL, QUERY_SEP, &saveptr);
        if(value != NULL) {
            DT_HashTable_Put(req->Queries, key, value);
            req->HasQueries = 1;
        }
    }
}

static void parseQueries(HTTP_Request *request, char *buffer) {
    char *saveptr = NULL;
    char *path = strtok_r(buffer, QUERY_INDICATOR, &saveptr);
    if(path != NULL) {
        char *query = strtok_r(NULL, QUERY_INDICATOR, &saveptr);
        if(query != NULL) {
            char *qsaveptr = NULL;
            char *query_start = strtok_r(query, QUERY_AND, &qsaveptr);
            if(query_start != NULL) {
                parseQuery(request, query_start);
                while((query_start = strtok_r(NULL, QUERY_AND, &qsaveptr) ) != NULL) {
                    parseQuery(request, query_start);
                }
            }
        }
    }
}


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

    parseQueries(request, request->Path);


    return request;
}

char HTTP_HeadersPresent(char *buffer) {
    return 0;
}



HTTP_Request *HTTP_ParseRequest(char *buffer, size_t len) {
    buffer[len-1] = 0;

    char *next = NULL;

    HTTP_Request *request = malloc(sizeof(HTTP_Request));
    if(request == NULL) {
        return NULL;
    }
    memset(request, 0, sizeof(HTTP_Request));

    request->Queries = DT_CreateHashTable(QUERY_MAX);
    if(request->Queries == NULL) {
        free(request);
        return NULL;
    }

    request = HTTP_ParseRequestLine(request, buffer, len, &next);
    if(request == NULL) {
        return NULL;
    }
    return request;

}


void HTTP_FreeRequest(HTTP_Request *request) {
    DT_FreeHashTable(request->Queries);
    free(request);
}