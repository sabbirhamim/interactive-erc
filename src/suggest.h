#ifndef SUGGEST_H
#define SUGGEST_H
#include <stdbool.h>
typedef struct {
    int line;
    char before[256];
    char after[256];
    char message[256];
} Suggestion;
typedef struct { Suggestion *items; int count; int cap; } Suggestions;
void suggestions_init(Suggestions *s);
void suggestions_free(Suggestions *s);
void suggestions_scan_source(const char *src, Suggestions *out);
bool suggestions_apply_all(char **inout_src, const Suggestions *s, bool (*ask)(const Suggestion*));
#endif
