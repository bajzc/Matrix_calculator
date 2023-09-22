%{
#include "parser.h"
#include "ast.h"
void yyerror (char const *s);
long line_count=0;

%}

NUM         ([0-9]+\.?[0-9]*)
NAME        ([a-zA-Z_][0-9a-zA-Z_]*)
OPERATOR    [+*/%=^(),;<>[\]{}-]

%%

"while"           { return t_while;                           }
"=="              { return t_equal;                           }
"!="              { return t_notequal;                        }
{NUM}             { yylval.t_num=atof(yytext);  return t_num; }
{NAME}            {	symbol_t *s=getsym(yytext);
                    if(!s)
                      {s=putsym(yytext,t_name);}
										yylval.t_name=s->name;
										return s->type.op;                        }
{OPERATOR}        { return yytext[0];               				 	}
[\n]		          { line_count++;                             }
[ \t]+            { /* ignore sapce */              					}
.									{ printf("unrecognized_char:'%c'\n",yytext[0]);
                    exit(-1);                       					}

%%

int yywrap(void){
	return 1;
}

void yyerror (char const *s)
{
  fprintf (stderr, "%s\n", s);
}
