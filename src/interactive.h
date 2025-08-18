#ifndef INTERACTIVE_H
#define INTERACTIVE_H
#include <stdbool.h>
#include "suggest.h"
bool interactive_mode(void);
void set_interactive(bool on);
bool ask_user_accept(const Suggestion *s);
#endif
