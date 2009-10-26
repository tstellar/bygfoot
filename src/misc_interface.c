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

#include "misc_callbacks.h"
#include "misc_interface.h"
#include "support.h"
#include "file.h"

#define GLADE_HOOKUP_OBJECT(component,widget,name) \
  g_object_set_data_full (G_OBJECT (component), name, \
    gtk_widget_ref (widget), (GDestroyNotify) gtk_widget_unref)

#define GLADE_HOOKUP_OBJECT_NO_REF(component,widget,name) \
  g_object_set_data (G_OBJECT (component), name, widget)

GtkWidget*
create_window_startup (void)
{
  GtkWidget *window_startup;
  GtkBuilder *builder;
  builder = load_ui(file_find_support_file("bygfoot_misc.ui", TRUE));
  window_startup = GTK_WIDGET (gtk_builder_get_object (builder, "window_startup"));

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GTK_HOOKUP_OBJECT (window_startup, builder, "combo_country");
  GTK_HOOKUP_OBJECT (window_startup, builder, "treeview_startup");
  GTK_HOOKUP_OBJECT (window_startup, builder, "combobox_start_league");
  GTK_HOOKUP_OBJECT (window_startup, builder, "button_add_player");
  GTK_HOOKUP_OBJECT (window_startup, builder, "treeview_users");
  GTK_HOOKUP_OBJECT (window_startup, builder, "entry_player_name");
  GTK_HOOKUP_OBJECT (window_startup, builder, "team_selection_ok");
  GTK_HOOKUP_OBJECT (window_startup, builder, "radiobutton_team_def_load");
  GTK_HOOKUP_OBJECT (window_startup, builder, "radiobutton_team_def_names");
  GTK_HOOKUP_OBJECT (window_startup, builder, "checkbutton_randomise_teams");

  /* free memory used by GtkBuilder object */
  g_object_unref (G_OBJECT (builder));

  return window_startup;
}

GtkWidget*
create_window_font_sel (void)
{
  GtkWidget *window_font_sel;
  GtkBuilder *builder;
  builder = load_ui(file_find_support_file("bygfoot_misc.ui", TRUE));
  window_font_sel = GTK_WIDGET (gtk_builder_get_object (builder, "window_font_sel"));

  /* Store pointers to all widgets, for use by lookup_widget(). */

  /* free memory used by GtkBuilder object */
  g_object_unref (G_OBJECT (builder));

  return window_font_sel;
}

GtkWidget*
create_window_live (void)
{
  GtkWidget *window_live;
  GtkBuilder *builder;
  builder = load_ui(file_find_support_file("bygfoot_misc.ui", TRUE));
  window_live = GTK_WIDGET (gtk_builder_get_object (builder, "window_live"));

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GTK_HOOKUP_OBJECT (window_live, builder, "button_live_close");
  GTK_HOOKUP_OBJECT (window_live, builder, "button_pause");
  GTK_HOOKUP_OBJECT (window_live, builder, "button_resume");
  GTK_HOOKUP_OBJECT (window_live, builder, "eventbox_poss0");
  GTK_HOOKUP_OBJECT (window_live, builder, "eventbox_poss1");
  GTK_HOOKUP_OBJECT (window_live, builder, "hscale_area");
  GTK_HOOKUP_OBJECT (window_live, builder, "image_lg_boost");
  GTK_HOOKUP_OBJECT (window_live, builder, "image_lg_opp_boost");
  GTK_HOOKUP_OBJECT (window_live, builder, "image_lg_opp_style");
  GTK_HOOKUP_OBJECT (window_live, builder, "image_lg_style");
  GTK_HOOKUP_OBJECT (window_live, builder, "label_lg_avskill");
  GTK_HOOKUP_OBJECT (window_live, builder, "label_lg_formation");
  GTK_HOOKUP_OBJECT (window_live, builder, "progressbar_live");
  GTK_HOOKUP_OBJECT (window_live, builder, "scrolledwindow9");
  GTK_HOOKUP_OBJECT (window_live, builder, "spinbutton_speed");
  GTK_HOOKUP_OBJECT (window_live, builder, "spinbutton_verbosity");
  GTK_HOOKUP_OBJECT (window_live, builder, "treeview_commentary");
  GTK_HOOKUP_OBJECT (window_live, builder, "treeview_lg_opponent");
  GTK_HOOKUP_OBJECT (window_live, builder, "treeview_result");
  GTK_HOOKUP_OBJECT (window_live, builder, "treeview_stats");
  GTK_HOOKUP_OBJECT (window_live, builder, "eventbox_lg_style");
  GTK_HOOKUP_OBJECT (window_live, builder, "eventbox_lg_boost");

  /* free memory used by GtkBuilder object */
  g_object_unref (G_OBJECT (builder));

  return window_live;
}

