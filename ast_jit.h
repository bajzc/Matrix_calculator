#ifndef AST_JIT_H
#define AST_JIT_H

#include "types.h"

void
ast_trans (ast_node_t *root, gcc_jit_function *func, gcc_jit_block **block);
gcc_jit_rvalue *
jit_make_literal (void *value, int type);
void
code_gen (ast_node_t *root);
void
jit_init (void);
void
code_compile ();
void
assig_trans (ast_node_t *root, gcc_jit_function *func, gcc_jit_block *block);
void
function_call_trans (ast_node_t *root, gcc_jit_function *parent_func,
		     gcc_jit_block *parent_block);
#endif