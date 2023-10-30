CC = gcc
SRC = ast-exec.c ast-tree.c lex.c main.c parser.c syms.c myprintf.c mem_pool.c
JIT_SRC = ast_jit.c
OBJ = $(patsubst %.c,%.o,${SRC})
JIT_OBJ = $(patsubst %.c,%.o,${JIT_SRC})

CFLAGS = -g3 -O2 -Wall
CLIBS = -lgccjit
all: jit

jit_flags:
	$(eval CFLAGS+=-DENABLE_JIT)

ast_flags:
	$(eval CFLAGS+=-DENABLE_AST)

jit:clean jit_flags pre ${OBJ} ${JIT_OBJ}
	${CC} ${CFLAGS} ${OBJ} ${JIT_OBJ} ${CLIBS} -o CLI

ast:clean ast_flags pre ${OBJ}
	${CC} ${CFLAGS} ${OBJ} -o CLI

pre:
	flex -o lex.c lex.l
	bison -Wall -Wcounterexamples -d parser.y -o parser.c


${OBJ}:%.o:%.c
	${CC} -c ${CFLAGS} $<

${JIT_OBJ}:%.o:%.c
	${CC} -c ${CFLAGS} $< ${CLIBS}

clean:
	- rm *.o 2> /dev/null || true
	- rm CLI 2> /dev/null || true
	- rm *.out 2> /dev/null || true

.PHONY: pre clean jit_flags ast_flags