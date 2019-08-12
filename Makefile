# Environment and config

CC = gcc
INC = -Isrc -Ilib -Iui/build
FLG = -g -std=c99 -O3 -Wall -Wextra -pedantic -Wunicode-whitespace
DEB = -DDEBUG # Debug specific flags
DEF = ""
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
UI_BUNDLE = ui/build/bundle.o


# Targets

all: $(BIN)

app: DEB = ""
app: $(BIN)
	rm -r bin/Markbook.app || echo ""
	cp -r bundle bin/bundle
	mv bin/bundle bin/Markbook.app
	mkdir bin/Markbook.app/Contents/MacOS
	cp bin/markbook bin/Markbook.app/Contents/MacOS

$(BIN): $(OBJ)
$(BIN): $(UI_BUNDLE)
	$(CC) $(OBJ) $(UI_BUNDLE) $(WEBVIEW_LDFLAGS) -o $(BIN)

$(OBJ): %.o: %.c
	$(CC) $< $(INC) $(FLG) $(DEF) $(DEB) $(WEBVIEW_CFLAGS) -c -o $@

$(UI_BUNDLE):
	make -C ui

# Functions

run: all
	# This is a helper during development
	bin/markbook

clean:
	rm bin/* $(shell find . -name *.o) || echo "Some file where already cleared."
	rm -r bin/Markbook.app || echo "Some file where already cleared."
	make -C ui clean

.PHONY: all app $(UI_BUNDLE) run clean
