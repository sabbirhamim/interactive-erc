#ifndef UTIL_H
#define UTIL_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define DIE(...) do { fprintf(stderr, __VA_ARGS__); fputc('\n', stderr); exit(EXIT_FAILURE);} while(0)
#define ARR_LEN(x) ((int)(sizeof(x)/sizeof((x)[0])))

/* Portable strdup */
static inline char* xstrdup(const char* s){
    size_t n = strlen(s)+1;
    char *p = (char*)malloc(n);
    if(!p) DIE("out of memory in xstrdup");
    memcpy(p, s, n);
    return p;
}

#endif
