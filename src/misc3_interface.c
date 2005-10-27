/*
 * DO NOT EDIT THIS FILE - it is generated by Glade.
 */

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

#define GLADE_HOOKUP_OBJECT(component,widget,name) \
  g_object_set_data_full (G_OBJECT (component), name, \
    gtk_widget_ref (widget), (GDestroyNotify) gtk_widget_unref)

#define GLADE_HOOKUP_OBJECT_NO_REF(component,widget,name) \
  g_object_set_data (G_OBJECT (component), name, widget)

GtkWidget*
create_window_bets (void)
{
  GtkWidget *window_bets;
  GtkWidget *vbox1;
  GtkWidget *label1;
  GtkWidget *hseparator1;
  GtkWidget *hbox1;
  GtkWidget *checkbutton_bet_all_leagues;
  GtkWidget *checkbutton_bet_cups;
  GtkWidget *checkbutton_bet_user_recent;
  GtkWidget *hseparator2;
  GtkWidget *treeview_bets;
  GtkWidget *button_bet_close;
  GtkAccelGroup *accel_group;
  GtkTooltips *tooltips;

  tooltips = gtk_tooltips_new ();

  accel_group = gtk_accel_group_new ();

  window_bets = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_container_set_border_width (GTK_CONTAINER (window_bets), 5);
  gtk_window_set_position (GTK_WINDOW (window_bets), GTK_WIN_POS_CENTER);
  gtk_window_set_modal (GTK_WINDOW (window_bets), TRUE);
  gtk_window_set_default_size (GTK_WINDOW (window_bets), 600, 700);

  vbox1 = gtk_vbox_new (FALSE, 3);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (window_bets), vbox1);

  label1 = gtk_label_new (_("Betting -- Left click on odd to place or remove bet"));
  gtk_widget_show (label1);
  gtk_box_pack_start (GTK_BOX (vbox1), label1, FALSE, FALSE, 0);

  hseparator1 = gtk_hseparator_new ();
  gtk_widget_show (hseparator1);
  gtk_box_pack_start (GTK_BOX (vbox1), hseparator1, FALSE, FALSE, 0);
  gtk_widget_set_size_request (hseparator1, 1, 10);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox1, FALSE, FALSE, 0);

  checkbutton_bet_all_leagues = gtk_check_button_new_with_mnemonic (_("Show matches from all leagues"));
  gtk_widget_show (checkbutton_bet_all_leagues);
  gtk_box_pack_start (GTK_BOX (hbox1), checkbutton_bet_all_leagues, TRUE, TRUE, 0);
  gtk_tooltips_set_tip (tooltips, checkbutton_bet_all_leagues, _("Whether to show matches of all leagues. Use the options window to toggle permanently."), NULL);

  checkbutton_bet_cups = gtk_check_button_new_with_mnemonic (_("Show matches from cups"));
  gtk_widget_show (checkbutton_bet_cups);
  gtk_box_pack_start (GTK_BOX (hbox1), checkbutton_bet_cups, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, checkbutton_bet_cups, _("Whether to show cup matches. Use the options window to toggle permanently."), NULL);

  checkbutton_bet_user_recent = gtk_check_button_new_with_mnemonic (_("Show only my recent bets"));
  gtk_widget_show (checkbutton_bet_user_recent);
  gtk_box_pack_start (GTK_BOX (hbox1), checkbutton_bet_user_recent, TRUE, TRUE, 0);
  gtk_tooltips_set_tip (tooltips, checkbutton_bet_user_recent, _("Whether to show only your bets in the recent bets list"), NULL);

  hseparator2 = gtk_hseparator_new ();
  gtk_widget_show (hseparator2);
  gtk_box_pack_start (GTK_BOX (vbox1), hseparator2, FALSE, FALSE, 0);
  gtk_widget_set_size_request (hseparator2, 1, 10);

  treeview_bets = gtk_tree_view_new ();
  gtk_widget_show (treeview_bets);
  gtk_box_pack_start (GTK_BOX (vbox1), treeview_bets, TRUE, TRUE, 0);

  button_bet_close = gtk_button_new_from_stock ("gtk-close");
  gtk_widget_show (button_bet_close);
  gtk_box_pack_start (GTK_BOX (vbox1), button_bet_close, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, button_bet_close, _("Esc"), NULL);
  gtk_widget_add_accelerator (button_bet_close, "clicked", accel_group,
                              GDK_Escape, 0,
                              GTK_ACCEL_VISIBLE);

  g_signal_connect ((gpointer) window_bets, "delete_event",
                    G_CALLBACK (on_window_bets_delete_event),
                    NULL);
  g_signal_connect ((gpointer) checkbutton_bet_all_leagues, "button_press_event",
                    G_CALLBACK (on_checkbutton_bet_all_leagues_button_press_event),
                    NULL);
  g_signal_connect ((gpointer) checkbutton_bet_cups, "button_press_event",
                    G_CALLBACK (on_checkbutton_bet_cups_button_press_event),
                    NULL);
  g_signal_connect ((gpointer) checkbutton_bet_user_recent, "button_press_event",
                    G_CALLBACK (on_checkbutton_bet_user_recent_button_press_event),
                    NULL);
  g_signal_connect ((gpointer) treeview_bets, "button_press_event",
                    G_CALLBACK (on_treeview_bets_button_press_event),
                    NULL);
  g_signal_connect ((gpointer) button_bet_close, "clicked",
                    G_CALLBACK (on_button_bet_close_clicked),
                    NULL);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (window_bets, window_bets, "window_bets");
  GLADE_HOOKUP_OBJECT (window_bets, vbox1, "vbox1");
  GLADE_HOOKUP_OBJECT (window_bets, label1, "label1");
  GLADE_HOOKUP_OBJECT (window_bets, hseparator1, "hseparator1");
  GLADE_HOOKUP_OBJECT (window_bets, hbox1, "hbox1");
  GLADE_HOOKUP_OBJECT (window_bets, checkbutton_bet_all_leagues, "checkbutton_bet_all_leagues");
  GLADE_HOOKUP_OBJECT (window_bets, checkbutton_bet_cups, "checkbutton_bet_cups");
  GLADE_HOOKUP_OBJECT (window_bets, checkbutton_bet_user_recent, "checkbutton_bet_user_recent");
  GLADE_HOOKUP_OBJECT (window_bets, hseparator2, "hseparator2");
  GLADE_HOOKUP_OBJECT (window_bets, treeview_bets, "treeview_bets");
  GLADE_HOOKUP_OBJECT (window_bets, button_bet_close, "button_bet_close");
  GLADE_HOOKUP_OBJECT_NO_REF (window_bets, tooltips, "tooltips");

  gtk_window_add_accel_group (GTK_WINDOW (window_bets), accel_group);

  return window_bets;
}

