#include <config.h>
#include "syms.h"
#include "cli.h"
#include "system.h"
#include "parser.h"

extern void
yyerror (char const *);
symvar *sym_table = NULL; // header

int
matrix_check_coord (matrix_t *matrix, double x, double y)
{
  printf ("matrix: row:%u\tcol:%u\n", matrix->row, matrix->column);
  if (ceil (x) != x || ceil (y) != y)
    {
      puts ("Matrix index is not an integer");
      return -1;
    }
  if (x > matrix->row || y > matrix->column || x < 0 || y < 0)
    {
      puts ("Matrix index out of range");
      return -1;
    }
  return 0;
}

double
matrix_edit_entity (matrix_t *matrix, double x, double y, double num)
{
  if (matrix_check_coord (matrix, x, y))
    return -1;
  unsigned x1 = (unsigned) (x) -1;
  unsigned y1 = (unsigned) (y) -1;
  printf ("%g -> ", matrix->matrix[x1][y1]);
  matrix->matrix[x1][y1] = num;
  printf ("%g\n", matrix->matrix[x1][y1]);
  return num;
}

double
matrix_get_entity (matrix_t *matrix, double x, double y)
{
  if (matrix_check_coord (matrix, x, y))
    return NAN;
  else
    return matrix->matrix[(size_t) x - 1][(size_t) y - 1];
}

matrix_t *
matrix_temp (void)
{
  matrix_t *temp = malloc (sizeof (matrix_t));
  if (!temp)
    {
      puts ("matrix_temp malloc error!");
      exit (-1);
    }
  temp->column = 1;
  temp->row = 1;
  matrix_calloc (temp);
  return temp;
}

matrix_t *
matrix_init (vector_t *vector)
{
  if (!vector_check (vector))
    {
      yyerror ("Abort");
      return (matrix_t *) NULL;
    }
  matrix_t *matrix = malloc (sizeof (matrix_t));
  matrix->column = vector->arrays[0]->count + 1;
  matrix->row = vector->count + 1;
  matrix_malloc (matrix);
  for (int i = 0; i < matrix->row; i++)
    {
      memcpy (matrix->matrix[i], vector->arrays[i]->num,
	      sizeof (double) * matrix->column);
    }
  for (int i = 0; i < matrix->row; i++)
    {
      for (int j = 0; j < matrix->column; j++)
	{
	  matrix->matrix[i][j] = vector->arrays[i]->num[j];
	}
    }
  return matrix;
}

bool
vector_check (vector_t *vector)
{			  // check if all arrays have the same size
  if (vector->count == 0) // only one array in vector
    return true;
  int col = vector->arrays[0]->count;
  for (int i = 1; i <= vector->count; i++)
    {
      if (col != vector->arrays[i]->count)
	{
	  printf ("ERROR: vertical dimensions mismatch (1x%d vs 1x%d)\n", col,
		  vector->arrays[i]->count);
	  return false;
	}
    }
  return true;
}

vector_t *
vector_init (array_t *arr)
{
  vector_t *vector = malloc (sizeof (vector_t));
  if (!vector)
    {
      puts ("vector malloc error");
      exit (-1);
    }
  vector->size = 4;
  vector->arrays = (array_t **) malloc (4 * sizeof (array_t *));
  vector->arrays[0] = malloc (sizeof (array_t));
  vector->arrays[0] = arr;
  vector->count = 0;
  return vector;
}

vector_t *
vector_init_nums (double num1, double num2)
{
  array_t *arr1 = array_init (num1, 0);
  array_t *arr2 = array_init (num2, 0);
  arr1->count = 0;
  arr2->count = 0;
  vector_t *vector = vector_init (arr1);
  vector_add (vector, arr2);
  return vector;
}

vector_t *
vector_add (vector_t *vector, array_t *arr)
{
  if (!vector)
    {
      puts ("vector malloc error");
      exit (-1);
    }
  if (vector->count + 1 == vector->size)
    {
      array_t **temp
	= realloc (vector->arrays, sizeof (array_t *) * (vector->size + 4));
      if (!temp)
	{
	  puts ("vector realloc error");
	  exit (-1);
	}
      else
	{
	  vector->arrays = temp;
	}
      vector->size += 4;
    }
  vector->count++;
  vector->arrays[vector->count] = malloc (sizeof (array_t));
  vector->arrays[vector->count] = arr;
  return vector;
}

