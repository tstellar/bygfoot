#ifndef LANGUAGE_H
#define LANGUAGE_H

#include "bygfoot.h"
#include "option.h"

#define language_get_current_index() (strlen(opt_str("string_opt_language_code")) > 0) ? language_get_code_index(opt_str("string_opt_language_code")) : -1

gint
language_get_code_index(const gchar *code);

void
language_set(gint index);

#endif
