#!/bin/sh
set -ex
rm -rf autom4te.cache Makefile.in aclocal.m4
aclocal
autoconf -W all
automake -W all  --add-missing
rm -rf autom4te.cache
exit 0
