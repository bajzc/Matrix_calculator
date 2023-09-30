#ifndef SRC_AST_H
#define SRC_AST_H
#include "config.h"
#include "mystring.h"
#include "syms.h"
#include "stdbool.h"
enum node_type
{
  exp_t = 2 << 8, // 512
  assig_t,	  // 513
  stmt_t,	  // 514
  stmts_t,	  // 515
  while_stmt_t,	  // 516
  compare_t,	  // 517
  postfix_t,	  // 518
};

typedef struct ast_node_s ast_node_t;

struct ast_node_s
{
  int type; // from parser.y
  char *name;
  union
  {
    double var;
    matrix_t *matrix;
    struct str_s *string;
    struct
    {
      ast_node_t *left, *right;
      int op;
    } expression;
    struct
    {
      symbol_t *name;
      ast_node_t *right; // exp
    } assignment;
    struct
    {
      int count; // current stmt_arr index
      ast_node_t **stmt_arr;
    } statements; // { ... }
    struct
    {
      ast_node_t *condition;
      ast_node_t *statements;
    } whilestmt, ifstmt;
    struct
    {
      symbol_t *name;
      ast_node_t *argv;
    } funcall;
    struct
    {
      char *format;
      ast_node_t *print_avg;
    } printf;
  } data;
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
#endif