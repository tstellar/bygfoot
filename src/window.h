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
    WINDOW_STARTUP_USERS,
    WINDOW_WARNING,
    WINDOW_PROGRESS,
    WINDOW_END
};

void
window_show_startup(void);

GtkWidget*
window_create(gint window_type);

void
window_destroy(GtkWidget **wind, gboolean count_popups);

#endif
