%{
#include "mystring.h"
#include "parser.h"
#include "syms.h"
#include <string.h>
void yyerror (char const *s);
long line_count=0;

%}

NUM         ([0-9]+\.?[0-9]*)
IDENTIFIER  ([a-zA-Z_][0-9a-zA-Z_]*)
OPERATOR    [+*/%=^(),;<>[\]-]

%%

"while"           { return t_while;                           }
"define"          { return t_define;                          }
"printf"          { return t_printf;                          }
"void"            { return t_void;                            }
"=="              { return t_equal;                           }
"!="              { return t_notequal;                        }
"<="              { return t_lessequal;                       }
">="              { return t_greaterequal;                    }
"++"              { return t_increment;                       }
"--"              { return t_decrement;                       }
"{"               { enter_scope(); return yytext[0];          }
"}"               { exit_scope();  return yytext[0];          }
{NUM}             { yylval.t_num=atof(yytext);  return t_num; }
{IDENTIFIER}      { flex_putsym(yytext);
                    yylval.t_identifier=strdup(yytext);
                    return t_identifier;                      }
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
