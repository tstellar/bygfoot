#ifndef GUI_H
#define GUI_H

#include "bygfoot.h"

void
gui_label_set_text_from_int(GtkLabel *label, gint number, gboolean append);

void
gui_show_progress(gfloat value, gchar *text);

void
gui_set_arrow_pair(gint pair, gboolean state);

void
gui_set_arrows(void);

void
gui_set_sensitive_lg_meters(gboolean state);

#endif