GtkWidget*
create_window_stadium (void)
{
  GtkWidget *window_stadium;
  GtkBuilder *builder;
  builder = load_ui(file_find_support_file("bygfoot_misc.ui", TRUE));
  window_stadium = GTK_WIDGET (gtk_builder_get_object (builder, "window_stadium"));

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GTK_HOOKUP_OBJECT (window_stadium, builder, "label_capacity");
  GTK_HOOKUP_OBJECT (window_stadium, builder, "label_stadium_status");
  GTK_HOOKUP_OBJECT (window_stadium, builder, "label_average_attendance");
  GTK_HOOKUP_OBJECT (window_stadium, builder, "label_stadium_name");
  GTK_HOOKUP_OBJECT (window_stadium, builder, "spin_ticket_price");
  GTK_HOOKUP_OBJECT (window_stadium, builder, "progressbar_safety");
  GTK_HOOKUP_OBJECT (window_stadium, builder, "progressbar_average_attendance");
  GTK_HOOKUP_OBJECT (window_stadium, builder, "spinbutton_capacity");
  GTK_HOOKUP_OBJECT (window_stadium, builder, "spinbutton_safety");

  /* free memory used by GtkBuilder object */
  g_object_unref (G_OBJECT (builder));

  return window_stadium;
}

GtkWidget*
create_window_file_chooser (void)
{
  GtkWidget *window_file_chooser;
  GtkWidget *dialog_vbox1;
  GtkWidget *dialog_action_area1;
  GtkWidget *button1;
  GtkWidget *button2;

  window_file_chooser = gtk_file_chooser_dialog_new (_("Choose file"), NULL, GTK_FILE_CHOOSER_ACTION_OPEN, NULL,NULL);
  gtk_container_set_border_width (GTK_CONTAINER (window_file_chooser), 5);
  g_object_set (window_file_chooser,
                "show-hidden", TRUE,
                NULL);
  gtk_window_set_position (GTK_WINDOW (window_file_chooser), GTK_WIN_POS_CENTER);
  gtk_window_set_type_hint (GTK_WINDOW (window_file_chooser), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = GTK_DIALOG (window_file_chooser)->vbox;
  gtk_widget_show (dialog_vbox1);

  dialog_action_area1 = GTK_DIALOG (window_file_chooser)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  button1 = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (button1);
  gtk_dialog_add_action_widget (GTK_DIALOG (window_file_chooser), button1, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS (button1, GTK_CAN_DEFAULT);

  button2 = gtk_button_new_from_stock ("gtk-open");
  gtk_widget_show (button2);
  gtk_dialog_add_action_widget (GTK_DIALOG (window_file_chooser), button2, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS (button2, GTK_CAN_DEFAULT);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (window_file_chooser, window_file_chooser, "window_file_chooser");
  GLADE_HOOKUP_OBJECT_NO_REF (window_file_chooser, dialog_vbox1, "dialog_vbox1");
  GLADE_HOOKUP_OBJECT_NO_REF (window_file_chooser, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (window_file_chooser, button1, "button1");
  GLADE_HOOKUP_OBJECT (window_file_chooser, button2, "button2");

  gtk_widget_grab_default (button2);
  return window_file_chooser;
}

GtkWidget*
create_window_sponsors (void)
{
  GtkWidget *window_sponsors;
  GtkWidget *vbox48;
  GtkWidget *label_sponsors;
  GtkWidget *scrolledwindow12;
  GtkWidget *treeview_sponsors;
  GtkWidget *hbox71;
  GtkWidget *button_sponsors;
  GtkWidget *alignment23;
  GtkWidget *hbox73;
  GtkWidget *image63;
  GtkWidget *label117;
  GtkWidget *button_sponsors_wait;
  GtkWidget *alignment22;
  GtkWidget *hbox72;
  GtkWidget *image62;
  GtkWidget *label116;
  GtkTooltips *tooltips;

  tooltips = gtk_tooltips_new ();

  window_sponsors = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_container_set_border_width (GTK_CONTAINER (window_sponsors), 5);
  gtk_window_set_position (GTK_WINDOW (window_sponsors), GTK_WIN_POS_CENTER);
  gtk_window_set_modal (GTK_WINDOW (window_sponsors), TRUE);
  gtk_window_set_default_size (GTK_WINDOW (window_sponsors), 450, 350);

  vbox48 = gtk_vbox_new (FALSE, 3);
  gtk_widget_show (vbox48);
  gtk_container_add (GTK_CONTAINER (window_sponsors), vbox48);

  label_sponsors = gtk_label_new (_("There are a few companies interested in sponsoring your team. Please select one:"));
  gtk_widget_show (label_sponsors);
  gtk_box_pack_start (GTK_BOX (vbox48), label_sponsors, FALSE, FALSE, 0);
  gtk_label_set_line_wrap (GTK_LABEL (label_sponsors), TRUE);

  scrolledwindow12 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow12);
  gtk_box_pack_start (GTK_BOX (vbox48), scrolledwindow12, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow12), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow12), GTK_SHADOW_IN);

  treeview_sponsors = gtk_tree_view_new ();
  gtk_widget_show (treeview_sponsors);
  gtk_container_add (GTK_CONTAINER (scrolledwindow12), treeview_sponsors);

  hbox71 = gtk_hbox_new (FALSE, 3);
  gtk_widget_show (hbox71);
  gtk_box_pack_start (GTK_BOX (vbox48), hbox71, FALSE, FALSE, 0);

  button_sponsors = gtk_button_new ();
  gtk_widget_show (button_sponsors);
  gtk_box_pack_start (GTK_BOX (hbox71), button_sponsors, TRUE, TRUE, 0);

  alignment23 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment23);
  gtk_container_add (GTK_CONTAINER (button_sponsors), alignment23);

  hbox73 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox73);
  gtk_container_add (GTK_CONTAINER (alignment23), hbox73);

  image63 = gtk_image_new_from_stock ("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image63);
  gtk_box_pack_start (GTK_BOX (hbox73), image63, FALSE, FALSE, 0);

  label117 = gtk_label_new_with_mnemonic (_("Accept"));
  gtk_widget_show (label117);
  gtk_box_pack_start (GTK_BOX (hbox73), label117, FALSE, FALSE, 0);

  button_sponsors_wait = gtk_button_new ();
  gtk_widget_show (button_sponsors_wait);
  gtk_box_pack_start (GTK_BOX (hbox71), button_sponsors_wait, TRUE, TRUE, 0);
  gtk_tooltips_set_tip (tooltips, button_sponsors_wait, _("If you don't like any of the sponsors, you can go a few weeks without sponsor and wait for new offers."), NULL);

  alignment22 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment22);
  gtk_container_add (GTK_CONTAINER (button_sponsors_wait), alignment22);

  hbox72 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox72);
  gtk_container_add (GTK_CONTAINER (alignment22), hbox72);

  image62 = gtk_image_new_from_stock ("gtk-go-forward", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image62);
  gtk_box_pack_start (GTK_BOX (hbox72), image62, FALSE, FALSE, 0);

  label116 = gtk_label_new_with_mnemonic (_("Reject for now"));
  gtk_widget_show (label116);
  gtk_box_pack_start (GTK_BOX (hbox72), label116, FALSE, FALSE, 0);

  g_signal_connect ((gpointer) window_sponsors, "delete_event",
                    G_CALLBACK (on_window_sponsors_delete_event),
                    NULL);
  g_signal_connect ((gpointer) treeview_sponsors, "row_activated",
                    G_CALLBACK (on_treeview_sponsors_row_activated),
                    NULL);
  g_signal_connect ((gpointer) button_sponsors, "clicked",
                    G_CALLBACK (on_button_sponsors_clicked),
                    NULL);
  g_signal_connect ((gpointer) button_sponsors_wait, "clicked",
                    G_CALLBACK (on_button_sponsors_wait_clicked),
                    NULL);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (window_sponsors, window_sponsors, "window_sponsors");
  GLADE_HOOKUP_OBJECT (window_sponsors, vbox48, "vbox48");
  GLADE_HOOKUP_OBJECT (window_sponsors, label_sponsors, "label_sponsors");
  GLADE_HOOKUP_OBJECT (window_sponsors, scrolledwindow12, "scrolledwindow12");
  GLADE_HOOKUP_OBJECT (window_sponsors, treeview_sponsors, "treeview_sponsors");
  GLADE_HOOKUP_OBJECT (window_sponsors, hbox71, "hbox71");
  GLADE_HOOKUP_OBJECT (window_sponsors, button_sponsors, "button_sponsors");
  GLADE_HOOKUP_OBJECT (window_sponsors, alignment23, "alignment23");
  GLADE_HOOKUP_OBJECT (window_sponsors, hbox73, "hbox73");
  GLADE_HOOKUP_OBJECT (window_sponsors, image63, "image63");
  GLADE_HOOKUP_OBJECT (window_sponsors, label117, "label117");
  GLADE_HOOKUP_OBJECT (window_sponsors, button_sponsors_wait, "button_sponsors_wait");
  GLADE_HOOKUP_OBJECT (window_sponsors, alignment22, "alignment22");
  GLADE_HOOKUP_OBJECT (window_sponsors, hbox72, "hbox72");
  GLADE_HOOKUP_OBJECT (window_sponsors, image62, "image62");
  GLADE_HOOKUP_OBJECT (window_sponsors, label116, "label116");
  GLADE_HOOKUP_OBJECT_NO_REF (window_sponsors, tooltips, "tooltips");

  return window_sponsors;
}

