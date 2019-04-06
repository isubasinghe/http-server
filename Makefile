CC	=	gcc

CFLAGS	=	-Wall -Werror -pedantic -std=c99 -O2
EXE = image_tagger
OBJ = main.o http_parser.o http_writer.o

all: $(EXE)

main.o: http_parser.h
http_parser.o: http_parser.h
http_writer.o: http_writer.h http_status.h

$(EXE): $(OBJ)
		$(CC) $(CFLAGS) -o $(EXE) $(OBJ)

.PHONY: clean cleanly

clean:
	rm -rf $(OBJ)
