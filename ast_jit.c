#include "ast_jit.h"
#include "ast.h"
#include "mem_pool.h"
#include "myprintf.h"
#include "parser.h"
#include "types.h"
#include <assert.h>
#include <libgccjit.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern mem_pool_t *POOL;
gcc_jit_context *ctxt = NULL;
gcc_jit_function *main_func = NULL;
gcc_jit_block *main_block = NULL;

gcc_jit_rvalue *
jit_make_literal (void *value, int type)
{
  switch (type)
    {
    case GCC_JIT_TYPE_DOUBLE:
      return gcc_jit_context_new_rvalue_from_double (
	ctxt, gcc_jit_context_get_type (ctxt, type), *(double *) value);
    case GCC_JIT_TYPE_CONST_CHAR_PTR:
      return gcc_jit_context_new_string_literal (ctxt, value);
      break;
    default:
      assert (0);
    }
}

gcc_jit_type *
type_conver (gcc_jit_rvalue *l, gcc_jit_rvalue *r)
{
  // TODO arithmetic conversion
  return gcc_jit_context_get_type (ctxt, GCC_JIT_TYPE_DOUBLE);
}

gcc_jit_type *
var_get_type (value_t *var)
{
  // TODO
  // dummy function
  gcc_jit_type *double_type
    = gcc_jit_context_get_type (ctxt, GCC_JIT_TYPE_DOUBLE);
  return double_type;
}

gcc_jit_rvalue *
exp_binary_op_trans (gcc_jit_rvalue *l, int op, gcc_jit_rvalue *r)
{
  int gcc_op;
  switch (op)
    {
    case '+':
      gcc_op = GCC_JIT_BINARY_OP_PLUS;
      break;
    case '-':
      gcc_op = GCC_JIT_BINARY_OP_MINUS;
      break;
    case '*':
      gcc_op = GCC_JIT_BINARY_OP_MULT;
      break;
    case '/':
      gcc_op = GCC_JIT_BINARY_OP_DIVIDE;
      break;
    case '%':
      gcc_op = GCC_JIT_BINARY_OP_MODULO;
      break;
    default:
      assert (0);
    }

  gcc_jit_rvalue *ret
    = gcc_jit_context_new_binary_op (ctxt, NULL, gcc_op, type_conver (l, r), l,
				     r);
  printf ("exp: %s\n",
	  gcc_jit_object_get_debug_string (gcc_jit_rvalue_as_object (ret)));
  return ret;
}

gcc_jit_rvalue *
compare_trans (gcc_jit_rvalue *l, gcc_jit_rvalue *r, int op)
{
  gcc_jit_rvalue *compare;
  int jit_op;

  switch (op)
    {
    case '<':
      jit_op = GCC_JIT_COMPARISON_LT;
      break;
    case '>':
      jit_op = GCC_JIT_COMPARISON_GT;
      break;
    case t_notequal:
      jit_op = GCC_JIT_COMPARISON_NE;
      break;
    case t_equal:
      jit_op = GCC_JIT_COMPARISON_EQ;
      break;
    case t_lessequal:
      jit_op = GCC_JIT_COMPARISON_LE;
      break;
    case t_greaterequal:
      jit_op = GCC_JIT_COMPARISON_GE;
      break;
    default:
      assert (0);
    }
  compare = gcc_jit_context_new_comparison (ctxt, NULL, jit_op, l, r);
  printf ("comp: %s\n",
	  gcc_jit_object_get_debug_string (gcc_jit_rvalue_as_object (compare)));
  return compare;
}

