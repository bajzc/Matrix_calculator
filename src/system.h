#ifndef SRC_SYSTEM_H
#define SRC_SYSTEM_H

#if defined __WIN32__
#  define CLEAR "cls"
# else
#   define CLEAR "clear"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <math.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <unistd.h>

#endif