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

gboolean
query_integer_is_in_array(gint item, gint *array, gint min, gint max);

gint
misc_int_compare(gint first, gint second);

gint
misc_float_compare(gfloat first, gfloat second);

void
misc_truncate_string(const gchar *src, gchar *dest, gint number_of_chars);

gboolean
query_misc_string_contains(const gchar *string, const gchar *text);

gboolean
query_misc_string_in_array(const gchar *string, GPtrArray *array);

#endif
