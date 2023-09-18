#ifndef SRC_CLI_H
#define SRC_CLI_H

#include "system.h"
typedef struct
{
  char *name;
  char *doc;
} COMMAND; // readline completion

void
print_help_msg (void);
void
print_ver_msg (void);
char *
dupstr (char *s);
char **
cal_completion (const char *text, int start, int end);
char *
command_generator (const char *text, int state);
char *
var_generator (const char *text, int state);
int
cli_file (void);
int
cli (void);
#ifndef HAVE_GETLINE
# include <errno.h>
  size_t getline(char **lineptr, size_t *n, FILE *stream);
#endif

#endif