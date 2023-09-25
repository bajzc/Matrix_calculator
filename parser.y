%{
#include "ast.h"
#include "syms.h"
void yyerror (char const *s);
int yylex(void);
extern int opt;
struct ast_node_s* ast_root=NULL;
%}

%define api.value.type union

%token <char*> t_name
%token t_while t_define
%token t_equal t_notequal t_lessequal t_greaterequal
%token t_increment t_decrement
%token <double> t_num
%nterm <struct ast_node_s*> WhileStmt exp stmt StmtBlock StmtList Lval

%precedence '='
%nonassoc t_equal t_notequal
%nonassoc '<' '>' t_lessequal t_greaterequal
%left '-' '+'
%left '*' '/' '%'
%precedence NEG
%right '^'

%verbose
%%

program:
%empty
| stmt               { ast_root = $1; return 0;                               }
;

StmtList:
  stmt               { $$ = make_statement(NULL,$1);                          }
| StmtList stmt      { $$ = make_statement($1,$2);                            }
;

stmt:
';'                  { $$ = NULL;                                             }
| exp ';'
| WhileStmt
| StmtBlock
;

StmtBlock:
  '{' StmtList '}'   { $$ = $2;                                               }
| '{' '}'            { $$ = NULL;                                             }
;

exp:
  t_num              { $$ = make_var($1);                                     }
| Lval               { $$ = $1;                                               }
| Lval '=' exp       { $$ = make_assignment($1->name,$3);                     }
| exp '+' exp        { $$ = make_expression($1,'+',$3);                       }
| exp '-' exp        { $$ = make_expression($1,'-',$3);                       }
| exp '*' exp        { $$ = make_expression($1,'*',$3);                       }
| exp '/' exp        { $$ = make_expression($1,'/',$3);                       }
| exp '%' exp        { $$ = make_expression($1,'%',$3);                       }
| exp '^' exp        { $$ = make_expression($1,'^',$3);                       }
| '-' exp %prec NEG  { $$ = make_expression(make_var(0),'-',$2);              }
| '(' exp ')'        { $$ = $2;                                               }
| exp '<' exp        { $$ = make_compare($1,'<',$3);                          }
| exp '>' exp        { $$ = make_compare($1,'>',$3);                          }
| exp t_equal exp    { $$ = make_compare($1,t_equal,$3);                      }
| exp t_notequal exp { $$ = make_compare($1,t_notequal,$3);                   }
| exp t_lessequal exp{ $$ = make_compare($1,t_lessequal,$3);                  }
| exp t_greaterequal exp { $$ = make_compare($1,t_greaterequal,$3);           }
| Lval t_increment   { $$ = make_postfix_exp($1,t_increment);                 }
| Lval t_decrement   { $$ = make_postfix_exp($1,t_decrement);                 }
;

WhileStmt:
t_while '(' exp ')' stmt { $$ = make_while($3,$5);                            }
;

Lval:
  t_name            { $$ = make_name($1);                                     }
;

%%