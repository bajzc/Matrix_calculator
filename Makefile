CC = gcc
SRC = ast-exec.c ast-tree.c lex.c main.c parser.c syms.c myprintf.c
DEBUG_SRC = main.c syms.c parser.c
pre:
	bison -Wall -Wcounterexamples -d parser.y -o parser.c
	flex -o lex.c lex.l

all: pre
	$(CC) -g -O0 -lm $(SRC)

debugsym: pre
	$(CC) -g -O0 -lm -D DEBUG_SYM=1 $(DEBUG_SRC)