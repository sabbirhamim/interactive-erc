#include "error.h"
#include "util.h"
#include <stdarg.h>

static const char *g_source = NULL;
static const char *g_path = NULL;

void set_current_source(const char *src, const char *path){ g_source = src; g_path = path; }

void print_context_line(int line){
    if(!g_source) return;
    int cur=1; const char *p=g_source, *start=g_source;
    while(*p && cur<line){ if(*p=='\n'){ cur++; start=p+1; } p++; }
    const char *end=start; while(*end && *end!='\n') end++;
    fprintf(stderr, " --> %s:%d\n     ", g_path?g_path:"<input>", line);
    fwrite(start, 1, (size_t)(end-start), stderr);
    fputc('\n', stderr);
}

void compiler_error(const char *fmt, ...){
    fprintf(stderr, "[error] ");
    va_list ap; va_start(ap, fmt); vfprintf(stderr, fmt, ap); va_end(ap);
    fputc('\n', stderr);
}

void parser_error(const char *msg){
    compiler_error("%s near '%s' (line %d)", msg, yytext?yytext:"?", yylineno);
    print_context_line(yylineno);
}
