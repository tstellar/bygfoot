#ifndef MISC_H
#define MISC_H

#include "bygfoot.h"

void
misc_print_error(GError **error, gboolean abort_program);

void
misc_swap_int(gint *first, gint *second);

void
misc_swap_gpointer(gpointer *first, gpointer *second);

GPtrArray*
misc_separate_strings(gchar *string);

GPtrArray*
misc_randomise_g_pointer_array(GPtrArray *array);

void
misc_print_grouped_int(gint number, gchar *buf, gboolean append);

#endif
