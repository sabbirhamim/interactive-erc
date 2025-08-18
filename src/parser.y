%define parse.error verbose
%locations

%code requires { #include <stdbool.h> }

%code {
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include "semantic.h"
  #include "error.h"
  void yyerror(const char *msg);   /* <-- 1-arg yyerror */
  int yylex(void);
}

/* Value types for tokens/nonterminals */
%union {
  int  ival;
  char *sval;
}

/* tokens (typed where needed) */
%token INT PRINT
%token ASSIGN EQ
%token SEMI LPAREN RPAREN
%token PLUS MINUS TIMES DIVIDE
%token <sval> ID
%token <ival> NUMBER

/* precedence & associativity */
%left PLUS MINUS
%left TIMES DIVIDE

/* nonterminal types */
%type <ival> expr

%%

program : stmts ;

stmts
  : %empty
  | stmts stmt
  ;

stmt
  : decl
  | assign
  | print_stmt
  | error SEMI   { yyerrok; }
  ;

decl
  : INT ID SEMI      { declare_int($2); free($2); }
  ;

assign
  : ID ASSIGN expr SEMI
      { if(check_var_exists($1)) {/* ok */} free($1); }
  ;

print_stmt
  : PRINT LPAREN expr RPAREN SEMI { /* ok */ }
  ;

expr
  : expr PLUS expr   { $$ = $1 + $3; }
  | expr MINUS expr  { $$ = $1 - $3; }
  | expr TIMES expr  { $$ = $1 * $3; }
  | expr DIVIDE expr { $$ = $3 ? $1 / $3 : 0; }
  | NUMBER           { $$ = $1; }
  | ID               { check_var_exists($1); free($1); $$ = 0; }
  | LPAREN expr RPAREN { $$ = $2; }
  ;

%%

void yyerror(const char *msg){
    parser_error(msg);
}
