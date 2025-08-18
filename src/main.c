#include "util.h"
#include "error.h"
#include "interactive.h"
#include "suggest.h"
#include "semantic.h"

extern int yyparse(void);
extern void yyrestart(FILE*);
extern FILE *yyin;

static char *read_file(const char *path){
    FILE *f = strcmp(path,"-")==0 ? stdin : fopen(path, "rb");
    if(!f) DIE("cannot open %s", path);
    fseek(f, 0, SEEK_END); long n=ftell(f); fseek(f,0,SEEK_SET);
    char *buf = malloc((size_t)n+1); fread(buf,1,(size_t)n,f); buf[n]=0;
    if(f!=stdin) fclose(f);
    return buf;
}
static void write_temp(const char *path, const char *src){
    FILE *f=fopen(path,"wb"); if(!f) DIE("cannot write %s", path);
    fwrite(src,1,strlen(src),f); fclose(f);
}
int main(int argc, char **argv){
    if(argc<2){ fprintf(stderr, "usage: %s [--interactive] <file>\n", argv[0]); return 2; }
    const char *path = argv[argc-1];
    for(int i=1;i<argc-1;i++) if(strcmp(argv[i],"--interactive")==0) set_interactive(true);
    char *src = read_file(path);
    set_current_source(src, path);
    Suggestions S; suggestions_init(&S);
    suggestions_scan_source(src, &S);
    if(S.count){
        fprintf(stdout, "\nFound %d suggestion(s).\n", S.count);
        suggestions_apply_all(&src, &S, ask_user_accept);
    // re-point error reporter at the UPDATED source
    set_current_source(src, path);
    }
    suggestions_free(&S);
    semantic_init();
    const char *tmp="build/.tmp.lang";
    write_temp(tmp, src);
    FILE *f = fopen(tmp, "rb"); if(!f) DIE("cannot open temp");
    yyin = f; yyrestart(yyin);
    int rc = yyparse();
    fclose(f);
    semantic_finish();
    free(src);
    if(rc==0){ puts("\nParse completed successfully (with recovery as needed)."); return 0; }
    else { fprintf(stderr, "\nParse failed.\n"); return 1; }
}
