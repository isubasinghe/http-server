#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#define MAX_ACTIVE_FD 500000
#define LISTEN_BACKLOG 10
#define READ_SIZE 10240

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>

#include <unistd.h>
#include <poll.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/signalfd.h>
#include <sys/sendfile.h>

#include "http_router.h"
#include "http_parser.h"
#include "http_writer.h"


typedef struct {
    unsigned short Port;
    char *Host;
    int ServerSock;
    HTTP_Router *Router;
    int __sigfd;
    struct sockaddr_in __server;
    struct epoll_event __events[MAX_ACTIVE_FD];
    int __epoll_fd;
} HTTP_Server;

#ifdef WITH_THREADS
typedef struct {
    HTTP_Server *server;
    int sock;
} __ThreadArg;
#endif // WITH_THREADS

HTTP_Server *HTTP_CreateServer();

void HTTP_SetRouter(HTTP_Server *server, HTTP_Router *router);

char HTTP_StartServer(HTTP_Server *server, char *host, unsigned short port);

void HTTP_FreeServer(HTTP_Server *server);

#endif // HTTP_SERVER_H