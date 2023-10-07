#include "ast.h"
#include "parser.h"
#include "syms.h"
extern struct ast_node_s *ast_root;
extern table_t *global;
int
main ()
{
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
  return 0;
}