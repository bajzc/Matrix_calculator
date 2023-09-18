bin_PROGRAMS = CLI
BUILT_SOURCES = parser.h
AM_YFLAGS = -d -Wcounterexamples -o parser.c
CLI_SOURCES = lex.l parser.y cli.c main.c matrix.c syms.c
CLI_LDADD = -lm
include $(top_srcdir)/aminclude_static.am
CLI_CPPFLAGS = $(CODE_COVERAGE_CPPFLAGS)
CLI_LIBS = $(CODE_COVERAGE_LIBS)
CLI_CFLAGS = $(CODE_COVERAGE_CFLAGS)