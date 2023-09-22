%{
#include "ast.h"
void yyerror (char const *s);
int yylex(void);
extern int opt;
struct ast_node_s* ast_root=NULL;
%}

%define api.value.type union

%token <char*> t_name t_while t_equal t_notequal
%token <double> t_num
%nterm <struct ast_node_s*> WhileStmt exp stmt StmtBlock StmtList

%precedence '='
%nonassoc t_equal t_notequal
%nonassoc '<' '>'
%left '-' '+'
%left '*' '/'

%precedence NEG
%right '^' '%'

%verbose
%%

program:
%empty
| stmt               { ast_root = $1; return 0;          }
;

StmtList:
  stmt               { $$ = make_statement(NULL,$1);     }
| StmtList stmt      { $$ = make_statement($1,$2);       }
;

stmt:
';'                  { $$ = NULL; }
| exp ';'
| WhileStmt
| StmtBlock
;

StmtBlock:
  '{' StmtList '}'   { $$ = $2;   }
| '{' '}'            { $$ = NULL; }
;

exp:
  t_num              { $$ = make_var($1);               }
| t_name             { $$ = make_name($1);              }
| t_name '=' exp     { $$ = make_assignment($1,$3);     }
| exp '+' exp        { $$ = make_expression($1,'+',$3); }
| exp '-' exp        { $$ = make_expression($1,'-',$3); }
| exp '*' exp        { $$ = make_expression($1,'*',$3); }
| exp '/' exp        { $$ = make_expression($1,'/',$3); }
| exp '%' exp        { $$ = make_expression($1,'%',$3); }
| exp '^' exp        { $$ = make_expression($1,'^',$3); }
| '-' exp %prec NEG  {
               $$ = make_expression(make_var(0),'-',$2);}
| '(' exp ')'        { $$ = $2;                         }
| exp '<' exp        { $$ = make_compare($1,'<',$3);    }
| exp '>' exp        { $$ = make_compare($1,'>',$3);    }
| exp t_equal exp    { $$ = make_compare($1,t_equal,$3);}
| exp t_notequal exp { $$ = make_compare($1,t_notequal,$3);}
;

WhileStmt:
t_while '(' exp ')' stmt { $$ = make_while($3,$5);     }
;

%%