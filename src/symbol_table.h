#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H
#include <stdbool.h>
typedef struct Sym { char *name; int is_int; struct Sym *next; } Sym;
typedef struct { Sym *head; } SymTable;
void sym_init(SymTable *t);
void sym_free(SymTable *t);
bool sym_add(SymTable *t, const char *name, int is_int);
bool sym_exists(SymTable *t, const char *name);
#endif
