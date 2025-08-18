#include "semantic.h"
#include "error.h"
SymTable g_symbols;
void semantic_init(void){ sym_init(&g_symbols); }
void semantic_finish(void){ sym_free(&g_symbols); }
bool declare_int(const char *name){
    if(!sym_add(&g_symbols, name, 1)){
        compiler_error("redeclaration of variable '%s'", name);
        return false;
    }
    return true;
}
bool check_var_exists(const char *name){
    if(!sym_exists(&g_symbols, name)){
        compiler_error("use of undeclared variable '%s'", name);
        return false;
    }
    return true;
}
