#ifndef WINDOW_H
#define WINDOW_H

#include "bygfoot.h"

/**  These are used to keep track of open windows.
     @see window_create() */
enum Windows
{
    WINDOW_MAIN = 0,
    WINDOW_STARTUP,
    WINDOW_LIVE,
    WINDOW_END
};

void
window_show_startup(void);

void
window_show_main(void);

GtkWidget*
window_set_version(GtkWidget *window);

GtkWidget*
window_create(gint window_type);

void
window_destroy(GtkWidget **window);

#endif
