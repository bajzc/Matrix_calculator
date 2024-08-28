#include "ast.h"
#include "mem_pool.h"
#include "parser.h"
#include "syms.h"
#include "types.h"
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#ifdef ENABLE_JIT
#include <libgccjit.h>
#include "ast_jit.h"
#endif

extern mem_pool_t *POOL;
extern int level;
void
yyerror (char const *s);

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
#ifdef DENABLE_JIT
  node->jit_literal = jit_make_literal (&num, GCC_JIT_TYPE_DOUBLE);
#endif
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
make_if (ast_node_t *condition, ast_node_t *if_body, ast_node_t *else_body)
{
  assert (condition != NULL && if_body != NULL);

  ast_node_t *node = mem_malloc (POOL, sizeof (ast_node_t));
  node->type = if_stmt_t;
  node->data.ifstmt.condition = condition;
  node->data.ifstmt.if_body = if_body;
  node->data.ifstmt.else_body = else_body;
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

#ifdef ENABLE_JIT
  node->jit_literal
    = jit_make_literal (string->data, GCC_JIT_TYPE_CONST_CHAR_PTR);
#endif
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
  node->data.printf.format = my_strdup (format);
  return node;
}

ast_node_t *
make_function_decl (char *name, var_list_t *argv, ast_node_t *stmt)
{
  ast_node_t *node = NULL;
  symbol_t *fun_sym = NULL;
  if ((fun_sym = getsym (name, global)) != NULL)
    {
      assert (fun_sym->type.op != funcDecl_t);
    }
  fun_sym = putsym (name, &funcdef, GLOBAL);
  fun_sym->value = mem_malloc (POOL, sizeof (value_t));
  fun_sym->type.op = fun_sym->value->type.op = funcDecl_t;
  fun_sym->value->fun = mem_malloc (POOL, sizeof (func_t));

  if (argv == NULL)
    {
      fun_sym->value->fun->argc = 0;
      fun_sym->value->fun->argv = NULL;
    }
  else
    {
      fun_sym->value->fun->argc = argv->count + 1;
      fun_sym->value->fun->argv = argv->variables;
    }

  fun_sym->value->fun->ret.op = t_num;
  fun_sym->value->fun->body = stmt;

  node = mem_malloc (POOL, sizeof (ast_node_t));
  node->type = funcDecl_t;
  node->data.funDecl.body = fun_sym;

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

ast_node_t *
make_return (ast_node_t *ret_exp)
{
  ast_node_t *node = mem_malloc (POOL, sizeof (ast_node_t));

  node->type = return_exp_t;
  node->data.return_exp.ret_exp = ret_exp;
  return node;
}

ast_node_t *
make_return_exp (ast_node_t *funCall)
{
  // if (funCall->data.funCall.body->value->fun->ret.op == -1)
  //   {
  //     return funCall;
  //   }
  // funCall->type = return_val_t;
  return funCall;
}
