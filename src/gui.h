#ifndef GUI_H
#define GUI_H

#include "bygfoot.h"

void
gui_label_set_text_from_int(GtkLabel *label, gint number, gboolean append);

void
gui_label_set_text_from_float(GtkLabel *label, gfloat number,
			      gboolean append, gint precision);

void
gui_show_progress(gfloat value, gchar *text);

#endif
