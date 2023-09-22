CC = gcc
SRC = ast-exec.c ast-tree.c lex.c main.c parser.c syms.c

all:
	bison -Wall -Wcounterexamples -d parser.y -o parser.c
	flex -o lex.c lex.l
	$(CC) -g -O0 -lm $(SRC)