#ifndef WINDOW_H
#define WINDOW_H

#include "bygfoot.h"

void
window_show_startup(void);

GtkWidget*
window_set_version(GtkWidget *window);

GtkWidget*
window_create(gint window_type);

#endif
