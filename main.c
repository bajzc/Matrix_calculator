#include "ast.h"
#include "parser.h"
#include "syms.h"
extern struct ast_node_s *ast_root;
extern table_t *global;
#ifndef DEBUG_SYM
int
main ()
{
  void *temp;
  temp = NULL;
  yyparse ();
  while (temp != ast_root)
    {
      ast_exec (ast_root);
      temp = ast_root;
      yyparse ();
    }
  return 0;
}

#else
int
main ()
{
  yyparse ();
  return 0;
}

#endif