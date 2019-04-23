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

static void parseCookie(HTTP_Request *request, char *buffer) {
    char *saveptr = NULL;
    char *cookie_key = strtok_r(buffer, "=", &saveptr);
    if(cookie_key != NULL) {
        char *cookie_value = strtok_r(NULL, "=", &saveptr);
        if(cookie_value != NULL) {
            DT_HashTable_Put(request->Cookies, cookie_key, cookie_value);
        }
    }
}

static void parseCookies(HTTP_Request *request, char *buffer) {
    char *saveptr = NULL;
    char *cookie = strtok_r(buffer, "; ", &saveptr);
    while(cookie != NULL) {
        parseCookie(request, cookie);
        cookie = strtok_r(NULL, "; ", &saveptr);
    }
}

static void parseHeader(HTTP_Request *request, char *buffer) {
    char *saveptr = NULL;
    char *header_key = strtok_r(buffer, ": ", &saveptr);
    if(header_key != NULL) {
        char *header_value = strtok_r(NULL, ":", &saveptr);
        if(header_value != NULL) {
            if(!strcmp(header_key, COOKIE_STR)) {
                parseCookies(request, header_value);
            }else {
                
                if(*header_value == SPACE) {
                    header_value++;
                }
                DT_HashTable_Put(request->Headers, header_key, header_value);
            }
        }
    }
}


static void parseFormValue(HTTP_Request *request, char *buffer) {
    char *saveptr = NULL;
    char *form_key = strtok_r(buffer, "=", &saveptr);
    if(form_key) {
        char *form_value = strtok_r(NULL, "=", &saveptr);
        if(form_value) {
            DT_HashTable_Put(request->FormValues, form_key, form_value);
        }
    }
}

static void parseFormValues(HTTP_Request *request, char *buffer) {
    char *saveptr = NULL;
    char *form_data = strtok_r(buffer, "&", &saveptr);
    while( form_data != NULL ) {
        parseFormValue(request, form_data);
        form_data = strtok_r(NULL, "&", &saveptr);
    }

}

static void terminateBody(HTTP_Request *request) {
    char *len_str = DT_HashTable_Gets(request->Headers, CLEN_HEADER_KEY, NULL);
    
    if(len_str == NULL) {
        return;
    }

    int len = atoi(len_str);
    size_t i = 0;

    while(request->Body[i] != 0) {
        if(i==len) {
            request->Body[i] = 0;
            return;
        }
        i++;
    }
}


HTTP_Request *HTTP_ParseRequest(char *buffer, size_t len) {
    buffer[len-1] = 0;
    HTTP_Request *request = malloc(sizeof(HTTP_Request));
    if(request == NULL) {
        return NULL;
    }
    memset(request, 0, sizeof(HTTP_Request));

    request->Queries = DT_CreateHashTable(QUERY_MAX);
    if(request->Queries == NULL) {
        HTTP_FreeRequest(request);
        return NULL;
    }

    request->Cookies = DT_CreateHashTable(COOKIES_MAX);
    if(request->Cookies == NULL) {
        HTTP_FreeRequest(request);
        return NULL;
    }

    request->Headers = DT_CreateHashTable(HEADERS_MAX);
    if(request->Headers == NULL) {
        HTTP_FreeRequest(request);
        return NULL;
    }

    request->FormValues = DT_CreateHashTable(FORM_MAX);
    if(request->FormValues == NULL) {
        HTTP_FreeRequest(request);
        return NULL;
    }
    
    request->Body = EMPTY_BODY;

    char *saveptr = NULL;
    char *method = strtok_r(buffer, " ", &saveptr);
    if(method == NULL) {
        HTTP_FreeRequest(request);
        return NULL;
    }

    char *path = strtok_r(NULL, " ", &saveptr);
    
    if(path == NULL) {
        HTTP_FreeRequest(request);
        return NULL;
    }
    char set_body = 0;
    char *body = strstr(saveptr, "\r\n\r\n");
    if(body != NULL) {
        // Null terminate the section which is end of headers
        *(body+2) = 0;
        *(body+3) = 0;
        body += 4;
        if(*body != 0) {
            set_body = 1;
        }
    }
    char *version = strtok_r(NULL, "\r\n", &saveptr);
    if(version == NULL) {
        HTTP_FreeRequest(request);
        return NULL;
    }
    
    if(!strcmp(method, "GET")) {
        request->Method = HTTP_GET;
    }else if(!strcmp(method, "POST")) {
        request->Method = HTTP_POST;
        if(set_body) {
            request->Body = body;
        }
    }else {
        HTTP_FreeRequest(request);
        return NULL;
    }

    request->MethodStr = method;
    request->Path = path;
    request->Version = version;

    char *header = NULL;
    while( (header = strtok_r(NULL, "\r\n", &saveptr) ) != NULL) {
        parseHeader(request, header);
    }
    
    parseQueries(request, path);
    terminateBody(request);
   

    char *content_type = DT_HashTable_Gets(request->Headers, CONTENT_TYPE, NULL);
    if(content_type) {
        if(!strcmp(content_type, FORM_TYPE)) {
            parseFormValues(request, body);
        }
    }
    return request;

}


void HTTP_FreeRequest(HTTP_Request *request) {
   if(request->Queries) {
       DT_FreeHashTable(request->Queries);
   }
   if(request->Headers) {
       DT_FreeHashTable(request->Headers);
   }
   if(request->Cookies) {
       DT_FreeHashTable(request->Cookies);
   }
   if(request->FormValues) {
       DT_FreeHashTable(request->FormValues);
   }
    free(request);
}