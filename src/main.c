//
// Created by LiZeCheng-Jason on 2023-06-28.
//
#include <config.h>
#include "system.h"
#include "cli.h"
#include "syms.h"

extern char *optarg;
FILE *ifp = NULL;
int opt = 256;

int
main (int argc, char **argv)
{
  init_table ();

#if HAVE_LOCALE_H
# include <locale.h>
  (void) setlocale (LC_ALL, "");
#endif

  rl_readline_name = PACKAGE_NAME;
  rl_attempted_completion_function = cal_completion;
  rl_bind_key ('\t', rl_complete);
  using_history ();
  // readline library init

  bool exit_flag = 0;
  while ((opt = getopt (argc, argv, "f:hv")) != EOF)
    {
      switch (opt)
	{
	case 'f':
	  puts (optarg);
	  if ((ifp = fopen (optarg, "r")) == NULL)
	    {
	      fprintf (stderr, "ERROR: cannot open file: '%s'\n", optarg);
	      exit(-1);
	    }
	  cli_file ();
	  break;
	case 'h':
	  print_help_msg ();
	  exit_flag = 1;
	  break;
	case 'v':
	  print_ver_msg ();
	  exit_flag = 1;
	  break;
	}
    }

  // ./CLI -hv OR ./CLI -vh
  if (exit_flag)
    return 0;

  // default model
  while (1)
    cli ();
  return 0;
}