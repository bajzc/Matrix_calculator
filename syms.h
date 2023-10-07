#ifndef SYMS_H
#define SYMS_H
#include "types.h"
#include "config.h"
#include "parser.h"
#include <stddef.h>

extern int level;
extern table_t *constants, *global, *identifiers;

enum
{
  CONSTANTS = 1,
  GLOBAL,
  PARAM,
  LOCAL,
};

void
enter_scope ();
void
exit_scope ();

table_t *
new_table (table_t *up, int level);
symbol_t *
getsym (char *name, table_t *tp);
symbol_t *
putsym (char *name, table_t **tpp, int level);
void
init_sym_head (symbol_t *head);
var_list_t *
add_var_2_list (var_list_t *list, char *node);
var_list_t *
new_variable_list (char *node);
void
init_base_tables ();
void
flex_putsym (char *name);
#endif