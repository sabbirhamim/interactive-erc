%option noyywrap yylineno nodefault
%option case-insensitive
%{
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#include <stdio.h>
#include <unistd.h>
#include <unistd.h>
#include "parser.h"
#include "error.h"
#include "util.h"
#include <string.h>
#include <stdlib.h>
%}
WS  [ \t\r]+
ID  [a-zA-Z_][a-zA-Z0-9_]*
NUM [0-9]+
%%
"int"    { return INT; }
"print"  { return PRINT; }
{NUM}    { yylval.ival = atoi(yytext); return NUMBER; }
{ID}     { yylval.sval = xstrdup(yytext); return ID; }
"=="     { return EQ; }
"="      { return ASSIGN; }
";"      { return SEMI; }
"("      { return LPAREN; }
")"      { return RPAREN; }
"+"      { return PLUS; }
"-"      { return MINUS; }
"*"      { return TIMES; }
"/"      { return DIVIDE; }
{WS}     { /* skip */ }
\n       { /* yylineno auto */ }
.        { return *yytext; }
%%
