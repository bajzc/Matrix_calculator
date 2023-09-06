//
// Created by LiZeCheng-Jason on 2023-06-25.
//
#include "config.h"

extern int
yy_scan_buffer (char *base, size_t size);
extern int
yy_scan_string (const char *yy_str);
extern int
yyparse (void);

void
print_help_msg (void)
{
  printf ("%.80s\n", PACKAGE_STRING);
  printf ("License: GPL-3.0 Author: bajzc %s\n", PACKAGE_BUGREPORT);
  printf ("Math Functions:\n");
  printf ("%-40s", "inv(matrix)");
  printf ("%-40s\n", "Find the inverse of matrix");
  printf ("%-40s", "det(matrix)");
  printf ("%-40s\n", "The determinant of matrix");
  printf("\nThe developing document is in doc/MatCal.pdf\n\n");
}
void
print_ver_msg (void)
{
  printf ("%.80s\n", PACKAGE_STRING);
  printf ("Copyright (C) 2023 Li Zecheng.\n");
  printf (
    "This program is free software: you can redistribute it and/or modify it "
    "under \n"
    "the terms of the GNU General Public License as published by the Free "
    "Software \n"
    "Foundation, either version 3 of the License, or (at your option) any "
    "later version.\n"
    "This program is distributed in the hope that it will be useful, but "
    "WITHOUT ANY \n"
    "WARRANTY; without even the implied warranty of MERCHANTABILITY or "
    "FITNESS \n"
    "FOR A PARTICULAR PURPOSE. See the GNU General Public License for more "
    "details.\n"
    "You should have received a copy of the GNU General Public License along "
    "with \n"
    "this program. If not, see <https://www.gnu.org/licenses/>.\n\n");
}

COMMAND commands[]
  = {{"inv", "1 / matrix"},
     {"det", "The determinant of matrix"},
     {"list", "Show all initialized matrix"},
     {"clean", "Delete all matrix stored in memory! (ans will remain)"},
     {"quit", "quit this program"},
     {"exit", "same as above"},
     {"help", "print help message"},
     {(char *) NULL, (char *) NULL}}; // readline completion commands

char *
dupstr (char *s)
{
  char *r;

  r = malloc (strlen (s) + 1);
  strcpy (r, s);
  return r;
}

char **
cal_completion (const char *text, int start, int end)
{
  char **matches = NULL;

  rl_attempted_completion_over = 1; // disable default completion

  int list_index = 0;
  char *name;

  while ((name = commands[list_index].name))
    { // calng warning
      if (strncmp (name, text, 3) == 0)
	break;
      list_index++;
    }

  if (start == 0)
    {
      if (list_index < 5)
	{
	  matches = rl_completion_matches (text, var_generator);
	}
      matches = rl_completion_matches (text, command_generator);
    }
  return matches;
}

char *
var_generator (const char *text, int state)
{
  // TODO list all variable

  return NULL;
}

char *
command_generator (const char *text, int state)
{
  static int list_index, len;
  char *name;

  if (!state)
    {
      list_index = 0;
      len = strlen (text);
    }

  while ((name = commands[list_index].name))
    { // calng warning
      // TODO functions should follow by '('
      rl_completion_suppress_append
	= 1; // "not appended to matches at the end of the command line"
      list_index++;
      if (strncmp (name, text, len) == 0)
	return (dupstr (name));
    }

  return NULL;
}

int
cli (int opt)
{
  if (opt == 'f')
    {
      // extern FILE* ifp;
      // fclose(std);
      // stdin = ifp;
      if (!freopen (optarg, "r", stdin))
	{
	  fprintf (stderr, "freopen stdin to %s error", optarg);
	  exit (-1);
	}
      extern FILE *ifp;
      size_t input_len;
      char *input = NULL;
      // FIXME if the last line of file does not contain '\n', add it!!!
      while (getline (&input, &input_len, ifp) != -1)
	{
	  add_history (input);
	  yy_scan_string (input);
	  yyparse ();
	}
      fclose (stdin);
      exit (1);
    }

  // normal mode
  char *input = readline (">>> ");
  if (!input)
    {
      cli (0);
      return 0;
    }
  add_history (input);
  char *input_newline = malloc (sizeof (char) * strlen (input) + 2);
  strcpy (input_newline, input);
  input_newline[strlen (input)] = '\n';
  input_newline[strlen (input) + 1] = '\0';
  yy_scan_string (input_newline); // ignore this warning
  free (input);
  free (input_newline);
  return yyparse ();
}
