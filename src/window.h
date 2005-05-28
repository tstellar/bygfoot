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
    WINDOW_WARNING,
    WINDOW_PROGRESS,
    WINDOW_DIGITS,
    WINDOW_STADIUM,
    WINDOW_JOB_OFFER,
    WINDOW_YESNO,
    WINDOW_OPTIONS,
    WINDOW_FONT_SEL,
    WINDOW_FILE_SEL,
    WINDOW_CONTRACT,
    WINDOW_USER_MANAGEMENT,
    WINDOW_DEBUG,
    WINDOW_HELP,
    WINDOW_END
};

void
window_show_help(gint page);

void
window_show_startup(void);

GtkWidget*
window_create(gint window_type);

void
window_destroy(GtkWidget **wind, gboolean count_popups);

void
window_show_digits(gchar *text_main, gchar* text1, gint value1, gchar* text2, gint value2);

void
window_show_file_sel(void);

void
window_show_stadium(void);

void
window_show_yesno(gchar *text);

void
window_show_options(void);

void
window_show_menu_player(GdkEvent *event);

void
window_live_set_spinbuttons(void);

#endif
