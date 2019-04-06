%{
#include "parser.h"
#define MAX_LABEL_LENGTH 60
%}

%%
"+" { return TOK_PLUS; }
"-" { return TOK_MINUS; }
"*" { return TOK_MULT; }
"/" { return TOK_DIV; }
"(" { return TOK_PAR_OPEN; }
")" { return TOK_PAR_CLOSE; }
"," { return TOK_DELIM; }
"=" { return TOK_EQ; }
"==" { return TOK_COMP_EQ; }
">" { return TOK_COMP_GT; }
"<" { return TOK_COMP_LT; }
">=" { return TOK_COMP_GTEQ; }
"<=" { return TOK_COMP_LTEQ; }
"?" { return TOK_IFTHEN; }
":" { return TOK_ELSE; }
"\n" { return TOK_NEW_LINE; }

[a-zA-Z][a-zA-Z0-9_]* {
  char *str = malloc(MAX_LABEL_LENGTH + 1);
  strcpy(str, yytext);
  yylval.s = str;
  return TOK_LABEL;
}

[0-9]+\.?[0-9]* {
  yylval.f = atof(yytext);
  return TOK_NUM;
}

[ \t] { /* Skip spaces and tabs. */ }
. { yyerror("invalid character"); }
%%