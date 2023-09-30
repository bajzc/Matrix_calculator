#include "ast.h"
#include "mystring.h"
#include "parser.h"
#include "syms.h"
#include <string.h>

extern int level;

ast_node_t *
make_var (double num)
{
  ast_node_t *node = malloc (sizeof (ast_node_t));
  node->type = t_num;
  node->data.var = num;
  return node;
}

ast_node_t *
make_name (char *name)
{
  ast_node_t *node = malloc (sizeof (ast_node_t));
  node->type = t_name;
  node->name = name;
  node->data.var = getsym (name)->value->var;
  return node;
}

ast_node_t *
make_expression (ast_node_t *left, char op, ast_node_t *right)
{
  ast_node_t *node = malloc (sizeof (ast_node_t));
  node->type = exp_t;
  node->data.expression.left = left;
  node->data.expression.right = right;
  node->data.expression.op = op;
  return node;
}

ast_node_t *
make_assignment (char *name, ast_node_t *right)
{
  ast_node_t *node = malloc (sizeof (ast_node_t));
  node->type = assig_t;
  node->data.assignment.name = getsym (name);
  node->data.assignment.right = right;
  return node;
}

ast_node_t *
make_statement (ast_node_t *StmtList, ast_node_t *stmt)
{
  if (StmtList == NULL)
    { // root is NULL, init statements
      StmtList = malloc (sizeof (ast_node_t));
      StmtList->type = stmts_t;
      StmtList->data.statements.count = -1;
    }
  ast_node_t ***stmt_arr = &StmtList->data.statements.stmt_arr;
  StmtList->data.statements.count += 1;

  ast_node_t **temp
    = realloc (*stmt_arr,
	       sizeof (ast_node_t *) * (StmtList->data.statements.count + 1));
  if (!temp)
    {
      printf ("stmt_arr realloc error!\n");
    }

  *stmt_arr = temp;
  (*stmt_arr)[StmtList->data.statements.count] = stmt;
  return StmtList;
}

ast_node_t *
make_while (ast_node_t *condition, ast_node_t *statements)
{
  ast_node_t *node = malloc (sizeof (ast_node_t));
  node->type = while_stmt_t;
  node->data.whilestmt.condition = condition;
  node->data.whilestmt.statements = statements;
  return node;
}

ast_node_t *
make_compare (ast_node_t *left, int op, ast_node_t *right)
{
  ast_node_t *node = malloc (sizeof (ast_node_t));
  node->type = compare_t;
  node->data.expression.left = left;
  node->data.expression.right = right;
  node->data.expression.op = op;
  return node;
}

ast_node_t *
make_postfix_exp (ast_node_t *lval, int op)
{
  ast_node_t *node = malloc (sizeof (ast_node_t));
  node->type = postfix_t;
  node->data.expression.left = lval;
  node->data.expression.right = NULL;
  node->data.expression.op = op;
  return node;
}

ast_node_t *
make_string (struct str_s *string)
{
  ast_node_t *node = malloc (sizeof (ast_node_t));
  node->type = t_string;
  node->data.string = string;
  return node;
}

ast_node_t *
make_print_list (ast_node_t *print_list, ast_node_t *print_element)
{
  print_list = make_statement (print_list, print_element);
}

ast_node_t *
make_printf (char *format, ast_node_t *print_list)
{
  ast_node_t *node = malloc (sizeof (ast_node_t));
  node->type = t_printf;
  if (print_list == NULL)
    node->data.printf.print_avg = NULL;
  else
    node->data.printf.print_avg = print_list;
  node->data.printf.format = strdup (format);
  return node;
}