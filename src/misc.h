#ifndef MISC_H
#define MISC_H

#include "bygfoot.h"

void
misc_print_error(GError *error, gboolean abort_program);

void
misc_swap_int(gint *first, gint *second);

GPtrArray*
misc_separate_strings(gchar *string);

#endif
