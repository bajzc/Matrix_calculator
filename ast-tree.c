#include "ast.h"
#include "parser.h"
#include "syms.h"
#include "types.h"
#include "mem_pool.h"
#include <stdbool.h>
#include <string.h>
#include <assert.h>

extern mem_pool_t *POOL;
extern int level;

int
is_constant (ast_node_t *root)
{
  switch (root->type)
    {
    case exp_t:
      return is_constant (root->data.expression.left)
	     && is_constant (root->data.expression.right);
    case t_identifier:
      return false;
    case compare_t:
      return is_constant (root->data.expression.left)
	     && is_constant (root->data.expression.left);
    case t_num:
      return true;
    default:
      assert (0);
    }
  return 0;
}

ast_node_t *
make_var (double num)
{
  ast_node_t *node = mem_malloc (POOL, sizeof (ast_node_t));
  node->type = t_num;
  node->data.value = num;
  return node;
}

ast_node_t *
make_name (char *name)
{
  ast_node_t *node = mem_malloc (POOL, sizeof (ast_node_t));
  node->type = t_identifier;
  if ((node->data.variable = getsym (name, identifiers)) == NULL)
    node->data.variable = putsym (name, &identifiers, level);
  return node;
}

ast_node_t *
make_expression (ast_node_t *left, char op, ast_node_t *right)
{
  ast_node_t *node = mem_malloc (POOL, sizeof (ast_node_t));
  node->type = exp_t;
  node->data.expression.left = left;
  node->data.expression.right = right;
  node->data.expression.op = op;
  return node;
}

ast_node_t *
make_assignment (char *name, ast_node_t *right)
{
  ast_node_t *node = mem_malloc (POOL, sizeof (ast_node_t));
  node->type = assig_t;
  node->data.assignment.name = getsym (name, identifiers);
  assert (node->data.assignment.name);
  node->data.assignment.right = right;
  return node;
}

ast_node_t *
make_statement (ast_node_t *StmtList, ast_node_t *stmt)
{
  if (StmtList == NULL)
    { // root is NULL, init statements
      StmtList = mem_malloc (POOL, sizeof (ast_node_t));
      StmtList->type = stmts_t;
      StmtList->data.statements.count = 0;
      StmtList->data.statements.stmt_arr
	= mem_malloc (POOL, sizeof (ast_node_t *));
      StmtList->data.statements.stmt_arr[0] = stmt;
      return StmtList;
    }
  StmtList->data.statements.count += 1;

  ast_node_t **new_lsit;
  //   = realloc (StmtList->data.statements.stmt_arr,
  //        sizeof (ast_node_t *) * (StmtList->data.statements.count + 1));
  new_lsit = mem_malloc (POOL, sizeof (ast_node_t *)
				 * (StmtList->data.statements.count + 1));
  memcpy (new_lsit, StmtList->data.statements.stmt_arr,
	  sizeof (ast_node_t *) * (StmtList->data.statements.count + 1));
  if (*new_lsit == NULL)
    {
      printf ("stmt_arr realloc error!\n");
      assert (0);
    }

  StmtList->data.statements.stmt_arr = new_lsit;
  StmtList->data.statements.stmt_arr[StmtList->data.statements.count] = stmt;
  return StmtList;
}

ast_node_t *
make_while (ast_node_t *condition, ast_node_t *statements)
{
  ast_node_t *node = mem_malloc (POOL, sizeof (ast_node_t));
  node->type = while_stmt_t;
  node->data.whilestmt.condition = condition;
  node->data.whilestmt.statements = statements;
  return node;
}

ast_node_t *
make_compare (ast_node_t *left, int op, ast_node_t *right)
{
  ast_node_t *node = mem_malloc (POOL, sizeof (ast_node_t));
  node->type = compare_t;
  node->data.expression.left = left;
  node->data.expression.right = right;
  node->data.expression.op = op;
  return node;
}

ast_node_t *
make_postfix_exp (ast_node_t *lval, int op)
{
  ast_node_t *node = mem_malloc (POOL, sizeof (ast_node_t));
  node->type = postfix_t;
  node->data.variable = lval->data.variable;
  node->data.expression.op = op;
  return node;
}

ast_node_t *
make_string (struct str_s *string)
{
  ast_node_t *node = mem_malloc (POOL, sizeof (ast_node_t));
  node->type = t_string;
  node->data.string = string;
  return node;
}

ast_node_t *
make_print_list (ast_node_t *print_list, ast_node_t *print_element)
{
  print_list = make_statement (print_list, print_element);
  return print_list;
}

ast_node_t *
make_printf (char *format, ast_node_t *print_list)
{
  ast_node_t *node = mem_malloc (POOL, sizeof (ast_node_t));
  node->type = t_printf;
  if (print_list == NULL)
    node->data.printf.print_avg = NULL;
  else
    node->data.printf.print_avg = print_list;
  node->data.printf.format = strdup (format);
  return node;
}

ast_node_t *
make_function_decl (char *name, var_list_t *argv, ast_node_t *stmt)
{
  ast_node_t *node = NULL;
  symbol_t *fun = NULL;
  if ((fun = getsym (name, global)) != NULL)
    {
      assert (fun->type.op != funcDecl_t);
    }
  fun = putsym (name, &funcdef, GLOBAL);
  fun->value = mem_malloc (POOL, sizeof (value_t));
  fun->type.op = fun->value->type.op = funcDecl_t;
  fun->value->fun = mem_malloc (POOL, sizeof (func_t));
  fun->value->fun->argc = argv->count + 1;
  fun->value->fun->ret.op = t_num;
  fun->value->fun->argv = argv->variables;
  fun->value->fun->body = stmt;

  node = mem_malloc (POOL, sizeof (ast_node_t));
  node->type = funcDecl_t;
  node->data.funDecl.body = fun;

  return node;
}

ast_node_t *
make_function_decl_void (char *name, var_list_t *argv, ast_node_t *stmt)
// TODO need to be verify
{
  ast_node_t *p = make_function_decl (name, argv, stmt);
  p->data.funDecl.body->value->fun->ret.op = -1;
  return p;
}

ast_node_t *
make_function_call (char *name, actuals_list_t *list)
{
  ast_node_t *node = mem_malloc (POOL, sizeof (ast_node_t));
  symbol_t *fun = getsym (name, funcdef);

  assert (fun);
  node->type = funCall_t;
  node->data.funCall.argv = list;
  node->data.funCall.body = fun;
  return node;
}