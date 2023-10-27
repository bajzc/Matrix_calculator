CC = gcc
SRC = ast-exec.c ast-tree.c lex.c main.c parser.c syms.c myprintf.c mem_pool.c
JIT_SRC = ast_jit.c
OBJ = $(patsubst %.c,%.o,${SRC})
JIT_OBJ = $(patsubst %.c,%.o,${JIT_SRC})
all: CLI

CLI:pre ${OBJ} ${JIT_OBJ}
	${CC} -g -O0 -Wall ${OBJ} ${JIT_OBJ} -lgccjit -o $@

pre:
	gcc --version
	flex -o lex.c lex.l
	bison -Wall -Wcounterexamples -d parser.y -o parser.c


${OBJ}:%.o:%.c
	${CC} -c -g $<

${JIT_OBJ}:%.o:%.c
	${CC} -c -g $< -lgccjit

clean:
	rm *.o
	rm CLI

.PHONY: pre clean