CC = gcc
SRC = ast-exec.c ast-tree.c lex.c main.c parser.c syms.c myprintf.c mem_pool.c
DEBUG_SRC = main.c syms.c parser.c ast-exec.c ast-tree.c lex.c myprintf.c
pre:
	bison -Wall -Wcounterexamples -d parser.y -o parser.c
	flex -o lex.c lex.l

all: pre
	$(CC) -O0 -g3 -Wall -lm $(SRC)

