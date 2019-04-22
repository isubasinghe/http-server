CC	=	gcc

CFLAGS	=	-Wall -Werror -pedantic -lpthread -std=gnu99
EXE = image_tagger
OBJ = main.o http_server.o http_router.o http_parser.o dt_hashtable.o http_writer.o

all: $(EXE)

debug: CFLAGS = -Wall -Werror -pedantic -lpthread -std=gnu99 -O0 -g -DDEBUG

main.o: http_parser.h
dt_hashtable.o: dt_hashtable.h
http_server.o: http_server.h http_router.h http_parser.h http_writer.h
http_router.o: http_router.h http_parser.h http_writer.h
http_parser.o: http_parser.h dt_hashtable.o
http_writer.o: http_writer.h http_status.h


debug: $(OBJ)
	$(CC) -o $(EXE) $(OBJ) $(CFLAGS)

	
$(EXE): $(OBJ)
		$(CC) -o $(EXE) $(OBJ) $(CFLAGS)

.PHONY: clean cleanly

clean:
	rm -rf $(OBJ)
