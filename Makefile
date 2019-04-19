CC	=	gcc

CFLAGS	=	-Wall -Werror -pedantic -lpthread -std=gnu99 -O0 -g 
EXE = image_tagger
OBJ = main.o http_server.o http_router.o http_parser.o dt_hashtable.o linklist.o hashtable.o http_writer.o

all: $(EXE)

main.o: http_parser.h
linklist.o: atomic_defs.h bsd_queue.h linklist.h
hashtable.o: atomic_defs.h bsd_queue.h linklist.h hashtable.h
dt_hashtable.o: dt_hashtable.h
http_server.o: http_server.h http_router.h http_parser.h http_writer.h
http_router.o: http_router.h http_parser.h http_writer.h
http_parser.o: http_parser.h atomic_defs.h dt_hashtable.o
http_writer.o: http_writer.h http_status.h

$(EXE): $(OBJ)
		$(CC) -o $(EXE) $(OBJ) $(CFLAGS)

.PHONY: clean cleanly

clean:
	rm -rf $(OBJ)
