#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "league_table_callbacks.h"
#include "league_table_interface.h"
#include "support.h"


gboolean
on_window_league_table_delete_event    (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{

  return FALSE;
}


void
on_lt_b_ok_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{

}

