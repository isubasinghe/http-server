#include <stdio.h>
#include <stdlib.h>

#include "http_parser.h"
#include "http_writer.h"

int main(int argc, char *argv[]) {
    char data[] = "           GET           /            HTTP/1.1\r\n";
    
    HTTP_Request *request = HTTP_ParseRequest(data, sizeof(data));
    HTTP_FreeRequest(request);
    return 0;
}