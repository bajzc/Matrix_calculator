#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.


AC_PREREQ([2.70])
AC_INIT([Matrix Calculator], [dev/lex&bison-0.1], [lzecheng98@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_CONFIG_SRCDIR([src/lex.l])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile
		 src/Makefile])
PKG_PROG_PKG_CONFIG([0.2])

# Checks for programs.
AC_PROG_CC([gcc clang])
AC_PROG_LEX(yywrap)

AC_SUBST(YACC,'bison')
# AC_PROG_YACC
# We want don't use this, because it will issue 'bison -y'
# but we need the extension of GNU/bison
# So add YACC=bison when running ./configure

# Checks for libraries.
AC_CHECK_LIB([readline],[readline],[],[echo "READLINE LIBRARY NOT FOUND"&& exit])

# Checks for header files.
AC_CHECK_HEADERS([locale.h],[HAVE_LOCALE_H=1],[echo "locale.h not found"])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([pow strdup])

AC_OUTPUT
