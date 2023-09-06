//
// Created by LiZeCheng-Jason on 2023-06-25.
//
#include "config.h"
void
error_exit (char *errormsg)
{
  fprintf (stderr, "\n\033[31m%s\033[0m\n", errormsg);
  fprintf (stderr, "Press any key to exit...");
  setbuf (stdin, NULL);
  (void) getchar ();
  exit (-1);
}
void
error_print (char *errormsg)
{
  fprintf (stderr, "\033[31m%s\033[0m\n", errormsg);
  setbuf (stdin, NULL);
}
