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
    WINDOW_FILE_CHOOSER,
    WINDOW_CONTRACT,
    WINDOW_USER_MANAGEMENT,
    WINDOW_DEBUG,
    WINDOW_HELP,
    WINDOW_TRANSFER_DIALOG,
    WINDOW_SPONSORS,
    WINDOW_MMATCHES,
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
window_show_digits(const gchar *text_main, const gchar* text1,
		   gint value1, const gchar* text2, gint value2);

void
window_show_file_sel(void);

void
window_show_stadium(void);

void
window_show_yesno(const gchar *text);

void
window_show_options(void);

void
window_show_menu_player(GdkEvent *event);

void
window_show_menu_youth(GdkEvent *event);

void
window_live_set_spinbuttons(void);

void
window_show_transfer_dialog(const gchar *text);

void
window_show_news(void);

void
window_show_mmatches(void);

#endif
