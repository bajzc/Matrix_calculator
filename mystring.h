#ifndef MY_STRING_H
#define MY_STRING_H
#include "stdlib.h"

typedef struct str_s
{
  size_t len;
  char *data;
}str_t;

#define new_string(str)                                                        \
  {                                                                            \
    sizeof (str) - 1, (char *) str                                             \
  }

#endif