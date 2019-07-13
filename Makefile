# Environment and config

CC = gcc
INC = -Isrc -Ilib
FLG = -g -std=c99 -O3 -Wall -Wextra -pedantic -Wunicode-whitespace
BIN = bin/markbook

TARGET_OS ?= $(OS)
ifeq ($(TARGET_OS),Windows_NT)
	TARGET=Minimal.exe
	WEBVIEW_CFLAGS := -DWEBVIEW_WINAPI=1
	WEBVIEW_LDFLAGS := -lole32 -lcomctl32 -loleaut32 -luuid -mwindows
else ifeq ($(shell uname -s),Linux)
	WEBVIEW_CFLAGS := -DWEBVIEW_GTK=1 $(shell pkg-config --cflags gtk+-3.0 webkit2gtk-4.0)
	WEBVIEW_LDFLAGS := $(shell pkg-config --libs gtk+-3.0 webkit2gtk-4.0)
else ifeq ($(shell uname -s),Darwin)
	WEBVIEW_CFLAGS := -DWEBVIEW_COCOA=1 -DOBJC_OLD_DISPATCH_PROTOTYPES=1
	WEBVIEW_LDFLAGS := -framework WebKit
endif

SRC = $(shell find . -name *.c)
OBJ = $(SRC:%.c=%.o)


# Targets

all: $(BIN)

app: $(BIN)
	echo "TODO: Create macOS .app bundle"

$(BIN): $(OBJ)
$(BIN): $(HEADERS)
	$(CC) $(OBJ) $(WEBVIEW_LDFLAGS) -o $(BIN)

$(OBJ): %.o: %.c
	$(CC) $< $(INC) $(FLG) $(WEBVIEW_CFLAGS) -c -o $@


clean:
	rm -r bin/* || echo "(skipping bin)"
	rm $(shell find . -name *.o)

.PHONY: clean
