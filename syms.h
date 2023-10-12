#ifndef SYMS_H
#define SYMS_H
#include "types.h"
#include "parser.h"
#include <stddef.h>

extern int level;
extern table_t *constants, *global, *identifiers,*funcdef;

enum
{
  CONSTANTS = 1,
  GLOBAL = 2,
  PARAM = 3,
  LOCAL = 4,
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
actuals_list_t *
new_actuals_list (ast_node_t *exp);
actuals_list_t *
add_exp_2_list (actuals_list_t *list, ast_node_t *exp);
void
init_base_tables ();
void
flex_putsym (char *name);
void
install_parameter (var_list_t *list);
#endif