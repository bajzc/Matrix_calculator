#include "syms.h"
#include "ast.h"
#include "types.h"
#include "mem_pool.h"
#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

extern mem_pool_t *POOL;
extern int level;
int level = GLOBAL;
symbol_t *sym_table = NULL;

static table_t cns = {CONSTANTS, NULL, NULL, NULL},
	       glb = {GLOBAL, NULL, NULL, NULL},
	       ids = {GLOBAL, NULL, NULL, NULL},
	       fundef = {GLOBAL, NULL, NULL, NULL}; // init table_t.level

table_t *constants = &cns;
table_t *global = &glb;
table_t *identifiers = &ids;
table_t *funcdef = &fundef;

void
init_sym_head (symbol_t *head)
{
  head->next = head;
  head->prev = head;
}

void
add_sym_node (symbol_t *new, symbol_t *head)
{
  symbol_t *prev = head;
  symbol_t *next = head->next;

  next->prev = new;
  new->next = next;
  new->prev = prev;
  prev->next = new;
}

void
del_sym_entry (symbol_t *entry)
{
  symbol_t *prev = entry->prev;
  symbol_t *next = entry->next;

  next->prev = prev;
  prev->next = next;
}

void
enter_scope ()
{
  // for a while loop based on the GLOBAl scope
  // skip PARAM and enter LOCAL scope
  if (level >= LOCAL)
    level++;
  else
    level = LOCAL;
  printf ("current level: %d\n", level);
}

void
exit_scope ()
{
  if (level == LOCAL)
    {
      if (identifiers->level == PARAM)
	identifiers = identifiers->previous;
      else if (identifiers->level == LOCAL)
	{
	  if (identifiers->previous->level < PARAM)
	    identifiers = identifiers->previous;
	  else
	    identifiers = identifiers->previous->previous;
	}
      level--;
    }
  else
    {
      if (identifiers->level == level)
	identifiers = identifiers->previous;
    }
  --level;
  printf ("current level: %d\n", level);
}

void
init_base_tables ()
{
  identifiers->head = mem_malloc (POOL, sizeof (symbol_t));
  init_sym_head (identifiers->head);
  identifiers->previous = NULL;
  global->head = mem_malloc (POOL, sizeof (symbol_t));
  init_sym_head (global->head);
  global->previous = NULL;
  funcdef->head = mem_malloc (POOL, sizeof (symbol_t));
  init_sym_head (funcdef->head);
}

// void
// flex_putsym (char *name)
// {
//   if (!getsym (name, identifiers))
//     {
//       putsym (name, &identifiers, level);
//     }
// }

/*
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * use &identifier to call this function
 * otherwise, the constant ids will be changed as we used &ids
 * to create identifier
 */
symbol_t *
putsym (char *name, table_t **tpp, int level)
{
  symbol_t *p;
  table_t *tp = *tpp;

  if (level > 0 && tp->level < level)
    {
      tp = *tpp = new_table (tp, level);
    }
  p = mem_malloc (POOL, sizeof (symbol_t));
  p->name = my_strdup (name);
  p->scope = level;
  p->type.op = 0;
  p->value = mem_malloc (POOL, sizeof (value_t));
  p->value->num = NAN;
  add_sym_node (p, tp->head);
  return p;
}

symbol_t *
getsym (char *name, table_t *tp)
{
  symbol_t *p;
  do
    {
      for (p = tp->head->next; p != tp->head; p = p->next)
	{
	  if (strcmp (p->name, name) == 0)
	    {
	      printf ("found %s in %d (%p)\n", name, tp->level, p);
	      return p;
	    }
	}
  } while ((tp = tp->previous) != NULL);
  return NULL;
}

table_t *
new_table (table_t *up, int level)
{
  table_t *new;

  new = mem_malloc (POOL, sizeof (table_t));
  new->previous = up; // this should not change ids
  new->level = level;
  new->head = mem_malloc (POOL, sizeof (symbol_t));
  init_sym_head (new->head);
  if (up)
    new->head_arr = up->head_arr;
  else
    new->head_arr = NULL;
  return new;
}

var_list_t *
new_variable_list (char *name)
{
  var_list_t *new = mem_malloc (POOL, sizeof (var_list_t));
  new->variables = mem_malloc (POOL, sizeof (symbol_t *) * 4);
  new->count = 0;
  new->size = 4;
  new->count--;
  return add_var_2_list (new, name);
}

var_list_t *
add_var_2_list (var_list_t *list, char *name)
{
  assert (list);
  if (list->count + 1 >= list->size)
    {
      symbol_t **temp
	= realloc (list->variables,
		   sizeof (symbol_t)
		     * (list->size + 2)); // increase array size by 2 each time
      assert (temp);
      list->variables = temp;
      list->size += 2;
    }
  list->count++;
  list->variables[list->count] = mem_malloc (POOL, sizeof (symbol_t));
  list->variables[list->count]->name = name;
  list->variables[list->count]->value = mem_malloc (POOL, sizeof (symbol_t));
  return list;
}

actuals_list_t *
new_actuals_list (ast_node_t *exp)
{
  assert (IS_EXP (exp) || IS_NUM (exp) || IS_NAME (exp));
  actuals_list_t *new = mem_malloc (POOL, sizeof (actuals_list_t));
  new->actuals = mem_malloc (POOL, sizeof (ast_node_t *) * 4);
  new->count = 0;
  new->size = 4;
  new->count--;
  return add_exp_2_list (new, exp);
}

actuals_list_t *
add_exp_2_list (actuals_list_t *list, ast_node_t *exp)
{
  assert (list);
  if (list->count + 1 >= list->size)
    {
      ast_node_t **temp
	= realloc (list->actuals, sizeof (ast_node_t) * (list->size + 2));
      assert (temp);
      list->actuals = temp;
      list->size += 2;
    }
  list->count++;
  list->actuals[list->count] = exp;
  return list;
}

void
install_parameter (var_list_t *list)
{
  assert (identifiers->level == 2);
  if (list == NULL)
    return;
  symbol_t *p = NULL;
  for (unsigned i = 0; i <= list->count; i++)
    {
      p = putsym (list->variables[i]->name, &identifiers, PARAM);
      p->type.op = list->variables[i]->type.op;
      p->value = list->variables[i]->value;
      p->src = list->variables[i]->src;
    }
}

char *
my_strdup (char *src)
{
  char *re = NULL;
  size_t len = 0;
  while (src[len])
    len++;
  re = mem_malloc (POOL, len + 1);
  memcpy (re, src, len + 1);
  return re;
}