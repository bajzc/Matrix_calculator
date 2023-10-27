#include "ast_jit.h"
#include "parser.h"
#include "syms.h"
#include "mem_pool.h"
#include "ast.h"
extern ast_node_t *ast_root;
extern table_t *global;
mem_pool_t *POOL;
int
main ()
{
  POOL = mem_create_pool (PAGE_SIZE);
  yydebug = 0;
  void *temp;
  temp = NULL;
  init_base_tables ();
  jit_init ();
  yyparse ();
  while (temp != ast_root)
    {
      //ast_exec (ast_root);
      code_gen (ast_root);
      temp = ast_root;
      yyparse ();
    }
  code_compile ();
  mem_destroy_pool (POOL);
  return 0;
}