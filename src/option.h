#ifndef OPTION_H
#define OPTION_H

#include <math.h>
#include "bygfoot.h"
#include "option_struct.h"

/** Convenience abbrevs. */
#define option_set_float(name, option_array, value) option_set_int(name, option_array, (gint)rint(value * 1000))

#define opt_str(name) option_string(name, &options)
#define opt_strp(name) option_string_pointer(name, &options)
#define opt_int(name) option_int(name, &options)
#define opt_intp(name) option_int_pointer(name, &options)
#define opt_float(name) option_float(name, &options)

#define opt_set_int(name, value) option_set_int(name, &options, value)
#define opt_set_str(name, value) option_set_string(name, &options, value)
#define opt_set_float(name, value) option_set_float(name, &options, value)

#define opt_user_str(name) option_string(name, &current_user.options)
#define opt_user_strp(name) option_string_pointer(name, &current_user.options)
#define opt_user_int(name) option_int(name, &current_user.options)
#define opt_user_intp(name) option_int_pointer(name, &current_user.options)
#define opt_user_float(name) option_float(name, &current_user.options)

#define opt_user_set_int(name, value) option_set_int(name, &current_user.options, value)
#define opt_user_set_str(name, value) option_set_string(name, &current_user.options, value)
#define opt_user_set_float(name, value) option_set_float(name, &current_user.options, value)

#define const_str(name) option_string(name, &constants)
#define const_int(name) option_int(name, &constants)
#define const_float(name) option_float(name, &constants)

gfloat
option_float(const gchar *name, OptionList *optionlist);

gint
option_int(const gchar *name, OptionList *optionlist);

gint*
option_int_pointer(const gchar *name, OptionList *optionlist);

gchar*
option_string(const gchar *name, OptionList *optionlist);

GString*
option_string_pointer(const gchar *name, OptionList *optionlist);

void
option_set_string(const gchar *name, OptionList *optionlist, const gchar *new_value);

void
option_set_int(const gchar *name, OptionList *optionlist, gint new_value);

#endif
