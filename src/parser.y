%{

#include <config.h>
#include "syms.h"
#include "cli.h"
void yyerror(char const *);
int yylex(void);
extern int opt;

%}

%define api.value.type union
%token <char*> t_clear t_clean t_quit t_help t_list
%token <double> t_num
%token <symvar *>  t_name t_fun t_matrix t_det
%token <matrix_t*> t_inv
%nterm <double> exp
%nterm <array_t *> array
%nterm <vector_t *> vector
%nterm <matrix_t *> matrix m_exp
%nterm <symvar *> matrix_stmt
%nterm command

%precedence '='
%left '['
%left ';'
%left ','
%left '-' '+'
%left '*' '/' '%'

%precedence NEG

%right ']'
%right '^' '\''

%verbose
%define parse.trace

%start input
%%

// for continuous input
input:
  %empty
| input line
;


line:
  '\n'
| command '\n'
| m_exp '\n'         { matrix_t_print($1);              }
| exp '\n'           { printf ("exp:%f\n", $1);         }
| error '\n'         { yyerrok;                         }
;


// %nterm <symvar *> matrix_stmt
matrix_stmt:
t_name '=' m_exp     { $1->type=t_matrix;
                       $1->value.matrix=$3;
                       $$=$1;                           }
| t_matrix '=' m_exp { matrix_free($1->value.matrix);
                     $1->value.matrix=matrix_t_copy($3);}
;


matrix:
'[' exp ']'          { array_t *a=array_init($2,0);a->count=0;
                       $$=matrix_init(vector_init(a));  }
| '[' vector ']'     { $$ = matrix_init($2);            }
;

vector:
  array              { $$ = vector_init($1);            }
| vector ';' array   { $$ = vector_add($1,$3);          }
| exp ';' exp        { $$ = vector_init_nums($1,$3);    }
;

array:
exp ',' exp          { $$ = array_init($1,$3);          }
| array ',' exp      { $$ = array_add($1,$3);           }
;

exp:
  t_num
| t_name             { $$ = $1->value.var;              }
| t_name '=' exp     { $$ = $3; $1->value.var = $3;     }
| t_fun '(' exp ')'  { $$ = $1->value.fun ($3);         }
| t_det '(' t_matrix ')'
                     {$$=matrix_det($3->value.matrix);  }
| m_exp '[' exp ']' '[' exp ']'
                     { $$=matrix_get_entity($1,$3,$6);  }
| m_exp '[' exp ']' '[' exp ']' '=' exp
                     {$$=matrix_edit_entity($1,$3,$6,$9);}
| t_matrix '=' exp   { matrix_free($1->value.matrix);
                       $1->type=t_name;
                       $$=$1->value.var=$3;             }
| exp '+' exp        { $$ = $1 + $3;                    }
| exp '-' exp        { $$ = $1 - $3;                    }
| exp '*' exp        { $$ = $1 * $3;                    }
| exp '/' exp        { $$ = $1 / $3;                    }
| exp '%' exp        { $$ = (int)$1 % (int)$3;          }
| '-' exp %prec NEG  { $$ = -$2;                        }
| exp '^' exp        { $$ = pow ($1, $3);               }
| '(' exp ')'        { $$ = $2;                         }
;

// matrix exp
// %nterm <matrix_t *>matrix m_exp
m_exp:
  matrix
| t_matrix              { $$=$1->value.matrix;                              }
| matrix_stmt           { $$=$1->value.matrix;                              }
| t_inv '(' m_exp ')'   { $$=matrix_temp(); matrix_inverse($3,$$);          }
| m_exp '+' m_exp       { $$=matrix_temp(); matrix_add($1,$3,$$);           }
| m_exp '*' m_exp       { $$=matrix_temp(); matrix_times_reorder($1,$3,$$); }
| m_exp '\''            { $$=matrix_temp(); matrix_transpose($1,$$);        }
| '(' t_matrix ')'      { $$=$2->value.matrix;                              }
;

command:
  t_clear               { if(-1==system(CLEAR)){
                          puts("system(CLEAR) error");
                          exit(-1);}                     }
| t_clean               { clean_symtab();                }
| t_quit                { exit(1);                       }
| t_help                { print_help_msg();              }
| t_list                { print_symtab();                }
;

%%
