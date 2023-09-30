#include "ast.h"
#include "mystring.h"
#include "myprintf.h"
#include "parser.h"
#include "syms.h"
#include <assert.h>
#include <math.h>
#include <float.h>
#include <stdbool.h>

extern int level;

double
ast_exec_op (double left, char op, double right)
{
  switch (op)
    {
    case '+':
      //     if (left == 1 && right == 1) // Easter Egg here, enable if use
      //     POSIX standard
      // return 3;
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
  return NAN;
}

bool
ast_exec_compare (double left, int op, double right)
{
  switch (op)
    {
    case '<':
      return left < right;
    case '>':
      return left > right;
    case t_notequal:
      return fabs (left - right) > DBL_EPSILON;
    case t_equal:
      return fabs (left - right) < DBL_EPSILON;
    case t_lessequal:
      return left <= right;
    case t_greaterequal:
      return left >= right;
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
  if (root->type == postfix_t)
    {
      return ast_exec_postfix (root);
    }
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
      lval = left->data.var;
      rval = right->data.var;
    }
  if (left->type == postfix_t)
    {
      lval = ast_exec_postfix (left);
    }
  if (right->type == postfix_t)
    {
      rval = ast_exec_postfix (right);
    }
  if (left->type == exp_t)
    {
      lval = ast_exec_exp (left);
    }
  if (right->type == exp_t)
    {
      rval = ast_exec_exp (right);
    }
  if (left->type == t_num)
    {
      lval = left->data.var;
    }
  if (right->type == t_num)
    {
      rval = right->data.var;
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
      ast_exec (stmts);
    }
}

double
ast_exec_postfix (ast_node_t *root)
{
  ast_node_t *left = root->data.expression.left;
  symbol_t *name = getsym (left->name);
  switch (root->data.expression.op)
    {
    case t_increment:
      return name->value->var++;
    case t_decrement:
      return name->value->var--;
    }
  return NAN;
}
int
ast_exec_printf (ast_node_t *root)
{
  char *format = root->data.printf.format;
  ast_node_t *print_avg = root->data.printf.print_avg;
  if (print_avg == NULL)
    {
      char *p;
      for (p = format + 1; p <= format + strlen (format) - 2; p++)
	{
	  if (*p == '\\')
	    {
	      p++;
	      print_special_char (*p);
	    }
	  else
	    printf ("%c", *p);
	}
    }
  else
    print_ast_avg (format, print_avg);
  return 0;
}

int
ast_exec (ast_node_t *root)
{ // entrance of ast
  if (!root)
    return 0;
  switch (root->type)
    {
    case exp_t:
      ast_exec_exp (root);
      break;
    case assig_t:
      // printf ("%s : %lf\n", root->data.assignment.name->name,
      //   ast_exec_assigment (root));
      ast_exec_assigment (root);
      break;
    case stmts_t:
      ast_exec_stmts (root);
      break;
    case while_stmt_t:
      ast_exec_while (root);
      break;
    case compare_t:
      ast_exec_exp (root);
      break;
    case t_name:
      printf ("%s = %lf\n", root->name, root->data.var);
      break;
    case t_num:
      assert (0);
      break;
    case postfix_t:
      ast_exec_postfix (root);
      break;
    case t_printf:
      ast_exec_printf (root);
      break;
    default:
      assert (0);
    }
  return 0;
}