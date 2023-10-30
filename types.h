#ifndef TYPES_H
#define TYPES_H
#include "config.h"
#include <libgccjit.h>
#include "mystring.h"

typedef struct ast_node_s ast_node_t;
typedef struct value_s value_t;
typedef struct symbol_s symbol_t;
typedef struct
{
  unsigned int row;
  unsigned int column;
  double **matrix;
} matrix_t;

typedef struct
{
  char *file;
  unsigned start;
  unsigned end;
} coord_t;

typedef struct
{
  int op; // operator code: t_name, t_fun, t_matrix
} type_t;

typedef struct func_s
{
  unsigned argc;
  type_t ret; // return type, set to -1 for not return
  symbol_t **argv;
  ast_node_t *body;
} func_t;

struct value_s
{
  type_t type;
  union
  {
    double num;
    func_t *fun;
    matrix_t *matrix;
  };
  union
  {
    gcc_jit_lvalue *lval;
    gcc_jit_function *fun;
    gcc_jit_rvalue* rval;
  } jit;
};

struct symbol_s
{
  char *name;
  type_t type;
  int scope;
  coord_t src;
  value_t *value;

  struct symbol_s *next, *prev;
};

typedef struct table_s table_t;

struct table_s
{
  int level;
  table_t *previous;
  symbol_t *head;      // symbol in this scope
  symbol_t **head_arr; // all symbols
};

typedef struct var_list_s
{
  size_t size;	  // allocated
  unsigned count; // index count
  symbol_t **variables;
} var_list_t;

typedef struct actuals_list_s
{
  size_t size;
  unsigned count;
  ast_node_t **actuals;
} actuals_list_t;

struct ast_node_s
{
  int type; // from parser.y
  union
  {
    double value;
    symbol_t *variable;
    str_t *string;
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
      size_t count; // current stmt_arr index
      ast_node_t **stmt_arr;
    } statements; // { ... }
    struct
    {
      ast_node_t *condition;
      ast_node_t *statements;
    } whilestmt;
    struct
    {
      ast_node_t *condition;
      ast_node_t *if_body;
      ast_node_t *else_body;
    } ifstmt;
    struct
    {
      symbol_t *body;
      actuals_list_t *argv;
      value_t *ret;
    } funCall;
    struct
    {
      symbol_t *body;
    } funDecl;
    struct
    {
      char *format;
      ast_node_t *print_avg;
    } printf;
    struct
    {
      ast_node_t *ret_exp;
    } return_exp;
  } data;

  gcc_jit_rvalue *jit_literal;
};

#endif