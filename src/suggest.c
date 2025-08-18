#include "suggest.h"
#include "util.h"
#include <ctype.h>
#include <string.h>

static void push(Suggestions *s, Suggestion sug){
    if(s->count==s->cap){
        s->cap = s->cap ? s->cap*2 : 8;
        s->items = (Suggestion*)realloc(s->items, s->cap*sizeof(*s->items));
    }
    s->items[s->count++] = sug;
}

void suggestions_init(Suggestions *s){ s->items=NULL; s->count=0; s->cap=0; }
void suggestions_free(Suggestions *s){ free(s->items); s->items=NULL; s->count=s->cap=0; }

static void ensure_trailing_semicolon(char *buf, size_t cap){
    size_t n = strlen(buf);
    while(n>0 && isspace((unsigned char)buf[n-1])) n--;
    if(n==0) return;
    if(buf[n-1] != ';' && n+1 < cap){
        buf[n] = ';';
        buf[n+1] = '\0';
    }
}

static void add_missing_semicolon(const char *line, int lno, Suggestions *out){
    size_t n=strlen(line);
    if(n==0) return;
    const char *p=line; while(*p && isspace((unsigned char)*p)) p++;
    if(*p=='\0' || *p=='{' || *p=='}') return;
    const char *end=line+n; while(end>line && isspace((unsigned char)end[-1])) end--;
    if(end>line && end[-1]==';') return;

    /* treat typical statements */
    bool looks_stmt =
        strstr(p, "print(")!=NULL ||
        strstr(p, "pritn(")!=NULL ||   /* catch misspelled print lines too */
        strstr(p, "=")!=NULL ||
        strncmp(p,"int ",4)==0;

    if(!looks_stmt) return;

    Suggestion s={0}; s.line=lno;
    snprintf(s.before, sizeof(s.before), "%s", line);
    snprintf(s.after, sizeof(s.after),  "%s;", line);
    snprintf(s.message, sizeof(s.message), "Add missing ';' at end of the statement");
    push(out, s);
}

static void replace_word(char *dst, size_t cap, const char *line, const char *bad, const char *good){
    const char *hit = strstr(line, bad);
    if(!hit){ snprintf(dst, cap, "%s", line); return; }
    size_t pre = (size_t)(hit-line);
    snprintf(dst, cap, "%.*s%s%s", (int)pre, line, good, hit+strlen(bad));
}

static void add_misspelling(const char *line, int lno, const char *bad, const char *good, const char *msg, Suggestions *out){
    if(strstr(line, bad)){
        Suggestion s={0}; s.line=lno;
        snprintf(s.before, sizeof(s.before), "%s", line);
        replace_word(s.after, sizeof(s.after), line, bad, good);
        /* special case: fixing 'pritn(...)' -> 'print(...)' should also ensure ';' */
        if(strcmp(bad, "pritn")==0){
            ensure_trailing_semicolon(s.after, sizeof(s.after));
        }
        snprintf(s.message, sizeof(s.message), "%s: '%s'→'%s'%s",
                 msg, bad, good, strcmp(bad,"pritn")==0 ? " (and add ';')" : "");
        push(out, s);
    }
}

void suggestions_scan_source(const char *src, Suggestions *out){
    int lno=1; const char *line=src, *p=src;
    while(*p){
        if(*p=='\n'){
            size_t len = (size_t)(p-line);
            char buf[256]; size_t cpy=len<255?len:255; memcpy(buf, line, cpy); buf[cpy]=0;
            add_missing_semicolon(buf, lno, out);
            add_misspelling(buf, lno, "pritn", "print", "Fix misspelled keyword", out);
            add_misspelling(buf, lno, "intr ", "int ", "Fix misspelled keyword", out);
            lno++; line=p+1;
        }
        p++;
    }
    if(line!=p){
        char buf[256]; size_t len=(size_t)(p-line); size_t cpy=len<255?len:255;
        memcpy(buf, line, cpy); buf[cpy]=0;
        add_missing_semicolon(buf, lno, out);
        add_misspelling(buf, lno, "pritn", "print", "Fix misspelled keyword", out);
        add_misspelling(buf, lno, "intr ", "int ", "Fix misspelled keyword", out);
    }
}

/* Safe, growing output builder (no overflow) */
bool suggestions_apply_all(char **inout_src, const Suggestions *s, bool (*ask)(const Suggestion*)){
    if(!s->count) return false;

    const char *src = *inout_src;
    size_t cap = strlen(src) + (size_t)s->count * 32 + 128;
    char *out = (char*)malloc(cap);
    size_t used = 0;

    /* ensure capacity helper */
    #define ENSURE(add) do{ \
        size_t need = (add); \
        if(used + need + 1 > cap){ cap = (used + need + 1) * 2; out = (char*)realloc(out, cap); } \
    }while(0)

    int lno=1; const char *line=src, *p=src;
    while(*p){
        if(*p=='\n'){
            size_t len=(size_t)(p-line);
            char cur[256]; size_t cpy=len<255?len:255; memcpy(cur, line, cpy); cur[cpy]=0;

            /* apply all suggestions for this line; if multiple exist, chain them manually */
            char work[256]; snprintf(work, sizeof(work), "%s", cur);
            for(int i=0;i<s->count;i++){
                if(s->items[i].line==lno){
                    if(!ask || ask(&s->items[i])){
                        /* reapply transform against current work if possible */
                        if(strcmp(s->items[i].message, "Fix misspelled keyword: 'pritn'→'print' (and add ';')")==0){
                            replace_word(work, sizeof(work), work, "pritn", "print");
                            ensure_trailing_semicolon(work, sizeof(work));
                        } else if(strcmp(s->items[i].message, "Add missing ';' at end of the statement")==0){
                            ensure_trailing_semicolon(work, sizeof(work));
                        } else {
                            /* default to suggested after */
                            snprintf(work, sizeof(work), "%s", s->items[i].after);
                        }
                    }
                }
            }

            size_t add = strlen(work) + 1;
            ENSURE(add);
            memcpy(out+used, work, strlen(work)); used += strlen(work);
            out[used++] = '\n';
            lno++; line=p+1;
        }
        p++;
    }
    if(line!=p){
        char cur[256]; size_t len=(size_t)(p-line); size_t cpy=len<255?len:255; memcpy(cur, line, cpy); cur[cpy]=0;
        char work[256]; snprintf(work, sizeof(work), "%s", cur);
        for(int i=0;i<s->count;i++){
            if(s->items[i].line==lno){
                if(!ask || ask(&s->items[i])){
                    if(strcmp(s->items[i].message, "Fix misspelled keyword: 'pritn'→'print' (and add ';')")==0){
                        replace_word(work, sizeof(work), work, "pritn", "print");
                        ensure_trailing_semicolon(work, sizeof(work));
                    } else if(strcmp(s->items[i].message, "Add missing ';' at end of the statement")==0){
                        ensure_trailing_semicolon(work, sizeof(work));
                    } else {
                        snprintf(work, sizeof(work), "%s", s->items[i].after);
                    }
                }
            }
        }
        size_t add = strlen(work);
        ENSURE(add);
        memcpy(out+used, work, add); used += add;
    }

    out[used] = '\0';
    #undef ENSURE

    free(*inout_src);
    *inout_src = out;
    return true;
}
