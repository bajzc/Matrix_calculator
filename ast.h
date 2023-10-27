#ifndef SRC_AST_H
#define SRC_AST_H
#include "syms.h"
#include "stdbool.h"
#include "types.h"

#define IS_EXP(node) node->type == exp_t
#define IS_NUM(node) node->type == t_num
#define IS_NAME(node) node->type == t_identifier
#define IS_STRING(node) node->type == t_string

enum node_type
{
  exp_t = 2 << 8, // 512
  assig_t,	  // 513
  stmt_t,	  // 514
  stmts_t,	  // 515
  while_stmt_t,	  // 516
  compare_t,	  // 517
  postfix_t,	  // 518
  funcDecl_t,	  // 519
  funCall_t,	  // 520
  return_exp_t,	  // 521 expression
  return_val_t,	  // 522 value
  if_stmt_t,	  // 523
};

ast_node_t *
make_expression (ast_node_t *left, char op, ast_node_t *right);
ast_node_t *
make_assignment (char *name, ast_node_t *right);
ast_node_t *
make_name (char *name);
ast_node_t *
make_var (double num);
int
ast_exec (ast_node_t *root);
double
ast_exec_op (double left, char op, double right);
double
ast_exec_exp (ast_node_t *root);
ast_node_t *
make_while (ast_node_t *condition, ast_node_t *statements);
ast_node_t *
make_statement (ast_node_t *statements, ast_node_t *stmt);
ast_node_t *
make_compare (ast_node_t *right, int op, ast_node_t *left);
bool
ast_exec_compare (double left, int op, double right);
double
ast_exec_postfix (ast_node_t *root);
ast_node_t *
make_postfix_exp (ast_node_t *lval, int op);
ast_node_t *
make_print_list (ast_node_t *print_list, ast_node_t *print_element);
ast_node_t *
make_printf (char *format, ast_node_t *print_list);
ast_node_t *
make_string (struct str_s *string);
int
is_constant (ast_node_t *root);
ast_node_t *
make_function_decl (char *name, var_list_t *argv, ast_node_t *stmt);
ast_node_t *
make_function_decl_void (char *name, var_list_t *argv, ast_node_t *stmt);
ast_node_t *
make_function_call (char *name, actuals_list_t *list);
ast_node_t *
make_return (ast_node_t *ret_exp);
value_t *
ast_exec_funCall (ast_node_t *root);
ast_node_t *
make_return_exp (ast_node_t *funCall);
ast_node_t *
make_if (ast_node_t *condition, ast_node_t *if_body, ast_node_t *else_body);
#endif