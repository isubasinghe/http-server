#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#define MAX_ACTIVE_FD 500000
#define LISTEN_BACKLOG 10

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#include <unistd.h>
#include <poll.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/signalfd.h>
#include <sys/sendfile.h>


typedef struct {
    unsigned short Port;
    char *Host;
    int ServerSock;
    int __sigfd;
    struct sockaddr_in __server;
    struct epoll_event __events[MAX_ACTIVE_FD];
    int epoll_fd;
} HTTP_Server;



HTTP_Server *HTTP_CreateServer();


char HTTP_StartServer(HTTP_Server *server, char *host, unsigned short port);

void HTTP_FreeServer(HTTP_Server *server);

#endif // HTTP_SERVER_H