#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "training_callbacks.h"
#include "training_interface.h"
#include "file.h"
#include "support.h"

GtkWidget*
create_window_training_camp (void)
{
  GtkWidget *window_training_camp;
  GtkBuilder *builder;
  builder = load_ui(file_find_support_file("bygfoot_training.glade", TRUE));
  window_training_camp = GTK_WIDGET (gtk_builder_get_object (builder, "window_training_camp"));

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GTK_HOOKUP_OBJECT (window_training_camp, builder, "tf_costs");
  GTK_HOOKUP_OBJECT (window_training_camp, builder, "rb_camp1");
  GTK_HOOKUP_OBJECT (window_training_camp, builder, "rb_camp2");
  GTK_HOOKUP_OBJECT (window_training_camp, builder, "rb_camp3");
  GTK_HOOKUP_OBJECT (window_training_camp, builder, "l_costs");
  GTK_HOOKUP_OBJECT (window_training_camp, builder, "l_recreation");
  GTK_HOOKUP_OBJECT (window_training_camp, builder, "l_training");
  GTK_HOOKUP_OBJECT (window_training_camp, builder, "l_camp_points");
  GTK_HOOKUP_OBJECT (window_training_camp, builder, "hs_recreation");
  GTK_HOOKUP_OBJECT (window_training_camp, builder, "hs_training");
  GTK_HOOKUP_OBJECT (window_training_camp, builder, "hs_camp_points");
  GTK_HOOKUP_OBJECT (window_training_camp, builder, "checkbutton_save");

  /* free memory used by GtkBuilder object */
  g_object_unref (G_OBJECT (builder));

  return window_training_camp;
}

