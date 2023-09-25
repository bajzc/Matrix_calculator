#ifndef SYMS_H
#define SYMS_H
#include "config.h"
#include "parser.h"

extern int level;

enum Scope
{
  CONSTANTS = 1,
  GLOBAL,
  PARAM,
  LOCAL,
};

typedef union value_s value_t;

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
  value_t *argv;
} func_t;

union value_s
{
  double var;
  func_t *fun; // return value
  matrix_t *matrix;
}; // right value of a symbol
typedef struct symbol_s symbol_t;

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
  symbol_t *head;
};

symbol_t *
putsym (char const *name, int sym_type);
symbol_t *
getsym (char const *name);

#endif