#include <stdio.h>
#include <stdlib.h>



#include "http_parser.h"
#include "http_writer.h"
#include "http_server.h"

int main(int argc, char *argv[]) {
    char data[] = "GET /?query=fair&zxc HTTP/1.1\r\n\r\n";
    
    HTTP_Request *request = HTTP_ParseRequest(data, sizeof(data));
    if(request != NULL) {
        HTTP_FreeRequest(request);
    }else {
        printf("Request parse failed\n");
    }

    // HTTP_Server *server = HTTP_CreateServer();
    // if(HTTP_StartServer(server, "127.0.0.1", 8080)) {
    //     HTTP_FreeServer(server);
    // }
    
    return 0;
}