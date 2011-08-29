#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "league_table_callbacks.h"
#include "support.h"


G_MODULE_EXPORT gboolean
on_window_league_table_delete_event    (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
#ifdef DEBUG
    printf("on_window_league_table_delete_event\n");
#endif


  return FALSE;
}


G_MODULE_EXPORT void
on_lt_b_ok_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
#ifdef DEBUG
    printf("on_lt_b_ok_clicked\n");
#endif


}

