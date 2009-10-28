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
//  GTK_HOOKUP_OBJECT (window_contract, builder, "label_contract");

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
  GtkWidget *vbox27;
  GtkWidget *hbox27;
  GtkWidget *label54;
  GtkWidget *entry_mm_file;
  GtkWidget *button_mm_file;
  GtkWidget *button_mm_reload;
  GtkWidget *alignment11;
  GtkWidget *hbox31;
  GtkWidget *image18;
  GtkWidget *label58;
  GtkWidget *button_mm_import;
  GtkWidget *alignment15;
  GtkWidget *hbox37;
  GtkWidget *image22;
  GtkWidget *label62;
  GtkWidget *scrolledwindow5;
  GtkWidget *treeview_mmatches;
  GtkWidget *hbox34;
  GtkWidget *button_mm_save_close;
  GtkWidget *alignment13;
  GtkWidget *hbox33;
  GtkWidget *image20;
  GtkWidget *label60;
  GtkWidget *button_mm_reload_close;
  GtkWidget *alignment14;
  GtkWidget *hbox35;
  GtkWidget *image21;
  GtkWidget *label61;
  GtkTooltips *tooltips;

  tooltips = gtk_tooltips_new ();

  window_mmatches = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_container_set_border_width (GTK_CONTAINER (window_mmatches), 5);
  gtk_window_set_position (GTK_WINDOW (window_mmatches), GTK_WIN_POS_CENTER);
  gtk_window_set_modal (GTK_WINDOW (window_mmatches), TRUE);
  gtk_window_set_default_size (GTK_WINDOW (window_mmatches), 600, 500);

  vbox27 = gtk_vbox_new (FALSE, 3);
  gtk_widget_show (vbox27);
  gtk_container_add (GTK_CONTAINER (window_mmatches), vbox27);

  hbox27 = gtk_hbox_new (FALSE, 3);
  gtk_widget_show (hbox27);
  gtk_box_pack_start (GTK_BOX (vbox27), hbox27, FALSE, TRUE, 0);

  label54 = gtk_label_new (_("Current file"));
  gtk_widget_show (label54);
  gtk_box_pack_start (GTK_BOX (hbox27), label54, FALSE, FALSE, 0);

  entry_mm_file = gtk_entry_new ();
  gtk_widget_show (entry_mm_file);
  gtk_box_pack_start (GTK_BOX (hbox27), entry_mm_file, TRUE, TRUE, 0);

  button_mm_file = gtk_button_new_from_stock ("gtk-open");
  gtk_widget_show (button_mm_file);
  gtk_box_pack_start (GTK_BOX (hbox27), button_mm_file, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, button_mm_file, _("Open a memorable matches file"), NULL);

  button_mm_reload = gtk_button_new ();
  gtk_widget_show (button_mm_reload);
  gtk_box_pack_start (GTK_BOX (hbox27), button_mm_reload, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, button_mm_reload, _("Reload matches from file"), NULL);

  alignment11 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment11);
  gtk_container_add (GTK_CONTAINER (button_mm_reload), alignment11);

  hbox31 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox31);
  gtk_container_add (GTK_CONTAINER (alignment11), hbox31);

  image18 = gtk_image_new_from_stock ("gtk-refresh", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image18);
  gtk_box_pack_start (GTK_BOX (hbox31), image18, FALSE, FALSE, 0);

  label58 = gtk_label_new_with_mnemonic (_("Reload"));
  gtk_widget_show (label58);
  gtk_box_pack_start (GTK_BOX (hbox31), label58, FALSE, FALSE, 0);

  button_mm_import = gtk_button_new ();
  gtk_widget_show (button_mm_import);
  gtk_box_pack_start (GTK_BOX (hbox27), button_mm_import, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, button_mm_import, _("Import the matches from a memorable matches file"), NULL);

  alignment15 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment15);
  gtk_container_add (GTK_CONTAINER (button_mm_import), alignment15);

  hbox37 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox37);
  gtk_container_add (GTK_CONTAINER (alignment15), hbox37);

  image22 = gtk_image_new_from_stock ("gtk-paste", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image22);
  gtk_box_pack_start (GTK_BOX (hbox37), image22, FALSE, FALSE, 0);

  label62 = gtk_label_new_with_mnemonic (_("Import file"));
  gtk_widget_show (label62);
  gtk_box_pack_start (GTK_BOX (hbox37), label62, FALSE, FALSE, 0);

  scrolledwindow5 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow5);
  gtk_box_pack_start (GTK_BOX (vbox27), scrolledwindow5, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow5), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow5), GTK_SHADOW_IN);

  treeview_mmatches = gtk_tree_view_new ();
  gtk_widget_show (treeview_mmatches);
  gtk_container_add (GTK_CONTAINER (scrolledwindow5), treeview_mmatches);
  gtk_tree_view_set_enable_search (GTK_TREE_VIEW (treeview_mmatches), FALSE);

  hbox34 = gtk_hbox_new (FALSE, 3);
  gtk_widget_show (hbox34);
  gtk_box_pack_start (GTK_BOX (vbox27), hbox34, FALSE, FALSE, 0);

  button_mm_save_close = gtk_button_new ();
  gtk_widget_show (button_mm_save_close);
  gtk_box_pack_start (GTK_BOX (hbox34), button_mm_save_close, TRUE, TRUE, 0);
  gtk_tooltips_set_tip (tooltips, button_mm_save_close, _("Save changes to file and close"), NULL);

  alignment13 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment13);
  gtk_container_add (GTK_CONTAINER (button_mm_save_close), alignment13);

  hbox33 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox33);
  gtk_container_add (GTK_CONTAINER (alignment13), hbox33);

  image20 = gtk_image_new_from_stock ("gtk-close", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image20);
  gtk_box_pack_start (GTK_BOX (hbox33), image20, FALSE, FALSE, 0);

  label60 = gtk_label_new_with_mnemonic (_("Save and close"));
  gtk_widget_show (label60);
  gtk_box_pack_start (GTK_BOX (hbox33), label60, FALSE, FALSE, 0);

  button_mm_reload_close = gtk_button_new ();
  gtk_widget_show (button_mm_reload_close);
  gtk_box_pack_start (GTK_BOX (hbox34), button_mm_reload_close, TRUE, TRUE, 0);
  gtk_tooltips_set_tip (tooltips, button_mm_reload_close, _("Reload matches from file and close"), NULL);

  alignment14 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment14);
  gtk_container_add (GTK_CONTAINER (button_mm_reload_close), alignment14);

  hbox35 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox35);
  gtk_container_add (GTK_CONTAINER (alignment14), hbox35);

  image21 = gtk_image_new_from_stock ("gtk-refresh", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image21);
  gtk_box_pack_start (GTK_BOX (hbox35), image21, FALSE, FALSE, 0);

  label61 = gtk_label_new_with_mnemonic (_("Reload and close"));
  gtk_widget_show (label61);
  gtk_box_pack_start (GTK_BOX (hbox35), label61, FALSE, FALSE, 0);

  g_signal_connect ((gpointer) window_mmatches, "delete_event",
                    G_CALLBACK (on_window_mmatches_delete_event),
                    NULL);
  g_signal_connect ((gpointer) button_mm_file, "clicked",
                    G_CALLBACK (on_button_mm_file_clicked),
                    NULL);
  g_signal_connect ((gpointer) button_mm_reload, "clicked",
                    G_CALLBACK (on_button_mm_reload_clicked),
                    NULL);
  g_signal_connect ((gpointer) button_mm_import, "clicked",
                    G_CALLBACK (on_button_mm_import_clicked),
                    NULL);
  g_signal_connect ((gpointer) treeview_mmatches, "button_press_event",
                    G_CALLBACK (on_treeview_mmatches_button_press_event),
                    NULL);
  g_signal_connect ((gpointer) button_mm_save_close, "clicked",
                    G_CALLBACK (on_button_mm_save_close_clicked),
                    NULL);
  g_signal_connect ((gpointer) button_mm_reload_close, "clicked",
                    G_CALLBACK (on_button_mm_reload_close_clicked),
                    NULL);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (window_mmatches, window_mmatches, "window_mmatches");
  GLADE_HOOKUP_OBJECT (window_mmatches, vbox27, "vbox27");
  GLADE_HOOKUP_OBJECT (window_mmatches, hbox27, "hbox27");
  GLADE_HOOKUP_OBJECT (window_mmatches, label54, "label54");
  GLADE_HOOKUP_OBJECT (window_mmatches, entry_mm_file, "entry_mm_file");
  GLADE_HOOKUP_OBJECT (window_mmatches, button_mm_file, "button_mm_file");
  GLADE_HOOKUP_OBJECT (window_mmatches, button_mm_reload, "button_mm_reload");
  GLADE_HOOKUP_OBJECT (window_mmatches, alignment11, "alignment11");
  GLADE_HOOKUP_OBJECT (window_mmatches, hbox31, "hbox31");
  GLADE_HOOKUP_OBJECT (window_mmatches, image18, "image18");
  GLADE_HOOKUP_OBJECT (window_mmatches, label58, "label58");
  GLADE_HOOKUP_OBJECT (window_mmatches, button_mm_import, "button_mm_import");
  GLADE_HOOKUP_OBJECT (window_mmatches, alignment15, "alignment15");
  GLADE_HOOKUP_OBJECT (window_mmatches, hbox37, "hbox37");
  GLADE_HOOKUP_OBJECT (window_mmatches, image22, "image22");
  GLADE_HOOKUP_OBJECT (window_mmatches, label62, "label62");
  GLADE_HOOKUP_OBJECT (window_mmatches, scrolledwindow5, "scrolledwindow5");
  GLADE_HOOKUP_OBJECT (window_mmatches, treeview_mmatches, "treeview_mmatches");
  GLADE_HOOKUP_OBJECT (window_mmatches, hbox34, "hbox34");
  GLADE_HOOKUP_OBJECT (window_mmatches, button_mm_save_close, "button_mm_save_close");
  GLADE_HOOKUP_OBJECT (window_mmatches, alignment13, "alignment13");
  GLADE_HOOKUP_OBJECT (window_mmatches, hbox33, "hbox33");
  GLADE_HOOKUP_OBJECT (window_mmatches, image20, "image20");
  GLADE_HOOKUP_OBJECT (window_mmatches, label60, "label60");
  GLADE_HOOKUP_OBJECT (window_mmatches, button_mm_reload_close, "button_mm_reload_close");
  GLADE_HOOKUP_OBJECT (window_mmatches, alignment14, "alignment14");
  GLADE_HOOKUP_OBJECT (window_mmatches, hbox35, "hbox35");
  GLADE_HOOKUP_OBJECT (window_mmatches, image21, "image21");
  GLADE_HOOKUP_OBJECT (window_mmatches, label61, "label61");
  GLADE_HOOKUP_OBJECT_NO_REF (window_mmatches, tooltips, "tooltips");

  return window_mmatches;
}

