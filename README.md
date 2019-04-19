# COMP30023-Assignment1 - Blaze HTTP Server v0.0.1

### What is this project?
 This project started off as an assignment for university of melbourne, then I got really carried away and was wondering about implementing a full http server, with routers, proper parsing of http requests etc. As of this writing, that part isnt really complete but it is heading in that direction.
 
### The core principle behind Blaze
The principle behine blaze is to be as fast and concurrent as possible on a modern linux server. In order to achieve this, the 
server uses epoll and follows the ngnix model of multiplexing. That is, the server uses one thread to listen for 
sockets that are ready, then passes this socket to a worker pool to be dealt with. In order to ensure speed,
the blaze does **ZERO MEMORY ALLOCATION** for string manipulation when parsing http requests, 
and it should always stay that way.

### TODO
* Add functionality to ensure a fully HTTP/1.0 compliant request parser
* Increase ability to handle malformed (extra spaces etc.) HTTP requests without sacrificing on any speed
* Add functionality to ensure a fully HTTP/1.0 compliant response writer
* Complete the http router using a hashtable (needs to be thread safe). A hashtable for each HTTP METHOD would reduce contention
* Implement a worker pool (atm threads are started to deal with active sockets)


