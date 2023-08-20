#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.71])
AC_INIT([Matrix-calculator], [1.0], [lzecheng98@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_SRCDIR([src/errorf.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile
		 src/Makefile])
PKG_PROG_PKG_CONFIG([0.2])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([pcre2-8],[pcre2_match_8],[],[echo "PCRE2 LIBRARY NOT FOUND"&& exit])
AC_CHECK_LIB([readline],[readline],[],[echo "READLINE LIBRARY NOT FOUND"&& exit])

# Checks for header files.
AC_CHECK_HEADERS([uthash.h],[HAVE_UTHASH_H=1],[echo "\"uthash.h\" NOT FOUND (https://troydhanson.github.io/uthash/utlist.html)"&&exit])
AC_CHECK_HEADERS([locale.h],[HAVE_LOCALE_H=1],[echo "locale.h not found"])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_STRTOD

AC_OUTPUT