gcc_jit_rvalue *
get_rvalue (ast_node_t *root, gcc_jit_function *func, gcc_jit_block *block)
{
  int op;
  ast_node_t *left, *right;
  gcc_jit_rvalue *l = NULL, *r = NULL;

  switch (root->type)
    {
    case t_num:
      return gcc_jit_context_new_rvalue_from_double (
	ctxt, gcc_jit_context_get_type (ctxt, GCC_JIT_TYPE_DOUBLE),
	root->data.value);
    case t_identifier:
      return gcc_jit_lvalue_as_rvalue (root->data.variable->value->jit.lval);
    case t_string:
      return gcc_jit_context_new_string_literal (ctxt, root->data.string->data);
    case assig_t:
      assig_trans (root, func, block);
      return gcc_jit_lvalue_as_rvalue (
	root->data.assignment.name->value->jit.lval);
    case funCall_t:
      function_call_trans (root, func, block);
      return root->data.funCall.ret->jit.rval;
    }

  op = root->data.expression.op;
  left = root->data.expression.left;
  right = root->data.expression.right;

  if (left->type == t_identifier)
    {
      l = gcc_jit_lvalue_as_rvalue (left->data.variable->value->jit.lval);
    }

  if (right->type == t_identifier)
    {
      r = gcc_jit_lvalue_as_rvalue (right->data.variable->value->jit.lval);
    }

  if (left->type == t_num)
    {
      l = gcc_jit_context_new_rvalue_from_double (
	ctxt, gcc_jit_context_get_type (ctxt, GCC_JIT_TYPE_DOUBLE),
	left->data.value);
    }

  if (right->type == t_num)
    {
      r = gcc_jit_context_new_rvalue_from_double (
	ctxt, gcc_jit_context_get_type (ctxt, GCC_JIT_TYPE_DOUBLE),
	right->data.value);
    }

  if (left->type == exp_t)
    {
      l = get_rvalue (left, func, block);
    }

  if (right->type == exp_t)
    {
      r = get_rvalue (right, func, block);
    }

  if (root->type == compare_t)
    {
      return compare_trans (l, r, op);
    }
  return exp_binary_op_trans (l, op, r);
}

void
assig_trans (ast_node_t *root, gcc_jit_function *func, gcc_jit_block *block)
{
  symbol_t *l_sym = root->data.assignment.name;
  gcc_jit_rvalue *r = get_rvalue (root->data.assignment.right, func, block);
  if (l_sym->value->jit.lval == NULL)
    {
      l_sym->value->jit.lval
	= gcc_jit_function_new_local (func, NULL, gcc_jit_rvalue_get_type (r),
				      l_sym->name);
    }
  gcc_jit_block_add_assignment (block, NULL, l_sym->value->jit.lval, r);
}

void
stmts_trans (ast_node_t *root, gcc_jit_function *func, gcc_jit_block **block)
{
  int count = root->data.statements.count;
  ast_node_t **stmt_arr = root->data.statements.stmt_arr;
  for (int i = 0; i <= count; i++)
    {
      ast_trans (stmt_arr[i], func, block);
    }
}

gcc_jit_rvalue *
cond_is_true (ast_node_t *root, gcc_jit_function *func, gcc_jit_block *block)
{
  gcc_jit_rvalue *p = get_rvalue (root, func, block);
  if (gcc_jit_type_is_bool (gcc_jit_rvalue_get_type (p)))
    {
      return p;
    }
  else
    {
      return gcc_jit_context_new_comparison (
	ctxt, NULL, GCC_JIT_COMPARISON_NE, p,
	gcc_jit_context_new_rvalue_from_int (ctxt, gcc_jit_rvalue_get_type (p),
					     0));
    }
}

void
while_trans (ast_node_t *root, gcc_jit_function *func, gcc_jit_block **block)
{
  ast_node_t *stmts = root->data.whilestmt.statements;
  gcc_jit_block *b_loop_cond
    = gcc_jit_function_new_block (func, "while_condition");
  gcc_jit_block *b_loop_body
    = gcc_jit_function_new_block (func, "while_loop_body");
  gcc_jit_block *b_after
    = gcc_jit_function_new_block (func, "after_while_loop");

  gcc_jit_block_end_with_jump (*block, NULL, b_loop_cond);
  gcc_jit_rvalue *cond
    = cond_is_true (root->data.whilestmt.condition, func, *block);
  gcc_jit_block_end_with_conditional (b_loop_cond, NULL, cond, b_loop_body,
				      b_after);
  stmts_trans (stmts, func, &b_loop_body);
  gcc_jit_block_end_with_jump (b_loop_body, NULL, b_loop_cond);

  *block = b_after;
}

