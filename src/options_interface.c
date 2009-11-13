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

#include "options_callbacks.h"
#include "options_interface.h"
#include "support.h"
#include "file.h"

#define GLADE_HOOKUP_OBJECT(component,widget,name) \
  g_object_set_data_full (G_OBJECT (component), name, \
    gtk_widget_ref (widget), (GDestroyNotify) gtk_widget_unref)

#define GLADE_HOOKUP_OBJECT_NO_REF(component,widget,name) \
  g_object_set_data (G_OBJECT (component), name, widget)

GtkWidget*
create_window_options (void)
{
  GtkWidget *window_options;
  GtkBuilder *builder;
  builder = load_ui(file_find_support_file("bygfoot_options.glade", TRUE));
  window_options = GTK_WIDGET (gtk_builder_get_object (builder, "window_options"));

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton1");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton10");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton11");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton12");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton13");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton14");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton15");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton16");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton17");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton18");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton19");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton2");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton20");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton21");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton22");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton23");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton24");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton25");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton26");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton27");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton28");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton29");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton3");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton30");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton31");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton32");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton33");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton34");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton35");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton36");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton4");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton5");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton6");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton7");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton8");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton9");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton_autosave");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton_auto_sub");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton_bet_show_all_leagues");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton_bet_show_cups");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton_bet_show_only_recent");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton_conf_quit");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton_conf_unfit");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton_conf_youth");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton_maximize");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton_news_cup");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton_news_league");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton_news_recent");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton_news_user");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton_pause_break");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton_pause_injury");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton_pause_red");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton_pref_mess");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton_save_overwrite");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton_show_all_leagues");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton_show_job");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton_show_live");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton_show_overall");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton_show_progress_pics");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton_show_tendency");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton_skip");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton_store_restore_default_team");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton_swap_adapts");
  GTK_HOOKUP_OBJECT (window_options, builder, "combobox_hotel");
  GTK_HOOKUP_OBJECT (window_options, builder, "combobox_languages");
  GTK_HOOKUP_OBJECT (window_options, builder, "entry_constants_file");
  GTK_HOOKUP_OBJECT (window_options, builder, "entry_font_name");
  GTK_HOOKUP_OBJECT (window_options, builder, "label_training");
  GTK_HOOKUP_OBJECT (window_options, builder, "radiobutton_news_popup_always");
  GTK_HOOKUP_OBJECT (window_options, builder, "radiobutton_news_popup_no");
  GTK_HOOKUP_OBJECT (window_options, builder, "radiobutton_news_popup_user");
  GTK_HOOKUP_OBJECT (window_options, builder, "spinbutton_autosave");
  GTK_HOOKUP_OBJECT (window_options, builder, "spinbutton_autosave_files");
  GTK_HOOKUP_OBJECT (window_options, builder, "spinbutton_bet_wager");
  GTK_HOOKUP_OBJECT (window_options, builder, "spinbutton_contract");
  GTK_HOOKUP_OBJECT (window_options, builder, "spinbutton_live_speed");
  GTK_HOOKUP_OBJECT (window_options, builder, "spinbutton_live_verbosity");
  GTK_HOOKUP_OBJECT (window_options, builder, "spinbutton_precision");
  GTK_HOOKUP_OBJECT (window_options, builder, "spinbutton_recreation");
  GTK_HOOKUP_OBJECT (window_options, builder, "spinbutton_refresh");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton_save_global");
  GTK_HOOKUP_OBJECT (window_options, builder, "checkbutton_save_user");

  /* free memory used by GtkBuilder object */
  g_object_unref (G_OBJECT (builder));

  return window_options;
}

GtkWidget*
create_window_constants (void)
{
  GtkWidget *window_constants;
  GtkBuilder *builder;
  builder = load_ui(file_find_support_file("bygfoot_options.glade", TRUE));
  window_constants = GTK_WIDGET (gtk_builder_get_object (builder, "window_constants"));

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GTK_HOOKUP_OBJECT (window_constants, builder, "treeview_constants_integer");
  GTK_HOOKUP_OBJECT (window_constants, builder, "treeview_constants_float");
  GTK_HOOKUP_OBJECT (window_constants, builder, "treeview_constants_string");
  GTK_HOOKUP_OBJECT (window_constants, builder, "treeview_constants_app");
  GTK_HOOKUP_OBJECT (window_constants, builder, "notebook_constants");

  /* free memory used by GtkBuilder object */
  g_object_unref (G_OBJECT (builder));

  return window_constants;
}

