#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "myprintf.h"
#include "mem_pool.h"
#include "ast.h"
extern mem_pool_t *POOL;

// inline optimize will be enabled with -O2
int
print_d (int count, ast_node_t **print_ast_avg)
{
  double num;
  if (IS_EXP (print_ast_avg[count]) || IS_NAME (print_ast_avg[count])
      || IS_NUM (print_ast_avg[count]))
    num = ast_exec_exp (print_ast_avg[count]);
  else
    return fprintf (
      stderr, "format specifies type '%%d' does not match the argument type\n");
  if (isnormal (num))
    return printf ("%d", (int) num);
  else
    return printf ("%lf", num);
  return -1;
}

int
print_s (int count, ast_node_t **print_ast_avg)
{
  if (IS_STRING (print_ast_avg[count]))
    return printf ("%.*s",
		   (int) strlen (print_ast_avg[count]->data.string->data) - 2,
		   print_ast_avg[count]->data.string->data + 1);
  fprintf (stderr,
	   "format specifies type '%%s' does not match the argument type\n");
  return -1;
}

int
print_lf (int count, ast_node_t **print_ast_avg)
{
  double num;
  if (IS_EXP (print_ast_avg[count]) || IS_NAME (print_ast_avg[count])
      || IS_NUM (print_ast_avg[count]))
    num = ast_exec_exp (print_ast_avg[count]);
  else
    return fprintf (
      stderr, "format specifies type '%%f' does not match the argument type\n");
  return printf ("%lf", num);
}

int
print_special_char (char op)
{
  switch (op)
    {
    case 'n':
      printf ("\n");
      break;
    case 't':
      printf ("\t");
      break;
    default:
      assert (0);
    }
  return 0;
}

int
print_ast_avg (char *format, ast_node_t *print_avg)
{
  char *p;
  int count = 0;
  ast_node_t **stmt_arr = print_avg->data.statements.stmt_arr;
  for (p = format + 1; p <= format + strlen (format) - 2; p++)
    {
      if (*p == '%')
	{
	  p++;
	  switch (*p)
	    {
	    case 'd':
	      print_d (count, stmt_arr);
	      break;
	    case 's':
	      print_s (count, stmt_arr);
	      break;
	    case 'l':
	      if (*(p + 1) == 'f')
		{
		  print_lf (count, stmt_arr);
		  p++;
		}
	      else
		assert (0);
	      break;
	    default:
	      puts ("not supported format code");
	      assert (0);
	    }
	  count++;
	}
      else if (*p == '\\')
	{
	  p++;
	  print_special_char (*p);
	}
      else
	printf ("%c", *p);
    }
  return 0;
}

char *
conver_format (const char *src)
{
  assert (src);
  char *buffer = mem_malloc (POOL, strlen (src));
  size_t index = 0;

  for (const char *p = src; *p != '\0'; p++)
    {
      if (*p == '\\')
	{
	  p++;
	  switch (*p)
	    {
	    case 'n':
	      buffer[index] = '\n';
	      break;
	    case 't':
	      buffer[index] = '\t';
	      break;
	    case 'f':
	      buffer[index] = '\f';
	      break;
	    case 'r':
	      buffer[index] = '\r';
	    }
	}
      else
	{
	  buffer[index] = *p;
	}
      index++;
    }
  return buffer;
}