#include <stdio.h>
#include <stdlib.h>



#include "http_parser.h"
#include "http_writer.h"
#include "http_server.h"

int main(int argc, char *argv[]) {

    


  
    HTTP_Server *server = HTTP_CreateServer();
    if(HTTP_StartServer(server, "127.0.0.1", 8080)) {
        HTTP_FreeServer(server);
    }
    
    return 0;
}