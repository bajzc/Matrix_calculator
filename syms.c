#include "syms.h"
symbol_t *sym_table=NULL;

char *
dupstr (char *s)
{
  char *r;

  r = malloc (strlen (s) + 1);
  strcpy (r, s);
  return r;
}

symbol_t *
putsym (char const *name, int sym_type)
{
  symbol_t *res = (symbol_t *) malloc (sizeof (symbol_t));
  res->name = strdup (name);
  res->type.op = sym_type;
  res->value=malloc(sizeof(value_t));
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