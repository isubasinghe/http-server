#include "http_server.h"


static int sigs[] = {SIGIO,SIGHUP,SIGTERM,SIGINT,SIGQUIT,SIGALRM};
static int sigCount = 6;

static char SetupEpoll(HTTP_Server *server) {
    int epoll_fd = epoll_create(MAX_ACTIVE_FD);
    if(epoll_fd < 0) {
        perror("Failed to create epoll fd\n");
        return 0;
    }
    return 1;
}

static char SetupSigFD(HTTP_Server *server) {
    sigset_t all;
    sigfillset(&all);
    sigprocmask(SIG_SETMASK,&all,NULL);

    sigset_t sigset;
    sigemptyset(&sigset);
    for(size_t i=0; i < sigCount; i++) {
        sigaddset(&sigset, sigs[i]);
    }
    
    int sigfd = signalfd(-1, &sigset, 0);
    if(sigfd == -1) {
        HTTP_FreeServer(server);
        return 0;
    }
    server->__sigfd = sigfd;
    return 1;
}

static char CreateServerSocket(HTTP_Server *server) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0) {
        perror("Unable to create socket\n");
        return 0;
    }
    server->__server.sin_family = AF_INET;
    server->__server.sin_addr.s_addr = inet_addr(server->Host);
    server->__server.sin_port = htons(8080);

    if(bind(sock, (struct sockaddr *)&server->__server, sizeof(server->__server)) < 0) {
        perror("Unable to bind\n");
        return 0;
    }
    if(listen(sock, LISTEN_BACKLOG)<0) {
        perror("Unable to bind\n");
        return 0;
    }
    server->ServerSock = sock;
    return 1;
}

HTTP_Server *HTTP_CreateServer() {
    HTTP_Server *server = malloc(sizeof(HTTP_Server));
    return server;
}

char HTTP_StartServer(HTTP_Server *server, char *host, unsigned short port) {
    server->Host = strdup(host);
    server->Port = port;
    
    if(!SetupEpoll(server)) {
        return 0;
    }

    if(!SetupSigFD(server)) {
        return 0;
    }
    if(!CreateServerSocket(server)) {
        return 0;
    }
    close(server->ServerSock);
    return 1;
}



void HTTP_FreeServer(HTTP_Server *server) {
    free(server->Host);
    free(server);
}