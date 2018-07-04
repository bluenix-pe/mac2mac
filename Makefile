GCC=gcc
CFLAGS=-g -std=c99
SOURCE=src/mac2mac.c
BIN=mac2mac

all: $(SOURCE)
	$(GCC) $(SOURCE) $(CFLAGS) -o $(BIN)
