#include "ast.h"
#include "parser.h"
extern struct ast_node_s *ast_root;
int
main ()
{
  while (1)
    {
      yyparse ();
      printf ("%p\n", ast_root);
      if (!ast_root)
	exit (-1);
      ast_exec (ast_root);
    }
  return 0;
  // return ast_exec(ast_root);
}