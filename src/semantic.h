#ifndef SEMANTIC_H
#define SEMANTIC_H
#include <stdbool.h>
#include "symbol_table.h"
extern SymTable g_symbols;
void semantic_init(void);
void semantic_finish(void);
bool declare_int(const char *name);
bool check_var_exists(const char *name);
#endif
