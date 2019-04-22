#include "http_server.h"


static const int sigs[] = {SIGIO,SIGHUP,SIGTERM,SIGINT,SIGQUIT,SIGALRM};
static const int sigCount = 6;


// Setup the epoll file descriptor
// then add the signal fd and listening socket
// to the epoll fd
static char SetupEpoll(HTTP_Server *server) {
    // MAX_ACTIVE_FD is discarded in the latest linux 
    // kernel, however to support older kernels
    // set the size to the correct upper limit
    server->__epoll_fd = epoll_create(MAX_ACTIVE_FD);
    if(server->__epoll_fd < 0) {
        perror("Failed to create epoll fd\n");
        return 0;
    }

    // Declare an epoll event and memset it to zero
    struct epoll_event event;
    memset(&event, 0, sizeof(struct epoll_event));

    // Add the signal file descriptor to epoll
    event.data.fd = server->__sigfd;
    event.events = EPOLLIN;
    if(epoll_ctl(server->__epoll_fd, EPOLL_CTL_ADD, server->__sigfd, &event)) {
        perror("Unable to add epoll to signal fd\n");
        return 0;
    }

    // Clear the epoll data structure, so we may add another file
    // descriptor to it
    memset(&event, 0, sizeof(struct epoll_event));

    // Add the listening socket to epoll
    event.data.fd = server->ServerSock;
    event.events = EPOLLIN;
    if(epoll_ctl(server->__epoll_fd, EPOLL_CTL_ADD, server->ServerSock, &event)) {
        perror("Unable to add epoll to socket fd\n");
        return 0;
    }
    return 1;
}

static char SetupSigFD(HTTP_Server *server) {
    // Block all signals
    sigset_t all;
    sigfillset(&all);
    sigprocmask(SIG_SETMASK,&all,NULL);

    // Accept the signals defined in the array sigs
    sigset_t sigset;
    sigemptyset(&sigset);
    for(size_t i=0; i < sigCount; i++) {
        sigaddset(&sigset, sigs[i]);
    }
    
    // Create a file descriptor to indicate a 
    // signal
    int sigfd = signalfd(-1, &sigset, 0);
    if(sigfd == -1) {
        HTTP_FreeServer(server);
        return 0;
    }
    server->__sigfd = sigfd;
    return 1;
}

static char CreateServerSocket(HTTP_Server *server) {
    // Create a TCP socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0) {
        perror("Unable to create socket\n");
        return 0;
    }
    
    // Setup the server information
    server->__server.sin_family = AF_INET;
    server->__server.sin_addr.s_addr = inet_addr(server->Host);
    server->__server.sin_port = htons(server->Port);

    // Bind the socket to the specified port
    if(bind(sock, (struct sockaddr *)&server->__server, sizeof(server->__server)) < 0) {
        perror("Unable to bind\n");
        return 0;
    }

    // Listen for connections
    if(listen(sock, LISTEN_BACKLOG)<0) {
        perror("Unable to bind\n");
        return 0;
    }
    server->ServerSock = sock;
    return 1;
}

static void AddEpollClient(HTTP_Server *server) {
    int sock;
    
    struct sockaddr_in client;
    memset(&client, 0, sizeof(struct sockaddr_in));
    int c = sizeof(struct sockaddr_in);
    
    sock = accept(server->ServerSock, (struct sockaddr *)&client, (socklen_t *)&c);
    if(sock < 0) {
        return;
    }
    struct epoll_event event;
    memset(&event, 0, sizeof(struct epoll_event));
    event.data.fd = sock;
    event.events = EPOLLIN;
    if(epoll_ctl(server->__epoll_fd, EPOLL_CTL_ADD, sock, &event)) {
        close(sock);
    }
}


static void ReadClient(HTTP_Server *server, int sock) {
    char buffer[READ_SIZE];
    memset(buffer, 0, READ_SIZE);
    buffer[READ_SIZE-1] = 0;

    if(recv(sock, buffer, READ_SIZE, 0) < 2) {
        epoll_ctl(server->__epoll_fd, EPOLL_CTL_DEL, sock, NULL);
        close(sock);
        return;
    }
    buffer[READ_SIZE-1] = 0;

    HTTP_Request *req = HTTP_ParseRequest(buffer, READ_SIZE);
    if(req == NULL) {
        printf("Failed to parse\n");
    }
    HTTP_Response *res = HTTP_CreateResponse();
    res->__sock = sock;
    server->Router->route(req, res);
    HTTP_FreeRequest(req);
    HTTP_FreeResponse(res);
    return;
}

#ifdef WITH_THREADS
static void *ReadClientThread(void *args) {
    __ThreadArg *targs = (__ThreadArg *)args;
    ReadClient(targs->server, targs->sock);
    return NULL;
}
#endif // WITH_THREADS

HTTP_Server *HTTP_CreateServer() {
    // Allocate a new structure and zero all the data
    HTTP_Server *server = malloc(sizeof(HTTP_Server));
    if(server==NULL) {
        return NULL;
    }
    memset(server, 0, sizeof(HTTP_Server));
    server->Router = HTTP_CreateRouter();
    server->Host = NULL;
    server->Port = 0;
    server->ServerSock = 0;
    server->__epoll_fd = 0;
    memset(server->__events, MAX_ACTIVE_FD, sizeof(struct epoll_event));
    memset(&(server->__server), 0, sizeof(struct sockaddr_in));
    return server;
}

void HTTP_SetRouter(HTTP_Server *server, HTTP_Router *router) {
    if(server->Router) {
        free(server->Router);
    }
    server->Router = router;
}

char HTTP_StartServer(HTTP_Server *server, char *host, unsigned short port) {
    // host memory could be from the stack, its safer to 
    // put it on the heap
    server->Host = strdup(host);
    server->Port = port;
    if(!SetupSigFD(server)) {
        return 0;
    }
    if(!CreateServerSocket(server)) {
        return 0;
    }

    if(!SetupEpoll(server)) {
        return 0;
    }
    #ifdef WITH_THREADS
    pthread_t thread_id;
    #endif // WITH_THREADS

    char loop = 1;
    while(loop) {
        int active_count = epoll_wait(server->__epoll_fd, server->__events, MAX_ACTIVE_FD, -1);
        for(int i=0; i < active_count; i++) {
            if(server->__events[i].data.fd == server->__sigfd) {
                printf("RECEIVED SIGNAL, CLOSING DOWN SERVER\n");
                loop = 0;
                continue;
            }else if(server->__events[i].data.fd == server->ServerSock) {
                AddEpollClient(server);
            }else {
                #ifndef WITH_THREADS
                ReadClient(server, server->__events[i].data.fd);
                #else
                // For now we dont have a worker pool
                // spawn a pthread to handle a ready client
                __ThreadArg args;
                args.server = server;
                args.sock = server->__events[i].data.fd;
                pthread_create(&thread_id, NULL, ReadClientThread, (void *)&args);
                #endif // WITH_THREADS
            }
        }
    } 

    close(server->__epoll_fd);
    close(server->ServerSock);
    return 1;
}



void HTTP_FreeServer(HTTP_Server *server) {
    free(server->Router);
    free(server->Host);
    free(server);
}