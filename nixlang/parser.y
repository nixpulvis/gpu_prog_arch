%{
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <math.h>

  static float assignment_hash[1024];

  int hash(char *label);
  float assign(char *label, float value);
  float lookup(char *label);
  float call0(char* func);
  float call1(char* func, float a);
  float call2(char* func, float a, float b);

  void yyerror(const char *msg);
%}

%union value {
  char *s;
  float f;
}

%right TOK_EQ
%left TOK_IFTHEN TOK_ELSE
%left TOK_COMP_EQ TOK_COMP_GT TOK_COMP_LT TOK_COMP_GTEQ TOK_COMP_LTEQ
%left TOK_MINUS TOK_PLUS
%left TOK_MULT TOK_DIV

%token TOK_NUM
%token TOK_LABEL
%token TOK_DELIM
%token TOK_PAR_OPEN
%token TOK_PAR_CLOSE
%token TOK_NEW_LINE

%type<f> TOK_NUM expression
%type<s> TOK_LABEL

%%

input :
      | input line
      ;

line : TOK_NEW_LINE
     | expression TOK_NEW_LINE { printf("> %g\n", $1); }
     ;

expression : TOK_NUM { $$ = $1; }
           | TOK_LABEL { $$ = lookup($1); }
           | expression TOK_PLUS expression { $$ = $1 + $3; }
           | expression TOK_MINUS expression { $$ = $1 - $3; }
           | expression TOK_MULT expression { $$ = $1 * $3; }
           | expression TOK_DIV expression { $$ = $1 / $3; }
           | TOK_PAR_OPEN expression TOK_PAR_CLOSE { $$ = $2; }
           | TOK_LABEL TOK_PAR_OPEN TOK_PAR_CLOSE { $$ = call0($1); }
           | TOK_LABEL TOK_PAR_OPEN expression TOK_PAR_CLOSE { $$ = call1($1, $3); }
           | TOK_LABEL TOK_PAR_OPEN expression TOK_DELIM expression TOK_PAR_CLOSE { $$ = call2($1, $3, $5); }
           | TOK_LABEL TOK_EQ expression { $$ = assign($1, $3); }
           | expression TOK_IFTHEN expression TOK_ELSE expression { $$ = $1 ? $3 : $5; }
           | expression TOK_COMP_EQ expression { $$ = $1 == $3; }
           | expression TOK_COMP_GT expression { $$ = $1 > $3; }
           | expression TOK_COMP_LT expression { $$ = $1 < $3; }
           | expression TOK_COMP_GTEQ expression { $$ = $1 >= $3; }
           | expression TOK_COMP_LTEQ expression { $$ = $1 <= $3; }
           ;

%%

/* This is kinda sketchy as is. */
int hash(char *label)
{
  int _hash = 0;
  for (int i = 0; label[i] != '\0'; i++)
    _hash = label[i] + (_hash << 6) + (_hash << 16) - _hash;
  return _hash % 1024;
}


float assign(char *label, float value)
{
  assignment_hash[hash(label)] = value;
  return value;
}


float lookup(char *label)
{
  return assignment_hash[hash(label)];
}


float call0(char* func)
{
  if (strcmp(func, "e") == 0)
  {
    return M_E;
  }
  else if (strcmp(func, "pi") == 0)
  {
    return M_PI;
  }
  else if (strcmp(func, "rand") == 0)
  {
    return rand();
  }

  fprintf(stderr, "function %s not found\n", func);
  exit(1);
}


float call1(char* func, float a)
{
  if (strcmp(func, "inc") == 0)
  {
    return a + 1;
  }
  else if (strcmp(func, "dec") == 0)
  {
    return a - 1;
  }
  else if (strcmp(func, "sqrt") == 0)
  {
    return sqrt(a);
  }
  else if (strcmp(func, "log") == 0)
  {
    return log(a);
  }
  else if (strcmp(func, "log2") == 0)
  {
    return log2(a);
  }
  else if (strcmp(func, "log10") == 0)
  {
    return log10(a);
  }
  else if (strcmp(func, "abs") == 0)
  {
    return abs(a);
  }
  else if (strcmp(func, "ceil") == 0)
  {
    return ceil(a);
  }
  else if (strcmp(func, "floor") == 0)
  {
    return floor(a);
  }
  else if (strcmp(func, "round") == 0)
  {
    return round(a);
  }
  else if (strcmp(func, "sin") == 0)
  {
    return sin(a);
  }
  else if (strcmp(func, "cos") == 0)
  {
    return cos(a);
  }
  else if (strcmp(func, "tan") == 0)
  {
    return tan(a);
  }
  else if (strcmp(func, "sinh") == 0)
  {
    return sinh(a);
  }
  else if (strcmp(func, "cosh") == 0)
  {
    return cosh(a);
  }
  else if (strcmp(func, "tanh") == 0)
  {
    return tanh(a);
  }
  else if (strcmp(func, "asin") == 0)
  {
    return asin(a);
  }
  else if (strcmp(func, "acos") == 0)
  {
    return acos(a);
  }
  else if (strcmp(func, "atan") == 0)
  {
    return atan(a);
  }

  fprintf(stderr, "function %s not found\n", func);
  exit(1);
}


float call2(char* func, float a, float b)
{
  if (strcmp(func, "pow") == 0)
  {
    return pow(a, b);
  }
  else if (strcmp(func, "mod") == 0)
  {
    return fmod(a, b);
  }
  else if (strcmp(func, "min") == 0)
  {
    return fmin(a, b);
  }
  else if (strcmp(func, "max") == 0)
  {
    return fmax(a, b);
  }


  fprintf(stderr, "function %s not found\n", func);
  exit(1);
}


void yyerror(const char *msg)
{
  fprintf(stderr, "%s\n", msg);
  exit(1);
}


int main(void)
{
  yyparse();
  return 0;
}