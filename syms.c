#include "syms.h"
#include "parser.h"
#include "types.h"
#include <assert.h>
#include <inttypes.h>
#include <linux/limits.h>
#include <stdlib.h>
#include <string.h>

extern int level;
int level = GLOBAL;
symbol_t *sym_table = NULL;

static table_t cns = {CONSTANTS}, glb = {GLOBAL},
	       ids = {GLOBAL}; // init table_t.level

table_t *constants = &cns;
table_t *global = &glb;
table_t *identifiers = &ids;

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
  ++level;
  printf ("current scope: %d\n", level);
}

void
exit_scope ()
{
  if (identifiers->level == level)
    {
      identifiers = identifiers->previous;
    }
  --level;

  printf ("current scope: %d\n", level);
}

void
init_base_tables ()
{
  identifiers->head = malloc (sizeof (symbol_t));
  init_sym_head (identifiers->head);
  identifiers->previous = NULL;
}

void
flex_putsym (char *name)
{
  if (!getsym (name, identifiers))
    {
      putsym (name, &identifiers, level);
    }
}

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// use &identifier to call this function
// Otherwise, the constant ids will be changed as we used &ids
// to create identifier
symbol_t *
putsym (char *name, table_t **tpp, int level)
{
  symbol_t *p;
  table_t *tp = *tpp; // No reason, this worth one hour

  if (level > 0 && tp->level < level)
    {
      tp = *tpp = new_table (tp, level);
    }
  p = malloc (sizeof (symbol_t));
  p->name = strdup (name);
  p->scope = level;
  p->type.op = 0;
  p->value = malloc (sizeof (value_t));
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
	      printf ("%s: found in level:%d\n",name, tp->level);
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

  new = malloc (sizeof (table_t));
  new->previous = up; // this should not change ids
  new->level = level;
  new->head = malloc (sizeof (symbol_t));
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
  var_list_t *new = malloc (sizeof (var_list_t));
  new->variables = malloc (sizeof (symbol_t *) * 4);
  new->count = 0;
  new->size = 4;
  new->variables[0] = malloc (sizeof (value_t));
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
	= realloc (list->variables, sizeof (symbol_t) * (list->size + 2));
      assert (temp);
      list->variables = temp;
      list->size += 2;
    }
  list->count++;
  list->variables[list->count]->type.op = t_identifier;
  list->variables[list->count]->name = name;
  return list;
}