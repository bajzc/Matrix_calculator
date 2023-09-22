#include "ast.h"
#include "parser.h"
#include "syms.h"
#include <math.h>
#include <float.h>
#include <stdbool.h>

double
ast_exec_op (double left, char op, double right)
{
  switch (op)
    {
    case '+':
      return left + right;
    case '-':
      return left - right;
    case '*':
      return left * right;
    case '/':
      return left / right;
    case '%':
      return (long) left % (long) right;
    }
  printf ("NAN\n");
  return NAN;
}

bool
ast_exec_compare (double left, int op, double right)
{
  printf ("compare: left:%lf right:%lf\n", left, right);
  switch (op)
    {
    case '<':
      return left < right;
    case t_equal:
      return fabs (left - right) < DBL_EPSILON;
    case '>':
      return left > right;
    case t_notequal:
      return left != right;
    }
  return 0;
}

double
ast_exec_exp (ast_node_t *root)
{
  ast_node_t *left = root->data.expression.left;
  ast_node_t *right = root->data.expression.right;
  double lval, rval; // value of left and right node
  int op = root->data.expression.op;
  if (root->type == t_num)
    {
      return root->data.var;
    }
  if (root->type == t_name)
    {
      return getsym (root->name)->value->var;
    }
  if (right->type == t_name)
    {
      rval = getsym (right->name)->value->var;
    }
  if (left->type == t_name)
    {
      lval = getsym (left->name)->value->var;
    }
  if (left->type == t_num && right->type == t_num)
    {
      printf ("%g %c %g\n", left->data.var, op, right->data.var);
      return ast_exec_op (left->data.var, op, right->data.var);
    }
  if (left->type == exp_t)
    {
      printf ("cal left\n");
      lval = ast_exec_exp (left);
      printf ("get %g\n", lval);
    }
  if (right->type == exp_t)
    {
      printf ("cal right\n");
      rval = ast_exec_exp (right);
      printf ("get %g\n", rval);
    }
  if (left->type == t_num)
    {
      lval = left->data.var;
      printf ("left is num:%g\n", lval);
    }
  if (right->type == t_num)
    {
      rval = right->data.var;
      printf ("right is num:%g\n", lval);
    }
  if (root->type == compare_t)
    {
      return ast_exec_compare (lval, op, rval);
    }
  return ast_exec_op (lval, op, rval);
}

double
ast_exec_assigment (ast_node_t *root)
{
  symbol_t *name = root->data.assignment.name;
  ast_node_t *right = root->data.assignment.right;
  if (!name)
    {
      printf ("assigment is NULL\n");
    }
  name->value->var = ast_exec_exp (right);
  return name->value->var;
}
void
ast_exec_stmts (ast_node_t *root)
{
  int count = root->data.statements.count;
  ast_node_t **stmt_arr = root->data.statements.stmt_arr;
  for (int i = 0; i <= count; i++)
    {
      ast_exec (stmt_arr[i]);
    }
}

void
ast_exec_while (ast_node_t *root)
{
  ast_node_t *stmts = root->data.whilestmt.statements;
  while (ast_exec_exp (root->data.whilestmt.condition))
    {
      printf ("contidion:%lf\n", ast_exec_exp (root->data.whilestmt.condition));
      ast_exec (stmts);
    }
}

int
ast_exec (ast_node_t *root)
{ // entrance of ast
  if (!root)
    return 0;
  switch (root->type)
    {
    case exp_t:
      printf ("%g\n", ast_exec_exp (root));
      break;
    case assig_t:
      printf ("%s : %g\n", root->data.assignment.name->name,
	      ast_exec_assigment (root));
      break;
    case stmts_t:
      ast_exec_stmts (root);
      break;
    case while_stmt_t:
      ast_exec_while (root);
      break;
    case compare_t:
      printf ("%d", (bool) ast_exec_exp (root));
    case t_name:
      printf ("%s = %lf\n", root->name, root->data.var);
      break;
    case t_num:
      printf ("%lf\n", root->data.var);
    }
  return 0;
}