#include "ast.h"
#include "parser.h"
#include "syms.h"
#include "mem_pool.h"
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
  yyparse ();
  while (temp != ast_root)
    {
      ast_exec (ast_root);
      temp = ast_root;
      yyparse ();
    }
  mem_destroy_pool (POOL);
  return 0;
}