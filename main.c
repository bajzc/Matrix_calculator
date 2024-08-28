#include "ast.h"
#ifdef ENABLE_JIT
#include "ast_jit.h"
#endif
#include "mem_pool.h"
#include "parser.h"
#include "syms.h"
extern ast_node_t *ast_root;
extern table_t *global;
mem_pool_t *POOL;
int
main ()
{
  POOL = mem_create_pool (PAGE_SIZE);
  yydebug = 0;
  void *temp = NULL;
  init_base_tables ();
#ifdef ENABLE_JIT
  jit_init ();
#endif
  yyparse ();
  while (temp != ast_root)
    {
#ifdef ENABLE_AST
      ast_exec (ast_root);
#endif
#ifdef ENABLE_JIT
      code_gen (ast_root);
#endif
      temp = ast_root;
      yyparse ();
    }
#ifdef ENABLE_JIT
  code_compile ();
#endif
  mem_destroy_pool (POOL);
  return 0;
}