void
if_trans (ast_node_t *root, gcc_jit_function *func, gcc_jit_block **block)
{
  ast_node_t *if_body = root->data.ifstmt.if_body;
  ast_node_t *else_body = root->data.ifstmt.else_body;
  gcc_jit_block *b_if_cond = gcc_jit_function_new_block (func, "if_condition");
  gcc_jit_block *b_if_body = gcc_jit_function_new_block (func, "if_body");
  gcc_jit_block *b_else_body = NULL;
  gcc_jit_block *b_after = gcc_jit_function_new_block (func, "if_after");

  gcc_jit_block_end_with_jump (*block, NULL, b_if_cond);
  gcc_jit_rvalue *cond
    = cond_is_true (root->data.ifstmt.condition, func, *block);

  stmts_trans (if_body, func, &b_if_body);

  if (else_body != NULL)
    {
      b_else_body = gcc_jit_function_new_block (func, "else_body");
      stmts_trans (else_body, func, &b_else_body);
      gcc_jit_block_end_with_conditional (b_if_cond, NULL, cond, b_if_body,
					  b_else_body);
      gcc_jit_block_end_with_jump (b_else_body, NULL, b_after);
    }
  else
    {
      gcc_jit_block_end_with_conditional (b_if_cond, NULL, cond, b_if_cond,
					  b_after);
    }
  gcc_jit_block_end_with_jump (b_if_body, NULL, b_after);

  *block = b_after;
}

void
return_trans (ast_node_t *root, gcc_jit_function *func, gcc_jit_block **block)
{
  gcc_jit_rvalue *ret
    = get_rvalue (root->data.return_exp.ret_exp, func, *block);
  fprintf (stderr, "Return: %s\n",
	   gcc_jit_object_get_debug_string (gcc_jit_rvalue_as_object (ret)));
  gcc_jit_block_end_with_return (*block, NULL, ret);
  gcc_jit_block *after_return
    = gcc_jit_function_new_block (func, "after_return");
  *block = after_return;
}

void
function_call_trans (ast_node_t *root, gcc_jit_function *parent_func,
		     gcc_jit_block *parent_block)
{
  gcc_jit_function *func = root->data.funCall.body->value->jit.fun;
  func_t *ast_func = root->data.funCall.body->value->fun;
  assert (func);

  if (ast_func->argc == 0) // foo(void)
    {
      root->data.funCall.ret->jit.rval
	= gcc_jit_context_new_call (ctxt, NULL, func, 0, NULL);
      gcc_jit_block_add_eval (parent_block, NULL,
			      root->data.funCall.ret->jit.rval);
    }
  else // foo(...)
    {
      gcc_jit_rvalue **args
	= mem_malloc (POOL, sizeof (gcc_jit_rvalue *) * (ast_func->argc));
      for (unsigned i = 0; i < ast_func->argc; i++)
	{
	  args[i]
	    = get_rvalue (root->data.funCall.argv->actuals[i], NULL, NULL);
	  printf ("args[%d]: %s\n", i,
		  gcc_jit_object_get_debug_string (
		    gcc_jit_rvalue_as_object (args[i])));
	  gcc_jit_param *param = gcc_jit_function_get_param (func, i);
	  if (!gcc_jit_compatible_types (gcc_jit_rvalue_get_type (args[i]),
					 gcc_jit_rvalue_get_type (
					   gcc_jit_param_as_rvalue (param))))
	    {
	      fprintf (stderr, "ERROR: type convert\n");
	      return;
	    }
	}
      if (root->data.funCall.ret == NULL)
	root->data.funCall.ret = mem_malloc (POOL, sizeof (value_t));
      root->data.funCall.ret->jit.rval
	= gcc_jit_context_new_call (ctxt, NULL, func, ast_func->argc, args);
      gcc_jit_block_add_eval (parent_block, NULL,
			      root->data.funCall.ret->jit.rval);
    }
}

