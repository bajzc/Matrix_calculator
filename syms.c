#include "syms.h"
#include <assert.h>
#include <inttypes.h>
#include <linux/limits.h>

extern int level;
int level = GLOBAL;
symbol_t *sym_table = NULL;

static table_t cns = {CONSTANTS}, glb = {GLOBAL}, ids = {GLOBAL},
	       tys = {GLOBAL}; // init table_t.level

table_t *constants = &cns;
table_t *global = &glb;
table_t *identifiers = &ids;
table_t *types = &tys;

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

char *
dupstr (char *s)
{
  char *r;

  r = malloc (strlen (s) + 1);
  strcpy (r, s);
  return r;
}

void
enter_scope ()
{
  ++level;
  printf ("enter scope: %d\n", level);
}

void
exit_scope ()
{
  --level;
  if (types->level == level)
    types = types->previous;
  if (identifiers->level == level)
    identifiers = identifiers->previous;

  printf ("exit scope: %d\n", level);
}

#ifndef DEBUG_SYM
symbol_t *
putsym (char const *name, int sym_type)
{
  symbol_t *res = (symbol_t *) malloc (sizeof (symbol_t));
  res->name = strdup (name);
  res->type.op = sym_type;
  res->value = malloc (sizeof (value_t));
  res->value->var = NAN;
  res->next = sym_table;
  sym_table = res;
  return res;
}

symbol_t *
getsym (char const *name)
{
  for (symbol_t *p = sym_table; p; p = p->next)
    if (strcmp (p->name, name) == 0)
      return p;
  return NULL;
}

#else

symbol_t *
putsym (char *name, table_t *tp, int level)
{
  symbol_t *p;

  if (level > 0 && tp->level < level)
    {
      tp = new_table (tp, level);
    }
  p = malloc (sizeof (symbol_t));
  p->name = strdup (name);
  p->scope = level;
  p->type.op = 0;
  p->value = malloc (sizeof (value_t));
  p->value->var = NAN;
  add_sym_node (p, tp->head);
  return p;
}

symbol_t *
loopup (char *name, table_t *tp)
{
  symbol_t *p;
  do
    {
      for (p = tp->head->next; p != tp->head; p = p->next)
	{
	  if (strcmp (p->name, name) == 0)
	    return p;
	}
  } while ((tp = tp->previous) != NULL);
  return NULL;
}

table_t *
new_table (table_t *up, int level)
{
  table_t *new;

  new = malloc (sizeof (table_t));
  new->previous = up;
  new->level = level;
  init_sym_head (new->head);
  if (up)
    new->head_arr = up->head_arr;
  else
    new->head_arr = NULL;
  return new;
}

symbol_t *
new_constant (type_t *type, value_t *value)
{}

#endif