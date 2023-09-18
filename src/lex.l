%{

#include <config.h>
#include "syms.h"
#include "parser.h"
void yyerror(char const *);
extern int opt;
static int lex_line_count=0;

%}

NUM         ([0-9]+\.?[0-9]*)
NAME        ([a-zA-Z_][0-9a-zA-Z_]*)
OPERATOR    [+*/%=^(),';\[\]{}<>-]

%%

"help"            { return t_help;                            }
"det"             { return t_det;                             }
"inv"             { return t_inv;                             }
"clear"           { return t_clear;                           }
"clean"           { return t_clean;                           }
"list"            { return t_list;                            }
"exit"|"quit"     { return t_quit;                            }
{NUM}             { yylval.t_num=atof(yytext);  return t_num; }
{NAME}            {  symvar *s=getsym(yytext);
                    if(!s)
                      {s=putsym(yytext,t_name);}
                    yylval.t_name=s;
                    return s->type;                            }
{OPERATOR}        { return yytext[0];                          }
[\n]              { lex_line_count++;if(opt=='f')
                  printf("------\nLine:%d\n",lex_line_count);
                  return '\n';                                 }
[ \t]+            { /* ignore sapce */                         }
.                 { printf("unrecognized_char:'%c'\n",yytext[0]);
                    exit(-1);                                 }

%%

int yywrap(void){
  return 1;
}

void yyerror (char const *s)
{
  fprintf (stderr, "%s\n", s);
}