void
function_decl_trans (ast_node_t *root)
{
  func_t *func_body = root->data.funDecl.body->value->fun;
  gcc_jit_param **params = NULL;
  gcc_jit_function *func = NULL;
  gcc_jit_type *ret_type = NULL;

  if (root->data.funDecl.body->value->fun->ret.op == -1) // void foo()
    ret_type = gcc_jit_context_get_type (ctxt, GCC_JIT_TYPE_VOID);
  else if (root->data.funDecl.body->value->fun->ret.op == t_num) // double foo()
    ret_type = gcc_jit_context_get_type (ctxt, GCC_JIT_TYPE_DOUBLE);
  else
    assert (0);

  if (func_body->argc != 0) // foo(...)
    {
      params = mem_malloc (POOL, sizeof (gcc_jit_param *) * func_body->argc);
      for (unsigned i = 0; i < func_body->argc; i++)
	{
	  params[i] = gcc_jit_context_new_param (ctxt, NULL,
						 var_get_type (
						   func_body->argv[i]->value),
						 func_body->argv[i]->name);
	}
      func
	= gcc_jit_context_new_function (ctxt, NULL, GCC_JIT_FUNCTION_EXPORTED,
					ret_type, root->data.funCall.body->name,
					func_body->argc, params, 0);
      for (unsigned i = 0; i < func_body->argc; i++)
	{
	  func_body->argv[i]->value->jit.lval
	    = gcc_jit_param_as_lvalue (params[i]);
	}
    }
  else
    { // foo(void)
      func
	= gcc_jit_context_new_function (ctxt, NULL, GCC_JIT_FUNCTION_EXPORTED,
					ret_type, root->data.funDecl.body->name,
					0, NULL, 0);
    }
  root->data.funDecl.body->value->jit.fun = func;

  gcc_jit_block *b_entry = gcc_jit_function_new_block (func, "entry");

  ast_trans (func_body->body, func, &b_entry);
  if (root->data.funDecl.body->value->fun->ret.op == -1) // void foo()
    gcc_jit_block_end_with_void_return (b_entry, NULL);
  else if (root->data.funDecl.body->value->fun->ret.op == t_num) // double foo()
    gcc_jit_block_end_with_return (
      b_entry, NULL,
      gcc_jit_context_new_rvalue_from_int (
	ctxt, gcc_jit_context_get_type (ctxt, GCC_JIT_TYPE_DOUBLE), 0));
}

void
printf_trans (ast_node_t *root, gcc_jit_function *func, gcc_jit_block *block)
{
  ast_node_t *print_avg = root->data.printf.print_avg;
  char *format = mem_malloc (POOL, strlen (root->data.printf.format) - 1);
  strncpy (format, root->data.printf.format + 1,
	   strlen (root->data.printf.format) - 2);
  format = conver_format (format);
  gcc_jit_param **param_format = NULL;
  gcc_jit_rvalue **args = NULL;
  int numargs;

  if (print_avg == NULL)
    {
      numargs = 0;
      args = mem_malloc (POOL, sizeof (gcc_jit_rvalue *) * 1);
      args[0] = gcc_jit_context_new_string_literal (ctxt, format);
      param_format = mem_malloc (POOL, sizeof (gcc_jit_rvalue *) * 1);
    }
  else
    {
      numargs = print_avg->data.statements.count + 1;
      param_format
	= mem_malloc (POOL, sizeof (gcc_jit_param *) * (numargs + 1));
      args = mem_malloc (POOL, sizeof (gcc_jit_rvalue *) * (numargs));
      args[0] = gcc_jit_context_new_string_literal (ctxt, format);
      for (int i = 1; i <= numargs; i++)
	{
	  args[i] = get_rvalue (print_avg->data.statements.stmt_arr[i - 1],
				func, block);
	  char buffer[32];
	  sprintf (buffer, "argv_%d", i);
	  param_format[i]
	    = gcc_jit_context_new_param (ctxt, NULL,
					 gcc_jit_rvalue_get_type (args[i]),
					 buffer);
	  printf ("printf: %s: (%s)%s\n", buffer,
		  gcc_jit_object_get_debug_string (
		    gcc_jit_type_as_object (gcc_jit_rvalue_get_type (args[i]))),
		  gcc_jit_object_get_debug_string (
		    gcc_jit_param_as_object (param_format[i])));
	}
    }

  param_format[0] = gcc_jit_context_new_param (
    ctxt, NULL, gcc_jit_context_get_type (ctxt, GCC_JIT_TYPE_CONST_CHAR_PTR),
    "format");
  gcc_jit_function *printf_func
    = gcc_jit_context_new_function (ctxt, NULL, GCC_JIT_FUNCTION_IMPORTED,
				    gcc_jit_context_get_type (ctxt,
							      GCC_JIT_TYPE_INT),
				    "printf", 1, param_format, 1);
  gcc_jit_block_add_eval (block, NULL,
			  gcc_jit_context_new_call (ctxt, NULL, printf_func,
						    numargs + 1, args));
}

