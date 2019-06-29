# Environment and config

CC = gcc
INC = -Isrc -Ilib
FLG = -g -std=c99 -O3 -Wall -Wextra -pedantic
BIN = bin/markbook

SRC = $(shell find . -name *.c)
OBJ = $(SRC:%.c=%.o)


# Targets

all: $(BIN)

app: $(BIN)
	echo "TODO: Create macOS .app bundle"

$(BIN): $(OBJ)
	$(CC) $(OBJ) -o $(BIN)

$(OBJ): %.o: %.c
	$(CC) $< $(INC) $(FLG) -c -o $@


clean:
	rm -r bin/*
	rm $(shell find . -name *.o)
