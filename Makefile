CC	=	gcc

CFLAGS	=	-Wall -Werror -pedantic -lpthread -std=gnu99 -O0 -g 
EXE = image_tagger
OBJ = main.o http_server.o http_parser.o http_writer.o

all: $(EXE)

main.o: http_parser.h
http_server.o: http_server.h
http_parser.o: http_parser.h
http_writer.o: http_writer.h http_status.h

$(EXE): $(OBJ)
		$(CC) -o $(EXE) $(OBJ) $(CFLAGS)

.PHONY: clean cleanly

clean:
	rm -rf $(OBJ)
