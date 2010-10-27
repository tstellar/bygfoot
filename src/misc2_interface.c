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

#include "misc2_callbacks.h"
#include "misc2_interface.h"
#include "file.h"
#include "support.h"

#define GLADE_HOOKUP_OBJECT(component,widget,name) \
  g_object_set_data_full (G_OBJECT (component), name, \
    gtk_widget_ref (widget), (GDestroyNotify) gtk_widget_unref)

#define GLADE_HOOKUP_OBJECT_NO_REF(component,widget,name) \
  g_object_set_data (G_OBJECT (component), name, widget)

GtkWidget*
create_window_job_offer (void)
{
  GtkWidget *window_job_offer;
  GtkBuilder *builder;
  builder = load_ui(file_find_support_file("bygfoot_misc2.glade", TRUE));
  window_job_offer = GTK_WIDGET (gtk_builder_get_object (builder, "window_job_offer"));

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GTK_HOOKUP_OBJECT (window_job_offer, builder, "label_text");
  GTK_HOOKUP_OBJECT (window_job_offer, builder, "label_text2");
  GTK_HOOKUP_OBJECT (window_job_offer, builder, "label_name");
  GTK_HOOKUP_OBJECT (window_job_offer, builder, "label_league");
  GTK_HOOKUP_OBJECT (window_job_offer, builder, "label_rank");
  GTK_HOOKUP_OBJECT (window_job_offer, builder, "label_money");
  GTK_HOOKUP_OBJECT (window_job_offer, builder, "label_cap");
  GTK_HOOKUP_OBJECT (window_job_offer, builder, "label_saf");
  GTK_HOOKUP_OBJECT (window_job_offer, builder, "label_average_skill");
  GTK_HOOKUP_OBJECT (window_job_offer, builder, "treeview_players");

  /* free memory used by GtkBuilder object */
  g_object_unref (G_OBJECT (builder));

  return window_job_offer;
}

GtkWidget*
create_window_progress (void)
{
  GtkWidget *window_progress;
  GtkBuilder *builder;
  builder = load_ui(file_find_support_file("bygfoot_misc2.glade", TRUE));
  window_progress = GTK_WIDGET (gtk_builder_get_object (builder, "window_progress"));

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GTK_HOOKUP_OBJECT (window_progress, builder, "image_match");
  GTK_HOOKUP_OBJECT (window_progress, builder, "progressbar");

  /* free memory used by GtkBuilder object */
  g_object_unref (G_OBJECT (builder));

  return window_progress;
}

GtkWidget*
create_window_warning (void)
{
  GtkWidget *window_warning;
  GtkBuilder *builder;
  builder = load_ui(file_find_support_file("bygfoot_misc2.glade", TRUE));
  window_warning = GTK_WIDGET (gtk_builder_get_object (builder, "window_warning"));

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GTK_HOOKUP_OBJECT (window_warning, builder, "label_warning");

  /* free memory used by GtkBuilder object */
  g_object_unref (G_OBJECT (builder));

  return window_warning;
}

GtkWidget*
create_window_digits (void)
{
  GtkWidget *window_digits;
  GtkBuilder *builder;
  builder = load_ui(file_find_support_file("bygfoot_misc2.glade", TRUE));
  window_digits = GTK_WIDGET (gtk_builder_get_object (builder, "window_digits"));

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GTK_HOOKUP_OBJECT (window_digits, builder, "label_main");
  GTK_HOOKUP_OBJECT (window_digits, builder, "label_1");
  GTK_HOOKUP_OBJECT (window_digits, builder, "label_2");
  GTK_HOOKUP_OBJECT (window_digits, builder, "spinbutton1");
  GTK_HOOKUP_OBJECT (window_digits, builder, "spinbutton2");
  GTK_HOOKUP_OBJECT (window_digits, builder, "button_digits_alr");

  /* free memory used by GtkBuilder object */
  g_object_unref (G_OBJECT (builder));

  return window_digits;
}

GtkWidget*
create_window_yesno (void)
{
  GtkWidget *window_yesno;
  GtkBuilder *builder;
  builder = load_ui(file_find_support_file("bygfoot_misc2.glade", TRUE));
  window_yesno = GTK_WIDGET (gtk_builder_get_object (builder, "window_yesno"));

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GTK_HOOKUP_OBJECT (window_yesno, builder, "label_yesno");

  /* free memory used by GtkBuilder object */
  g_object_unref (G_OBJECT (builder));

  return window_yesno;
}

