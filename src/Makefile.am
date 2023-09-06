bin_PROGRAMS = CLI
BUILT_SOURCES = parser.h
AM_YFLAGS = -d -Wcounterexamples -o parser.c
CLI_SOURCES = lex.l parser.y cli.c errorf.c main.c matrix.c syms.c
CLI_LDADD = -lm