void
ast_trans (ast_node_t *root, gcc_jit_function *func,
	   gcc_jit_block **block) // add new statements to "main" or
				  // creating new function
{
  if (!root)
    return;
  switch (root->type)
    {
    case assig_t:
      assig_trans (root, func, *block);
      break;
    case exp_t:
      printf ("exp: %s\n",
	      gcc_jit_object_get_debug_string (
		gcc_jit_rvalue_as_object (get_rvalue (root, func, *block))));
      break;
    case t_printf:
      printf_trans (root, func, *block);
      break;
    case stmts_t:
      stmts_trans (root, func, block);
      break;
    case while_stmt_t:
      while_trans (root, func, block);
      break;
    case if_stmt_t:
      if_trans (root, func, block);
      break;
    case compare_t:
    case t_num:
    case t_identifier:
      fprintf (stderr, "uesless statement\n");
      break;
    case funCall_t:
      function_call_trans (root, func, *block);
      break;
    case return_exp_t:
      return_trans (root, func, block);
      break;
    default:
      fprintf (stderr, "unsupported type:%d\n", root->type);
      assert (0);
    }
}

void
jit_init (void)
{
  ctxt = gcc_jit_context_acquire ();
  assert (ctxt);
  gcc_jit_context_set_int_option (ctxt, GCC_JIT_INT_OPTION_OPTIMIZATION_LEVEL,
				  3);
  gcc_jit_context_set_bool_allow_unreachable_blocks (ctxt, 1);
  gcc_jit_context_set_bool_option (ctxt,
				   GCC_JIT_BOOL_OPTION_DUMP_INITIAL_GIMPLE, 1);
  gcc_jit_context_set_bool_option (ctxt, GCC_JIT_BOOL_OPTION_DEBUGINFO, 1);
  gcc_jit_context_set_bool_print_errors_to_stderr (ctxt, 1);
  gcc_jit_type *main_func_type
    = gcc_jit_context_get_type (ctxt, GCC_JIT_TYPE_VOID); // global statements
  main_func
    = gcc_jit_context_new_function (ctxt, NULL, GCC_JIT_FUNCTION_EXPORTED,
				    main_func_type, "main", 0, NULL, 0);
  main_block = gcc_jit_function_new_block (main_func, "entry");
}

void
code_gen (ast_node_t *root)
{
  assert (ctxt);
  if (root->type != funcDecl_t)
    ast_trans (root, main_func, &main_block);
  else
    function_decl_trans (root);
}

void
code_compile ()
{
  // gcc_jit_context_compile (ctxt);
  gcc_jit_block_end_with_void_return (main_block, NULL);
  gcc_jit_context_compile_to_file (ctxt, GCC_JIT_OUTPUT_KIND_EXECUTABLE,
				   "JIT.out");
}