array_t *
array_init (double num1, double num2)
{
  array_t *array = malloc (sizeof (array_t));
  if (!array)
    {
      printf ("array malloc error");
      exit (-1);
    }
  array->num = malloc (16 * sizeof (double));
  array->num[0] = num1;
  array->num[1] = num2;
  array->size = 16;
  array->count = 1;
  return array;
}

array_t *
array_add (array_t *array, double num1)
{
  if (!array)
    {
      puts ("array malloc error");
      exit (-1);
    }
  if (array->count + 1 == array->size)
    {
      double *temp = realloc (array->num, sizeof (double) * (array->size + 16));
      if (!temp)
	{
	  puts ("array realloc error");
	  exit (-1);
	}
      else
	{
	  array->num = temp;
	}
      array->size += 16;
    }
  array->count += 1;
  array->num[array->count] = num1;
  return array;
}

struct init const funs[] = {
  {"atan", .value.fun = atan, 1},
  {"cos", .value.fun = cos, 1},
  {"tan", .value.fun = tan, 1},
  {"asin", .value.fun = asin, 1},
  {"acos", .value.fun = acos, 1},
  {"atan", .value.fun = atan, 1},
  {"exp", .value.fun = exp, 1},
  {"ln", .value.fun = log, 1},
  {"sin", .value.fun = sin, 1},
  {"sqrt", .value.fun = sqrt, 1},
  {0, 0, 0},
};

void
init_table (void)
{
  sym_table = malloc (sizeof (symvar));
  init_sym_head (sym_table);
  for (int i = 0; funs[i].name; i++)
    {
      symvar *ptr = putsym (funs[i].name, t_fun);
      ptr->value.fun = funs[i].value.fun;
    }
  symvar *ptr = putsym ("pi", t_name);
  ptr->value.var = M_PI;
  ptr = putsym ("e", t_name);
  ptr->value.var = M_E;
}

symvar *
putsym (char const *name, int sym_type)
{
  symvar *res = (symvar *) malloc (sizeof (symvar));
  res->name = strdup (name);
  res->type = sym_type;
  res->value.var = NAN;
  add_sym_node (res, sym_table);
  return res;
}

symvar *
getsym (char const *name)
{
  symvar *p;
  for (p = sym_table->next; p != sym_table; p = p->next)
    if (strcmp (p->name, name) == 0)
      return p;
  return NULL;
}

void
print_symtab (void)
{
  int found = 0; // flag for first found
  for (symvar *p = sym_table->next; p != sym_table; p = p->next)
    {
      if (p->type == t_name)
	{
	  if (!found)
	    {
	      puts ("--- Variable ---");
	      found = 1;
	    }
	  printf ("%s: %f\n", p->name, p->value.var);
	}
    }

  found = 0;
  for (symvar *p = sym_table->next; p != sym_table; p = p->next)
    {
      if (p->type == t_matrix)
	{
	  if (!found)
	    {
	      puts ("\n---- Matrix ----");
	      found = 1;
	    }
	  printf ("%s: %u by %u\n", p->name, p->value.matrix->row,
		  p->value.matrix->column);
	  matrix_t_print (p->value.matrix);
	}
    }
  found = 0;
  for (symvar *p = sym_table->next; p != sym_table; p = p->next)
    {
      if (p->type == t_fun)
	{
	  if (!found)
	    {
	      puts ("\n--- Function ---");
	      found = 1;
	    }
	  printf ("%s\t", p->name);
	}
    }
  puts ("");
}

void
clean_symtab (void)
{
  while (1)
    {
      printf ("Proceed with \"clean\"? [y/N]: ");
      char ch = getchar ();
      if (ch == 'Y' || ch == 'y')
	break;
      else if (ch == 'n' || ch == 'N' || ch == '\n')
	return;
    }
  free (sym_table);
  sym_table = NULL;
  init_table ();
}

// Circular doubly linked list
// from <linux/list.h>
void
init_sym_head (symvar *head)
    {
  head->next = head;
  head->prev = head;
    }

void
add_sym_node (symvar *new, symvar *head)
{
  symvar *prev = head;
  symvar *next = head->next;

  next->prev = new;
  new->next = next;
  new->prev = prev;
  prev->next = new;
}

void
del_sym_entry (symvar *entry)
{
  symvar *prev = entry->prev;
  symvar *next = entry->next;

  next->prev = prev;
  prev->next = next;
}