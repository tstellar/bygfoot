#ifndef OPTION_H
#define OPTION_H

#include <math.h>
#include "bygfoot.h"

/** Convenience abbrevs. */
#define option_set_float(name, option_array, value) option_set_int(name, option_array, (gint)rint(value * 1000))

#define opt_str(name) option_string(name, options)
#define opt_int(name) option_int(name, options)
#define opt_float(name) option_float(name, options)

#define opt_set_int(name, value) option_set_int(name, options, value)
#define opt_set_str(name, value) option_set_string(name, options, value)
#define opt_set_float(name, value) option_set_float(name, options, value)

#define opt_user_str(name) option_string(name, current_user.options)
#define opt_user_int(name) option_int(name, current_user.options)
#define opt_user_float(name) option_float(name, current_user.options)

#define opt_user_set_int(name, value) option_set_int(name, current_user.options, value)
#define opt_user_set_str(name, value) option_set_string(name, current_user.options, value)
#define opt_user_set_float(name, value) option_set_float(name, current_user.options, value)

#define const_str(name) option_string(name, constants)
#define const_int(name) option_int(name, constants)
#define const_float(name) option_float(name, constants)

gfloat
option_float(gchar *name, GArray *option_array);

gint
option_int(gchar *name, GArray *option_array);

gchar*
option_string(gchar *name, GArray *option_array);

void
option_set_string(gchar *name, GArray *option_array, gchar *new_value);

void
option_set_int(gchar *name, GArray *option_array, gint new_value);

#endif
