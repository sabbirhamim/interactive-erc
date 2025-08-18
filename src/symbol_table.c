#include "symbol_table.h"
#include "util.h"

void sym_init(SymTable *t){ t->head=NULL; }
void sym_free(SymTable *t){
    Sym *p=t->head; while(p){ Sym *n=p->next; free(p->name); free(p); p=n; }
    t->head=NULL;
}
bool sym_exists(SymTable *t, const char *name){
    for(Sym *p=t->head;p;p=p->next) if(strcmp(p->name,name)==0) return true;
    return false;
}
bool sym_add(SymTable *t, const char *name, int is_int){
    if(sym_exists(t,name)) return false;
    Sym *s=(Sym*)calloc(1,sizeof(*s));
    s->name=xstrdup(name); s->is_int=is_int; s->next=t->head; t->head=s; return true;
}
