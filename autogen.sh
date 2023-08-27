#!/bin/sh
set -ex
rm -rf autom4te.cache Makefile.in aclocal.m4
aclocal
autoreconf --install
rm -rf autom4te.cache
exit 0
