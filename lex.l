%{
#include "mystring.h"
#include "parser.h"
#include <string.h>
#include "ast.h"
void yyerror (char const *s);
long line_count=0;

%}

NUM         ([0-9]+\.?[0-9]*)
NAME        ([a-zA-Z_][0-9a-zA-Z_]*)
OPERATOR    [+*/%=^(),;<>[\]{}-]

%%

"while"           { return t_while;                           }
"define"          { return t_define;                          }
"printf"          { return t_printf;                          }
"=="              { return t_equal;                           }
"!="              { return t_notequal;                        }
"<="              { return t_lessequal;                       }
">="              { return t_greaterequal;                    }
"++"              { return t_increment;                       }
"--"              { return t_decrement;                       }
{NUM}             { yylval.t_num=atof(yytext);  return t_num; }
{NAME}            {  symbol_t *s=getsym(yytext);
                    if(!s)
                      {s=putsym(yytext,t_name);}
                    yylval.t_name=s->name;
                    return s->type.op;                        }
{OPERATOR}        { return yytext[0];                         }
\"[^\"]*\"        { int count = 0;
                    yylval.t_string=malloc(sizeof(struct str_s));
                    yylval.t_string->len=0;
                    yylval.t_string->data=strdup(yytext);
                    char *lookup;
                    for(lookup=yytext;*lookup!='\0';lookup++){
                      yylval.t_string->len++;
                      if(*lookup=='\n')line_count++;
                      if(*lookup=='"') count++;
                    }
                    if(count!=2)
                      yyerror("missing '\"' character in string.");
                    return t_string;
                  }
[\n]              { line_count++;                             }
[ \t]+            { /* ignore sapce */                        }
.                 {printf("unrecognized_char:'%c'\n",yytext[0]);
                    exit(-1);                                 }

%%

int yywrap(void){
  return 1;
}

void yyerror (char const *s)
{
  fprintf(stderr, "at line: %ld\n",line_count);
  fprintf (stderr, "%s\n", s);
}
