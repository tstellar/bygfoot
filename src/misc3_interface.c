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

#include "misc3_callbacks.h"
#include "misc3_interface.h"
#include "support.h"
#include "file.h"

GtkWidget*
create_window_bets (void)
{
  GtkWidget *window_bets;
  GtkBuilder *builder;
  builder = load_ui(file_find_support_file("bygfoot_misc3.glade", TRUE));
  window_bets = GTK_WIDGET (gtk_builder_get_object (builder, "window_bets"));
  gtk_widget_show (window_bets);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GTK_HOOKUP_OBJECT (window_bets, builder, "checkbutton_bet_all_leagues");
  GTK_HOOKUP_OBJECT (window_bets, builder, "checkbutton_bet_cups");
  GTK_HOOKUP_OBJECT (window_bets, builder, "checkbutton_bet_user_recent");
  GTK_HOOKUP_OBJECT (window_bets, builder, "treeview_bets");

  /* free memory used by GtkBuilder object */
  g_object_unref (G_OBJECT (builder));

  return window_bets;
}

GtkWidget*
create_window_splash (void)
{
  GtkWidget *window_splash;
  GtkBuilder *builder;
  builder = load_ui(file_find_support_file("bygfoot_misc3.glade", TRUE));
  window_splash = GTK_WIDGET (gtk_builder_get_object (builder, "window_splash"));
  gtk_widget_show (window_splash);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GTK_HOOKUP_OBJECT (window_splash, builder, "treeview_splash_contributors");
  GTK_HOOKUP_OBJECT (window_splash, builder, "label_splash_hint");
  GTK_HOOKUP_OBJECT (window_splash, builder, "label_splash_hintcounter");
  GTK_HOOKUP_OBJECT (window_splash, builder, "label_splash_progress");
  GTK_HOOKUP_OBJECT (window_splash, builder, "progressbar_splash");
  GTK_HOOKUP_OBJECT (window_splash, builder, "button_splash_hint_back");
  GTK_HOOKUP_OBJECT (window_splash, builder, "button_splash_hint_next");
  GTK_HOOKUP_OBJECT (window_splash, builder, "hbox3");

  /* free memory used by GtkBuilder object */
  g_object_unref (G_OBJECT (builder));

  return window_splash;
}

GtkWidget*
create_window_alr (void)
{
  GtkWidget *window_alr;
  GtkBuilder *builder;
  builder = load_ui(file_find_support_file("bygfoot_misc3.glade", TRUE));
  window_alr = GTK_WIDGET (gtk_builder_get_object (builder, "window_alr"));
  gtk_widget_show (window_alr);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GTK_HOOKUP_OBJECT (window_alr, builder, "label_current_start_week");
  GTK_HOOKUP_OBJECT (window_alr, builder, "label_current_weekly_installment");
  GTK_HOOKUP_OBJECT (window_alr, builder, "label_alr_debt");
  GTK_HOOKUP_OBJECT (window_alr, builder, "spinbutton_start_week");
  GTK_HOOKUP_OBJECT (window_alr, builder, "spinbutton_weekly_installment");

  /* free memory used by GtkBuilder object */
  g_object_unref (G_OBJECT (builder));

  return window_alr;
}

GtkWidget*
create_window_news (void)
{
  GtkWidget *window_news;
  GtkBuilder *builder;
  builder = load_ui(file_find_support_file("bygfoot_misc3.glade", TRUE));
  window_news = GTK_WIDGET (gtk_builder_get_object (builder, "window_news"));
  gtk_widget_show (window_news);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GTK_HOOKUP_OBJECT (window_news, builder, "treeview_news");

  /* free memory used by GtkBuilder object */
  g_object_unref (G_OBJECT (builder));

  return window_news;
}

