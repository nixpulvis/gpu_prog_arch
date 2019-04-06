TARGET = nixlang

BIN_DIR = bin

CC = gcc
CFLAGS = -std=gnu99 -O0 -g -Wall -pedantic -Wextra \
				 -Wno-implicit-function-declaration -Wno-unused-function \
				 -Wno-unneeded-internal-declaration
LIBRARIES = -ll -lm

all:
	flex -o lex.c lex.l
	bison -d -o parser.c parser.y
	$(CC) $(CFLAGS) -o $(TARGET) parser.c lex.c $(LIBRARIES)
