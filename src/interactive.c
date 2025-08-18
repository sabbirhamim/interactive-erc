#include "interactive.h"
#include <stdio.h>
static bool g_interactive=false;
bool interactive_mode(void){ return g_interactive; }
void set_interactive(bool on){ g_interactive=on; }
bool ask_user_accept(const Suggestion *s){
    if(!interactive_mode()) return true;
    printf("\nSuggestion (line %d): %s\n", s->line, s->message);
    printf("  before: %s\n", s->before);
    printf("   after: %s\n", s->after);
    printf("Apply? [y/N] "); fflush(stdout);
    char buf[8]={0}; if(!fgets(buf, sizeof(buf), stdin)) return false;
    return buf[0]=='y' || buf[0]=='Y';
}
