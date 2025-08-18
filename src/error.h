#ifndef ERROR_H
#define ERROR_H
#include <stdbool.h>
extern int yylineno;
extern char *yytext;
void set_current_source(const char *src, const char *path);
void compiler_error(const char *fmt, ...);
void parser_error(const char *msg);
void print_context_line(int line);
#endif