GtkWidget*
create_window_contract (void)
{
  GtkWidget *window_contract;
  GtkBuilder *builder;
  builder = load_ui(file_find_support_file("bygfoot_misc2.glade", TRUE));
  window_contract = GTK_WIDGET (gtk_builder_get_object (builder, "window_contract"));

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GTK_HOOKUP_OBJECT (window_contract, builder, "label_contract");
  GTK_HOOKUP_OBJECT (window_contract, builder, "spinbutton_contract1");
  GTK_HOOKUP_OBJECT (window_contract, builder, "spinbutton_contract2");
  GTK_HOOKUP_OBJECT (window_contract, builder, "spinbutton_contract3");
  GTK_HOOKUP_OBJECT (window_contract, builder, "spinbutton_contract4");
  GTK_HOOKUP_OBJECT (window_contract, builder, "radiobutton_contract1");
  GTK_HOOKUP_OBJECT (window_contract, builder, "radiobutton_contract2");
  GTK_HOOKUP_OBJECT (window_contract, builder, "radiobutton_contract3");
  GTK_HOOKUP_OBJECT (window_contract, builder, "radiobutton_contract4");
  GTK_HOOKUP_OBJECT (window_contract, builder, "button_contract_cancel");

  /* free memory used by GtkBuilder object */
  g_object_unref (G_OBJECT (builder));

  return window_contract;
}

GtkWidget*
create_window_user_management (void)
{
  GtkWidget *window_user_management;
  GtkBuilder *builder;
  builder = load_ui(file_find_support_file("bygfoot_misc2.glade", TRUE));
  window_user_management = GTK_WIDGET (gtk_builder_get_object (builder, "window_user_management"));

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GTK_HOOKUP_OBJECT (window_user_management, builder, "treeview_user_management_users");
  GTK_HOOKUP_OBJECT (window_user_management, builder, "treeview_user_management_teams");
  GTK_HOOKUP_OBJECT (window_user_management, builder, "entry_user_management");

  /* free memory used by GtkBuilder object */
  g_object_unref (G_OBJECT (builder));

  return window_user_management;
}

GtkWidget*
create_window_debug (void)
{
  GtkWidget *window_debug;
  GtkBuilder *builder;
  builder = load_ui(file_find_support_file("bygfoot_misc2.glade", TRUE));
  window_debug = GTK_WIDGET (gtk_builder_get_object (builder, "window_debug"));

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GTK_HOOKUP_OBJECT (window_debug, builder, "entry_debug");

  /* free memory used by GtkBuilder object */
  g_object_unref (G_OBJECT (builder));

  return window_debug;
}

GtkWidget*
create_window_help (void)
{
  GtkWidget *window_help;
  GtkBuilder *builder;
  builder = load_ui(file_find_support_file("bygfoot_misc2.glade", TRUE));
  window_help = GTK_WIDGET (gtk_builder_get_object (builder, "window_help"));

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GTK_HOOKUP_OBJECT (window_help, builder, "label_about");
  GTK_HOOKUP_OBJECT (window_help, builder, "treeview_contributors");
  GTK_HOOKUP_OBJECT (window_help, builder, "label_help_text1");
  GTK_HOOKUP_OBJECT (window_help, builder, "label_help_text2");
  GTK_HOOKUP_OBJECT (window_help, builder, "notebook1");

  /* free memory used by GtkBuilder object */
  g_object_unref (G_OBJECT (builder));

  return window_help;
}

GtkWidget*
create_window_transfer_dialog (void)
{
  GtkWidget *window_transfer_dialog;
  GtkBuilder *builder;
  builder = load_ui(file_find_support_file("bygfoot_misc2.glade", TRUE));
  window_transfer_dialog = GTK_WIDGET (gtk_builder_get_object (builder, "window_transfer_dialog"));

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GTK_HOOKUP_OBJECT (window_transfer_dialog, builder, "label_transfer_dialog");

  /* free memory used by GtkBuilder object */
  g_object_unref (G_OBJECT (builder));

  return window_transfer_dialog;
}

GtkWidget*
create_window_mmatches (void)
{
  GtkWidget *window_mmatches;
  GtkBuilder *builder;
  builder = load_ui(file_find_support_file("bygfoot_misc2.glade", TRUE));
  window_mmatches = GTK_WIDGET (gtk_builder_get_object (builder, "window_mmatches"));

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GTK_HOOKUP_OBJECT (window_mmatches, builder, "treeview_mmatches");
  GTK_HOOKUP_OBJECT (window_mmatches, builder, "entry_mm_file");

  /* free memory used by GtkBuilder object */
  g_object_unref (G_OBJECT (builder));

  return window_mmatches;
}

