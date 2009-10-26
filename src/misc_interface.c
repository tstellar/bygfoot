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
  //GLADE_HOOKUP_OBJECT_NO_REF (window_startup, window_startup, "window_startup");
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
  GtkWidget *button_font_sel_ok;
  GtkWidget *button_font_sel_cancel;
  GtkWidget *button_font_sel_apply;
  GtkWidget *font_sel;

  window_font_sel = gtk_font_selection_dialog_new (_("Select font"));
  gtk_container_set_border_width (GTK_CONTAINER (window_font_sel), 5);
  gtk_window_set_position (GTK_WINDOW (window_font_sel), GTK_WIN_POS_CENTER);
  gtk_window_set_modal (GTK_WINDOW (window_font_sel), TRUE);
  gtk_window_set_type_hint (GTK_WINDOW (window_font_sel), GDK_WINDOW_TYPE_HINT_DIALOG);

  button_font_sel_ok = GTK_FONT_SELECTION_DIALOG (window_font_sel)->ok_button;
  gtk_widget_show (button_font_sel_ok);
  GTK_WIDGET_SET_FLAGS (button_font_sel_ok, GTK_CAN_DEFAULT);

  button_font_sel_cancel = GTK_FONT_SELECTION_DIALOG (window_font_sel)->cancel_button;
  gtk_widget_show (button_font_sel_cancel);
  GTK_WIDGET_SET_FLAGS (button_font_sel_cancel, GTK_CAN_DEFAULT);

  button_font_sel_apply = GTK_FONT_SELECTION_DIALOG (window_font_sel)->apply_button;
  gtk_widget_show (button_font_sel_apply);
  GTK_WIDGET_SET_FLAGS (button_font_sel_apply, GTK_CAN_DEFAULT);

  font_sel = GTK_FONT_SELECTION_DIALOG (window_font_sel)->fontsel;
  gtk_widget_show (font_sel);
  gtk_container_set_border_width (GTK_CONTAINER (font_sel), 4);

  g_signal_connect ((gpointer) window_font_sel, "delete_event",
                    G_CALLBACK (on_button_font_sel_cancel_clicked),
                    NULL);
  g_signal_connect ((gpointer) button_font_sel_ok, "clicked",
                    G_CALLBACK (on_button_font_sel_ok_clicked),
                    NULL);
  g_signal_connect ((gpointer) button_font_sel_cancel, "clicked",
                    G_CALLBACK (on_button_font_sel_cancel_clicked),
                    NULL);
  g_signal_connect ((gpointer) button_font_sel_apply, "clicked",
                    G_CALLBACK (on_button_font_sel_apply_clicked),
                    NULL);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (window_font_sel, window_font_sel, "window_font_sel");
  GLADE_HOOKUP_OBJECT_NO_REF (window_font_sel, button_font_sel_ok, "button_font_sel_ok");
  GLADE_HOOKUP_OBJECT_NO_REF (window_font_sel, button_font_sel_cancel, "button_font_sel_cancel");
  GLADE_HOOKUP_OBJECT_NO_REF (window_font_sel, button_font_sel_apply, "button_font_sel_apply");
  GLADE_HOOKUP_OBJECT_NO_REF (window_font_sel, font_sel, "font_sel");

  return window_font_sel;
}

GtkWidget*
create_window_live (void)
{
  GtkWidget *window_live;
  GtkWidget *vbox36;
  GtkWidget *progressbar_live;
  GtkWidget *hbox50;
  GtkWidget *vbox38;
  GtkWidget *hbox57;
  GtkWidget *eventbox_poss0;
  GtkWidget *label78;
  GtkWidget *scrolledwindow8;
  GtkWidget *treeview_result;
  GtkWidget *eventbox_poss1;
  GtkWidget *label79;
  GtkWidget *hscale_area;
  GtkWidget *notebook1;
  GtkWidget *scrolledwindow9;
  GtkWidget *treeview_commentary;
  GtkWidget *label80;
  GtkWidget *scrolledwindow11;
  GtkWidget *treeview_stats;
  GtkWidget *label81;
  GtkWidget *scrolledwindow13;
  GtkWidget *viewport1;
  GtkWidget *vbox49;
  GtkWidget *hbox75;
  GtkWidget *image_lg_opp_style;
  GtkWidget *image_lg_opp_boost;
  GtkWidget *label_lg_formation;
  GtkWidget *hbox76;
  GtkWidget *label120;
  GtkWidget *label_lg_avskill;
  GtkWidget *treeview_lg_opponent;
  GtkWidget *label118;
  GtkWidget *hbox48;
  GtkWidget *button_pause;
  GtkWidget *alignment16;
  GtkWidget *hbox51;
  GtkWidget *image56;
  GtkWidget *label72;
  GtkWidget *button_resume;
  GtkWidget *alignment17;
  GtkWidget *hbox52;
  GtkWidget *image57;
  GtkWidget *label73;
  GtkWidget *button_live_close;
  GtkWidget *table2;
  GtkObject *spinbutton_speed_adj;
  GtkWidget *spinbutton_speed;
  GtkWidget *label112;
  GtkWidget *label113;
  GtkObject *spinbutton_verbosity_adj;
  GtkWidget *spinbutton_verbosity;
  GtkWidget *vseparator1;
  GtkWidget *eventbox_lg_boost;
  GtkWidget *image_lg_boost;
  GtkWidget *eventbox_lg_style;
  GtkWidget *image_lg_style;
  GtkAccelGroup *accel_group;
  GtkTooltips *tooltips;

  tooltips = gtk_tooltips_new ();

  accel_group = gtk_accel_group_new ();

  window_live = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_container_set_border_width (GTK_CONTAINER (window_live), 6);
  gtk_window_set_title (GTK_WINDOW (window_live), _("Live game"));
  gtk_window_set_position (GTK_WINDOW (window_live), GTK_WIN_POS_CENTER);
  gtk_window_set_modal (GTK_WINDOW (window_live), TRUE);
  gtk_window_set_default_size (GTK_WINDOW (window_live), 620, 550);

  vbox36 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox36);
  gtk_container_add (GTK_CONTAINER (window_live), vbox36);

  progressbar_live = gtk_progress_bar_new ();
  gtk_widget_show (progressbar_live);
  gtk_box_pack_start (GTK_BOX (vbox36), progressbar_live, FALSE, FALSE, 0);

  hbox50 = gtk_hbox_new (FALSE, 3);
  gtk_widget_show (hbox50);
  gtk_box_pack_start (GTK_BOX (vbox36), hbox50, TRUE, TRUE, 0);

  vbox38 = gtk_vbox_new (FALSE, 3);
  gtk_widget_show (vbox38);
  gtk_box_pack_start (GTK_BOX (hbox50), vbox38, TRUE, TRUE, 0);

  hbox57 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox57);
  gtk_box_pack_start (GTK_BOX (vbox38), hbox57, FALSE, FALSE, 0);

  eventbox_poss0 = gtk_event_box_new ();
  gtk_widget_show (eventbox_poss0);
  gtk_box_pack_start (GTK_BOX (hbox57), eventbox_poss0, FALSE, TRUE, 0);

  label78 = gtk_label_new ("");
  gtk_widget_show (label78);
  gtk_container_add (GTK_CONTAINER (eventbox_poss0), label78);
  gtk_misc_set_padding (GTK_MISC (label78), 5, 0);

  scrolledwindow8 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow8);
  gtk_box_pack_start (GTK_BOX (hbox57), scrolledwindow8, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow8), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow8), GTK_SHADOW_ETCHED_IN);

  treeview_result = gtk_tree_view_new ();
  gtk_widget_show (treeview_result);
  gtk_container_add (GTK_CONTAINER (scrolledwindow8), treeview_result);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeview_result), FALSE);

  eventbox_poss1 = gtk_event_box_new ();
  gtk_widget_show (eventbox_poss1);
  gtk_box_pack_start (GTK_BOX (hbox57), eventbox_poss1, FALSE, TRUE, 0);

  label79 = gtk_label_new ("");
  gtk_widget_show (label79);
  gtk_container_add (GTK_CONTAINER (eventbox_poss1), label79);
  gtk_misc_set_padding (GTK_MISC (label79), 5, 0);

  hscale_area = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (5, 0, 10, 0, 0, 0)));
  gtk_widget_show (hscale_area);
  gtk_box_pack_start (GTK_BOX (vbox38), hscale_area, FALSE, TRUE, 0);
  gtk_scale_set_draw_value (GTK_SCALE (hscale_area), FALSE);

  notebook1 = gtk_notebook_new ();
  gtk_widget_show (notebook1);
  gtk_box_pack_start (GTK_BOX (vbox38), notebook1, TRUE, TRUE, 0);

  scrolledwindow9 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow9);
  gtk_container_add (GTK_CONTAINER (notebook1), scrolledwindow9);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow9), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow9), GTK_SHADOW_ETCHED_IN);

  treeview_commentary = gtk_tree_view_new ();
  gtk_widget_show (treeview_commentary);
  gtk_container_add (GTK_CONTAINER (scrolledwindow9), treeview_commentary);

  label80 = gtk_label_new (_("Commentary"));
  gtk_widget_show (label80);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 0), label80);

  scrolledwindow11 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow11);
  gtk_container_add (GTK_CONTAINER (notebook1), scrolledwindow11);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow11), GTK_SHADOW_IN);

  treeview_stats = gtk_tree_view_new ();
  gtk_widget_show (treeview_stats);
  gtk_container_add (GTK_CONTAINER (scrolledwindow11), treeview_stats);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeview_stats), FALSE);

  label81 = gtk_label_new (_("Statistics"));
  gtk_widget_show (label81);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 1), label81);

  scrolledwindow13 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow13);
  gtk_container_add (GTK_CONTAINER (notebook1), scrolledwindow13);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow13), GTK_SHADOW_IN);

  viewport1 = gtk_viewport_new (NULL, NULL);
  gtk_widget_show (viewport1);
  gtk_container_add (GTK_CONTAINER (scrolledwindow13), viewport1);

  vbox49 = gtk_vbox_new (FALSE, 3);
  gtk_widget_show (vbox49);
  gtk_container_add (GTK_CONTAINER (viewport1), vbox49);

  hbox75 = gtk_hbox_new (FALSE, 10);
  gtk_widget_show (hbox75);
  gtk_box_pack_start (GTK_BOX (vbox49), hbox75, FALSE, FALSE, 0);

  image_lg_opp_style = create_pixmap (window_live, NULL);
  gtk_widget_show (image_lg_opp_style);
  gtk_box_pack_start (GTK_BOX (hbox75), image_lg_opp_style, FALSE, FALSE, 0);

  image_lg_opp_boost = create_pixmap (window_live, NULL);
  gtk_widget_show (image_lg_opp_boost);
  gtk_box_pack_start (GTK_BOX (hbox75), image_lg_opp_boost, FALSE, FALSE, 0);

  label_lg_formation = gtk_label_new ("");
  gtk_widget_show (label_lg_formation);
  gtk_box_pack_start (GTK_BOX (hbox75), label_lg_formation, FALSE, FALSE, 0);

  hbox76 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox76);
  gtk_box_pack_start (GTK_BOX (hbox75), hbox76, FALSE, FALSE, 0);

  label120 = gtk_label_new (_("Av. skill: "));
  gtk_widget_show (label120);
  gtk_box_pack_start (GTK_BOX (hbox76), label120, FALSE, FALSE, 0);

  label_lg_avskill = gtk_label_new ("");
  gtk_widget_show (label_lg_avskill);
  gtk_box_pack_start (GTK_BOX (hbox76), label_lg_avskill, FALSE, FALSE, 0);

  treeview_lg_opponent = gtk_tree_view_new ();
  gtk_widget_show (treeview_lg_opponent);
  gtk_box_pack_start (GTK_BOX (vbox49), treeview_lg_opponent, TRUE, TRUE, 0);

  label118 = gtk_label_new (_("Opponent player list"));
  gtk_widget_show (label118);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 2), label118);

  hbox48 = gtk_hbox_new (FALSE, 4);
  gtk_widget_show (hbox48);
  gtk_box_pack_start (GTK_BOX (vbox36), hbox48, FALSE, TRUE, 5);

  button_pause = gtk_button_new ();
  gtk_widget_show (button_pause);
  gtk_box_pack_start (GTK_BOX (hbox48), button_pause, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, button_pause, _("Esc / Return / Space"), NULL);
  gtk_widget_add_accelerator (button_pause, "clicked", accel_group,
                              GDK_Escape, (GdkModifierType) 0,
                              GTK_ACCEL_VISIBLE);

  alignment16 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment16);
  gtk_container_add (GTK_CONTAINER (button_pause), alignment16);

  hbox51 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox51);
  gtk_container_add (GTK_CONTAINER (alignment16), hbox51);

  image56 = gtk_image_new_from_stock ("gtk-no", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image56);
  gtk_box_pack_start (GTK_BOX (hbox51), image56, FALSE, FALSE, 0);

  label72 = gtk_label_new_with_mnemonic (_("_Pause"));
  gtk_widget_show (label72);
  gtk_box_pack_start (GTK_BOX (hbox51), label72, FALSE, FALSE, 0);

  button_resume = gtk_button_new ();
  gtk_widget_show (button_resume);
  gtk_box_pack_start (GTK_BOX (hbox48), button_resume, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, button_resume, _("Return / Space"), NULL);

  alignment17 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment17);
  gtk_container_add (GTK_CONTAINER (button_resume), alignment17);

  hbox52 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox52);
  gtk_container_add (GTK_CONTAINER (alignment17), hbox52);

  image57 = gtk_image_new_from_stock ("gtk-yes", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image57);
  gtk_box_pack_start (GTK_BOX (hbox52), image57, FALSE, FALSE, 0);

  label73 = gtk_label_new_with_mnemonic (_("_Resume"));
  gtk_widget_show (label73);
  gtk_box_pack_start (GTK_BOX (hbox52), label73, FALSE, FALSE, 0);

  button_live_close = gtk_button_new_from_stock ("gtk-close");
  gtk_widget_show (button_live_close);
  gtk_box_pack_start (GTK_BOX (hbox48), button_live_close, FALSE, FALSE, 0);
  gtk_widget_set_sensitive (button_live_close, FALSE);
  gtk_tooltips_set_tip (tooltips, button_live_close, _("Return / Space"), NULL);

  table2 = gtk_table_new (2, 2, FALSE);
  gtk_widget_show (table2);
  gtk_box_pack_end (GTK_BOX (hbox48), table2, FALSE, TRUE, 0);
  gtk_table_set_row_spacings (GTK_TABLE (table2), 1);
  gtk_table_set_col_spacings (GTK_TABLE (table2), 3);

  spinbutton_speed_adj = gtk_adjustment_new (0, 0, 30, 1, 10, 0);
  spinbutton_speed = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton_speed_adj), 1, 0);
  gtk_widget_show (spinbutton_speed);
  gtk_table_attach (GTK_TABLE (table2), spinbutton_speed, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, spinbutton_speed, _("Change live game speed. The higher the value the faster the live game."), NULL);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbutton_speed), TRUE);

  label112 = gtk_label_new (_("Speed"));
  gtk_widget_show (label112);
  gtk_table_attach (GTK_TABLE (table2), label112, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label112), GTK_JUSTIFY_RIGHT);

  label113 = gtk_label_new (_("Verbosity"));
  gtk_widget_show (label113);
  gtk_table_attach (GTK_TABLE (table2), label113, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  spinbutton_verbosity_adj = gtk_adjustment_new (1, 1, 7, 1, 10, 0);
  spinbutton_verbosity = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton_verbosity_adj), 1, 0);
  gtk_widget_show (spinbutton_verbosity);
  gtk_table_attach (GTK_TABLE (table2), spinbutton_verbosity, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, spinbutton_verbosity, _("Change commentary verbosity. The higher this value the more events you can see."), NULL);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbutton_verbosity), TRUE);

  vseparator1 = gtk_vseparator_new ();
  gtk_widget_show (vseparator1);
  gtk_box_pack_end (GTK_BOX (hbox48), vseparator1, FALSE, FALSE, 0);
  gtk_widget_set_size_request (vseparator1, 10, 1);

  eventbox_lg_boost = gtk_event_box_new ();
  gtk_widget_show (eventbox_lg_boost);
  gtk_box_pack_end (GTK_BOX (hbox48), eventbox_lg_boost, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, eventbox_lg_boost, _("Right click and left click to change boost state"), NULL);

  image_lg_boost = create_pixmap (window_live, "boost_off.png");
  gtk_widget_show (image_lg_boost);
  gtk_container_add (GTK_CONTAINER (eventbox_lg_boost), image_lg_boost);

  eventbox_lg_style = gtk_event_box_new ();
  gtk_widget_show (eventbox_lg_style);
  gtk_box_pack_end (GTK_BOX (hbox48), eventbox_lg_style, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, eventbox_lg_style, _("Right click and left click to change your playing style"), NULL);

  image_lg_style = create_pixmap (window_live, "style_bal.png");
  gtk_widget_show (image_lg_style);
  gtk_container_add (GTK_CONTAINER (eventbox_lg_style), image_lg_style);

  g_signal_connect ((gpointer) window_live, "delete_event",
                    G_CALLBACK (on_live_window_delete_event),
                    NULL);
  g_signal_connect ((gpointer) button_pause, "clicked",
                    G_CALLBACK (on_button_pause_clicked),
                    NULL);
  g_signal_connect ((gpointer) button_resume, "clicked",
                    G_CALLBACK (on_button_resume_clicked),
                    NULL);
  g_signal_connect ((gpointer) button_live_close, "clicked",
                    G_CALLBACK (on_button_live_close_clicked),
                    NULL);
  g_signal_connect ((gpointer) spinbutton_speed, "value_changed",
                    G_CALLBACK (on_spinbutton_speed_value_changed),
                    NULL);
  g_signal_connect ((gpointer) spinbutton_speed, "button_press_event",
                    G_CALLBACK (on_spinbutton_speed_button_press_event),
                    NULL);
  g_signal_connect ((gpointer) spinbutton_verbosity, "value_changed",
                    G_CALLBACK (on_spinbutton_verbosity_value_changed),
                    NULL);
  g_signal_connect ((gpointer) eventbox_lg_boost, "button_press_event",
                    G_CALLBACK (on_eventbox_lg_boost_button_press_event),
                    NULL);
  g_signal_connect ((gpointer) eventbox_lg_style, "button_press_event",
                    G_CALLBACK (on_eventbox_lg_style_button_press_event),
                    NULL);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (window_live, window_live, "window_live");
  GLADE_HOOKUP_OBJECT (window_live, vbox36, "vbox36");
  GLADE_HOOKUP_OBJECT (window_live, progressbar_live, "progressbar_live");
  GLADE_HOOKUP_OBJECT (window_live, hbox50, "hbox50");
  GLADE_HOOKUP_OBJECT (window_live, vbox38, "vbox38");
  GLADE_HOOKUP_OBJECT (window_live, hbox57, "hbox57");
  GLADE_HOOKUP_OBJECT (window_live, eventbox_poss0, "eventbox_poss0");
  GLADE_HOOKUP_OBJECT (window_live, label78, "label78");
  GLADE_HOOKUP_OBJECT (window_live, scrolledwindow8, "scrolledwindow8");
  GLADE_HOOKUP_OBJECT (window_live, treeview_result, "treeview_result");
  GLADE_HOOKUP_OBJECT (window_live, eventbox_poss1, "eventbox_poss1");
  GLADE_HOOKUP_OBJECT (window_live, label79, "label79");
  GLADE_HOOKUP_OBJECT (window_live, hscale_area, "hscale_area");
  GLADE_HOOKUP_OBJECT (window_live, notebook1, "notebook1");
  GLADE_HOOKUP_OBJECT (window_live, scrolledwindow9, "scrolledwindow9");
  GLADE_HOOKUP_OBJECT (window_live, treeview_commentary, "treeview_commentary");
  GLADE_HOOKUP_OBJECT (window_live, label80, "label80");
  GLADE_HOOKUP_OBJECT (window_live, scrolledwindow11, "scrolledwindow11");
  GLADE_HOOKUP_OBJECT (window_live, treeview_stats, "treeview_stats");
  GLADE_HOOKUP_OBJECT (window_live, label81, "label81");
  GLADE_HOOKUP_OBJECT (window_live, scrolledwindow13, "scrolledwindow13");
  GLADE_HOOKUP_OBJECT (window_live, viewport1, "viewport1");
  GLADE_HOOKUP_OBJECT (window_live, vbox49, "vbox49");
  GLADE_HOOKUP_OBJECT (window_live, hbox75, "hbox75");
  GLADE_HOOKUP_OBJECT (window_live, image_lg_opp_style, "image_lg_opp_style");
  GLADE_HOOKUP_OBJECT (window_live, image_lg_opp_boost, "image_lg_opp_boost");
  GLADE_HOOKUP_OBJECT (window_live, label_lg_formation, "label_lg_formation");
  GLADE_HOOKUP_OBJECT (window_live, hbox76, "hbox76");
  GLADE_HOOKUP_OBJECT (window_live, label120, "label120");
  GLADE_HOOKUP_OBJECT (window_live, label_lg_avskill, "label_lg_avskill");
  GLADE_HOOKUP_OBJECT (window_live, treeview_lg_opponent, "treeview_lg_opponent");
  GLADE_HOOKUP_OBJECT (window_live, label118, "label118");
  GLADE_HOOKUP_OBJECT (window_live, hbox48, "hbox48");
  GLADE_HOOKUP_OBJECT (window_live, button_pause, "button_pause");
  GLADE_HOOKUP_OBJECT (window_live, alignment16, "alignment16");
  GLADE_HOOKUP_OBJECT (window_live, hbox51, "hbox51");
  GLADE_HOOKUP_OBJECT (window_live, image56, "image56");
  GLADE_HOOKUP_OBJECT (window_live, label72, "label72");
  GLADE_HOOKUP_OBJECT (window_live, button_resume, "button_resume");
  GLADE_HOOKUP_OBJECT (window_live, alignment17, "alignment17");
  GLADE_HOOKUP_OBJECT (window_live, hbox52, "hbox52");
  GLADE_HOOKUP_OBJECT (window_live, image57, "image57");
  GLADE_HOOKUP_OBJECT (window_live, label73, "label73");
  GLADE_HOOKUP_OBJECT (window_live, button_live_close, "button_live_close");
  GLADE_HOOKUP_OBJECT (window_live, table2, "table2");
  GLADE_HOOKUP_OBJECT (window_live, spinbutton_speed, "spinbutton_speed");
  GLADE_HOOKUP_OBJECT (window_live, label112, "label112");
  GLADE_HOOKUP_OBJECT (window_live, label113, "label113");
  GLADE_HOOKUP_OBJECT (window_live, spinbutton_verbosity, "spinbutton_verbosity");
  GLADE_HOOKUP_OBJECT (window_live, vseparator1, "vseparator1");
  GLADE_HOOKUP_OBJECT (window_live, eventbox_lg_boost, "eventbox_lg_boost");
  GLADE_HOOKUP_OBJECT (window_live, image_lg_boost, "image_lg_boost");
  GLADE_HOOKUP_OBJECT (window_live, eventbox_lg_style, "eventbox_lg_style");
  GLADE_HOOKUP_OBJECT (window_live, image_lg_style, "image_lg_style");
  GLADE_HOOKUP_OBJECT_NO_REF (window_live, tooltips, "tooltips");

  gtk_window_add_accel_group (GTK_WINDOW (window_live), accel_group);

  return window_live;
}

GtkWidget*
create_window_stadium (void)
{
  GtkWidget *window_stadium;
  GtkWidget *vbox39;
  GtkWidget *label_stadium_name;
  GtkWidget *table1;
  GtkWidget *hbox69;
  GtkWidget *label_capacity;
  GtkWidget *label102;
  GtkWidget *label_average_attendance;
  GtkWidget *progressbar_average_attendance;
  GtkWidget *label106;
  GtkWidget *label_stadium_status;
  GtkWidget *label101;
  GtkWidget *progressbar_safety;
  GtkWidget *label103;
  GtkWidget *label109;
  GtkWidget *label111;
  GtkWidget *hseparator17;
  GtkWidget *table3;
  GtkObject *spin_ticket_price_adj;
  GtkWidget *spin_ticket_price;
  GtkWidget *label124;
  GtkWidget *label123;
  GtkWidget *hseparator14;
  GtkWidget *vbox43;
  GtkWidget *label90;
  GtkWidget *hbox61;
  GtkWidget *vbox44;
  GtkWidget *label91;
  GtkObject *spinbutton_capacity_adj;
  GtkWidget *spinbutton_capacity;
  GtkWidget *vbox46;
  GtkWidget *label93;
  GtkWidget *label_costs_capacity;
  GtkWidget *label95;
  GtkWidget *label_duration_capacity;
  GtkWidget *vbox45;
  GtkWidget *label92;
  GtkObject *spinbutton_safety_adj;
  GtkWidget *spinbutton_safety;
  GtkWidget *vbox47;
  GtkWidget *label94;
  GtkWidget *label_costs_safety;
  GtkWidget *label96;
  GtkWidget *label_duration_safety;
  GtkWidget *hseparator15;
  GtkWidget *button_stadium_ok;
  GtkWidget *alignment20;
  GtkWidget *hbox67;
  GtkWidget *image60;
  GtkWidget *label107;
  GtkWidget *button_stadium_cancel;
  GtkAccelGroup *accel_group;
  GtkTooltips *tooltips;

  tooltips = gtk_tooltips_new ();

  accel_group = gtk_accel_group_new ();

  window_stadium = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_container_set_border_width (GTK_CONTAINER (window_stadium), 5);
  gtk_window_set_position (GTK_WINDOW (window_stadium), GTK_WIN_POS_CENTER);
  gtk_window_set_modal (GTK_WINDOW (window_stadium), TRUE);

  vbox39 = gtk_vbox_new (FALSE, 5);
  gtk_widget_show (vbox39);
  gtk_container_add (GTK_CONTAINER (window_stadium), vbox39);

  label_stadium_name = gtk_label_new ("");
  gtk_widget_show (label_stadium_name);
  gtk_box_pack_start (GTK_BOX (vbox39), label_stadium_name, FALSE, FALSE, 0);

  table1 = gtk_table_new (5, 2, TRUE);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (vbox39), table1, FALSE, FALSE, 0);

  hbox69 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox69);
  gtk_table_attach (GTK_TABLE (table1), hbox69, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  label_capacity = gtk_label_new ("");
  gtk_widget_show (label_capacity);
  gtk_box_pack_start (GTK_BOX (hbox69), label_capacity, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC (label_capacity), 0, 0.5);

  label102 = gtk_label_new (_(" seats"));
  gtk_widget_show (label102);
  gtk_box_pack_start (GTK_BOX (hbox69), label102, FALSE, FALSE, 0);

  label_average_attendance = gtk_label_new ("");
  gtk_widget_show (label_average_attendance);
  gtk_table_attach (GTK_TABLE (table1), label_average_attendance, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label_average_attendance), 0, 0.5);

  progressbar_average_attendance = gtk_progress_bar_new ();
  gtk_widget_show (progressbar_average_attendance);
  gtk_table_attach (GTK_TABLE (table1), progressbar_average_attendance, 1, 2, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label106 = gtk_label_new (_("Status:"));
  gtk_widget_show (label106);
  gtk_table_attach (GTK_TABLE (table1), label106, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label106), 0, 0.5);

  label_stadium_status = gtk_label_new ("");
  gtk_widget_show (label_stadium_status);
  gtk_table_attach (GTK_TABLE (table1), label_stadium_status, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label_stadium_status), 0, 0.5);

  label101 = gtk_label_new (_("Current capacity"));
  gtk_widget_show (label101);
  gtk_table_attach (GTK_TABLE (table1), label101, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label101), 0, 0.5);

  progressbar_safety = gtk_progress_bar_new ();
  gtk_widget_show (progressbar_safety);
  gtk_table_attach (GTK_TABLE (table1), progressbar_safety, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label103 = gtk_label_new (_("Current safety"));
  gtk_widget_show (label103);
  gtk_table_attach (GTK_TABLE (table1), label103, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label103), 0, 0.5);

  label109 = gtk_label_new (_("Average attendance"));
  gtk_widget_show (label109);
  gtk_table_attach (GTK_TABLE (table1), label109, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label109), 0, 0.5);

  label111 = gtk_label_new (_("Average attendance %"));
  gtk_widget_show (label111);
  gtk_table_attach (GTK_TABLE (table1), label111, 0, 1, 4, 5,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label111), 0, 0.5);

  hseparator17 = gtk_hseparator_new ();
  gtk_widget_show (hseparator17);
  gtk_box_pack_start (GTK_BOX (vbox39), hseparator17, FALSE, FALSE, 0);

  table3 = gtk_table_new (2, 2, TRUE);
  gtk_widget_show (table3);
  gtk_box_pack_start (GTK_BOX (vbox39), table3, FALSE, FALSE, 0);
  gtk_table_set_row_spacings (GTK_TABLE (table3), 3);
  gtk_table_set_col_spacings (GTK_TABLE (table3), 3);

  spin_ticket_price_adj = gtk_adjustment_new (1, 0, 100, 0.10000000149, 10, 0);
  spin_ticket_price = gtk_spin_button_new (GTK_ADJUSTMENT (spin_ticket_price_adj), 1, 2);
  gtk_widget_show (spin_ticket_price);
  gtk_table_attach (GTK_TABLE (table3), spin_ticket_price, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spin_ticket_price), TRUE);

  label124 = gtk_label_new (_("Change ticket price"));
  gtk_widget_show (label124);
  gtk_table_attach (GTK_TABLE (table3), label124, 0, 2, 0, 1,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label124), GTK_JUSTIFY_CENTER);
  gtk_misc_set_alignment (GTK_MISC (label124), 0, 0.5);

  label123 = gtk_label_new (_("Price per ticket"));
  gtk_widget_show (label123);
  gtk_table_attach (GTK_TABLE (table3), label123, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label123), 0, 0);

  hseparator14 = gtk_hseparator_new ();
  gtk_widget_show (hseparator14);
  gtk_box_pack_start (GTK_BOX (vbox39), hseparator14, FALSE, FALSE, 0);
  gtk_widget_set_size_request (hseparator14, 1, 10);

  vbox43 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox43);
  gtk_box_pack_start (GTK_BOX (vbox39), vbox43, FALSE, FALSE, 0);

  label90 = gtk_label_new (_("Increase"));
  gtk_widget_show (label90);
  gtk_box_pack_start (GTK_BOX (vbox43), label90, FALSE, FALSE, 0);

  hbox61 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox61);
  gtk_box_pack_start (GTK_BOX (vbox43), hbox61, FALSE, FALSE, 0);

  vbox44 = gtk_vbox_new (FALSE, 3);
  gtk_widget_show (vbox44);
  gtk_box_pack_start (GTK_BOX (hbox61), vbox44, TRUE, TRUE, 0);

  label91 = gtk_label_new (_("Capacity (seats)"));
  gtk_widget_show (label91);
  gtk_box_pack_start (GTK_BOX (vbox44), label91, FALSE, FALSE, 0);

  spinbutton_capacity_adj = gtk_adjustment_new (0, 0, 10000, 1, 100, 0);
  spinbutton_capacity = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton_capacity_adj), 1, 0);
  gtk_widget_show (spinbutton_capacity);
  gtk_box_pack_start (GTK_BOX (vbox44), spinbutton_capacity, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, spinbutton_capacity, _("Right-click to set to 0"), NULL);

  vbox46 = gtk_vbox_new (FALSE, 3);
  gtk_widget_show (vbox46);
  gtk_box_pack_start (GTK_BOX (vbox44), vbox46, FALSE, FALSE, 0);

  label93 = gtk_label_new (_("Costs"));
  gtk_widget_show (label93);
  gtk_box_pack_start (GTK_BOX (vbox46), label93, FALSE, FALSE, 0);

  label_costs_capacity = gtk_label_new (_("0"));
  gtk_widget_show (label_costs_capacity);
  gtk_box_pack_start (GTK_BOX (vbox46), label_costs_capacity, FALSE, FALSE, 0);

  label95 = gtk_label_new (_("Expected duration"));
  gtk_widget_show (label95);
  gtk_box_pack_start (GTK_BOX (vbox46), label95, FALSE, FALSE, 0);

  label_duration_capacity = gtk_label_new (_("0"));
  gtk_widget_show (label_duration_capacity);
  gtk_box_pack_start (GTK_BOX (vbox46), label_duration_capacity, FALSE, FALSE, 0);

  vbox45 = gtk_vbox_new (FALSE, 3);
  gtk_widget_show (vbox45);
  gtk_box_pack_start (GTK_BOX (hbox61), vbox45, TRUE, TRUE, 0);

  label92 = gtk_label_new (_("Safety (%)"));
  gtk_widget_show (label92);
  gtk_box_pack_start (GTK_BOX (vbox45), label92, FALSE, FALSE, 0);

  spinbutton_safety_adj = gtk_adjustment_new (0, 0, 100, 1, 5, 0);
  spinbutton_safety = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton_safety_adj), 1, 0);
  gtk_widget_show (spinbutton_safety);
  gtk_box_pack_start (GTK_BOX (vbox45), spinbutton_safety, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, spinbutton_safety, _("Right-click to set to 0"), NULL);

  vbox47 = gtk_vbox_new (FALSE, 3);
  gtk_widget_show (vbox47);
  gtk_box_pack_start (GTK_BOX (vbox45), vbox47, FALSE, FALSE, 0);

  label94 = gtk_label_new (_("Costs"));
  gtk_widget_show (label94);
  gtk_box_pack_start (GTK_BOX (vbox47), label94, FALSE, FALSE, 0);

  label_costs_safety = gtk_label_new (_("0"));
  gtk_widget_show (label_costs_safety);
  gtk_box_pack_start (GTK_BOX (vbox47), label_costs_safety, FALSE, FALSE, 0);

  label96 = gtk_label_new (_("Expected duration"));
  gtk_widget_show (label96);
  gtk_box_pack_start (GTK_BOX (vbox47), label96, FALSE, FALSE, 0);

  label_duration_safety = gtk_label_new (_("0"));
  gtk_widget_show (label_duration_safety);
  gtk_box_pack_start (GTK_BOX (vbox47), label_duration_safety, FALSE, FALSE, 0);

  hseparator15 = gtk_hseparator_new ();
  gtk_widget_show (hseparator15);
  gtk_box_pack_start (GTK_BOX (vbox39), hseparator15, FALSE, FALSE, 0);
  gtk_widget_set_size_request (hseparator15, 1, 10);

  button_stadium_ok = gtk_button_new ();
  gtk_widget_show (button_stadium_ok);
  gtk_box_pack_start (GTK_BOX (vbox39), button_stadium_ok, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, button_stadium_ok, _("Return"), NULL);
  gtk_widget_add_accelerator (button_stadium_ok, "clicked", accel_group,
                              GDK_Return, (GdkModifierType) 0,
                              GTK_ACCEL_VISIBLE);

  alignment20 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment20);
  gtk_container_add (GTK_CONTAINER (button_stadium_ok), alignment20);

  hbox67 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox67);
  gtk_container_add (GTK_CONTAINER (alignment20), hbox67);

  image60 = gtk_image_new_from_stock ("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image60);
  gtk_box_pack_start (GTK_BOX (hbox67), image60, FALSE, FALSE, 0);

  label107 = gtk_label_new_with_mnemonic (_("Confirm!"));
  gtk_widget_show (label107);
  gtk_box_pack_start (GTK_BOX (hbox67), label107, FALSE, FALSE, 0);

  button_stadium_cancel = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (button_stadium_cancel);
  gtk_box_pack_start (GTK_BOX (vbox39), button_stadium_cancel, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, button_stadium_cancel, _("Esc"), NULL);
  gtk_widget_add_accelerator (button_stadium_cancel, "clicked", accel_group,
                              GDK_Escape, (GdkModifierType) 0,
                              GTK_ACCEL_VISIBLE);

  g_signal_connect ((gpointer) window_stadium, "delete_event",
                    G_CALLBACK (on_window_stadium_delete_event),
                    NULL);
  g_signal_connect ((gpointer) spinbutton_capacity, "value_changed",
                    G_CALLBACK (on_spinbutton_capacity_value_changed),
                    NULL);
  g_signal_connect ((gpointer) spinbutton_capacity, "button_press_event",
                    G_CALLBACK (on_spinbutton_capacity_button_press_event),
                    NULL);
  g_signal_connect ((gpointer) spinbutton_safety, "value_changed",
                    G_CALLBACK (on_spinbutton_safety_value_changed),
                    NULL);
  g_signal_connect ((gpointer) spinbutton_safety, "button_press_event",
                    G_CALLBACK (on_spinbutton_capacity_button_press_event),
                    NULL);
  g_signal_connect ((gpointer) button_stadium_ok, "clicked",
                    G_CALLBACK (on_button_stadium_ok_clicked),
                    NULL);
  g_signal_connect ((gpointer) button_stadium_cancel, "clicked",
                    G_CALLBACK (on_button_stadium_cancel_clicked),
                    NULL);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (window_stadium, window_stadium, "window_stadium");
  GLADE_HOOKUP_OBJECT (window_stadium, vbox39, "vbox39");
  GLADE_HOOKUP_OBJECT (window_stadium, label_stadium_name, "label_stadium_name");
  GLADE_HOOKUP_OBJECT (window_stadium, table1, "table1");
  GLADE_HOOKUP_OBJECT (window_stadium, hbox69, "hbox69");
  GLADE_HOOKUP_OBJECT (window_stadium, label_capacity, "label_capacity");
  GLADE_HOOKUP_OBJECT (window_stadium, label102, "label102");
  GLADE_HOOKUP_OBJECT (window_stadium, label_average_attendance, "label_average_attendance");
  GLADE_HOOKUP_OBJECT (window_stadium, progressbar_average_attendance, "progressbar_average_attendance");
  GLADE_HOOKUP_OBJECT (window_stadium, label106, "label106");
  GLADE_HOOKUP_OBJECT (window_stadium, label_stadium_status, "label_stadium_status");
  GLADE_HOOKUP_OBJECT (window_stadium, label101, "label101");
  GLADE_HOOKUP_OBJECT (window_stadium, progressbar_safety, "progressbar_safety");
  GLADE_HOOKUP_OBJECT (window_stadium, label103, "label103");
  GLADE_HOOKUP_OBJECT (window_stadium, label109, "label109");
  GLADE_HOOKUP_OBJECT (window_stadium, label111, "label111");
  GLADE_HOOKUP_OBJECT (window_stadium, hseparator17, "hseparator17");
  GLADE_HOOKUP_OBJECT (window_stadium, table3, "table3");
  GLADE_HOOKUP_OBJECT (window_stadium, spin_ticket_price, "spin_ticket_price");
  GLADE_HOOKUP_OBJECT (window_stadium, label124, "label124");
  GLADE_HOOKUP_OBJECT (window_stadium, label123, "label123");
  GLADE_HOOKUP_OBJECT (window_stadium, hseparator14, "hseparator14");
  GLADE_HOOKUP_OBJECT (window_stadium, vbox43, "vbox43");
  GLADE_HOOKUP_OBJECT (window_stadium, label90, "label90");
  GLADE_HOOKUP_OBJECT (window_stadium, hbox61, "hbox61");
  GLADE_HOOKUP_OBJECT (window_stadium, vbox44, "vbox44");
  GLADE_HOOKUP_OBJECT (window_stadium, label91, "label91");
  GLADE_HOOKUP_OBJECT (window_stadium, spinbutton_capacity, "spinbutton_capacity");
  GLADE_HOOKUP_OBJECT (window_stadium, vbox46, "vbox46");
  GLADE_HOOKUP_OBJECT (window_stadium, label93, "label93");
  GLADE_HOOKUP_OBJECT (window_stadium, label_costs_capacity, "label_costs_capacity");
  GLADE_HOOKUP_OBJECT (window_stadium, label95, "label95");
  GLADE_HOOKUP_OBJECT (window_stadium, label_duration_capacity, "label_duration_capacity");
  GLADE_HOOKUP_OBJECT (window_stadium, vbox45, "vbox45");
  GLADE_HOOKUP_OBJECT (window_stadium, label92, "label92");
  GLADE_HOOKUP_OBJECT (window_stadium, spinbutton_safety, "spinbutton_safety");
  GLADE_HOOKUP_OBJECT (window_stadium, vbox47, "vbox47");
  GLADE_HOOKUP_OBJECT (window_stadium, label94, "label94");
  GLADE_HOOKUP_OBJECT (window_stadium, label_costs_safety, "label_costs_safety");
  GLADE_HOOKUP_OBJECT (window_stadium, label96, "label96");
  GLADE_HOOKUP_OBJECT (window_stadium, label_duration_safety, "label_duration_safety");
  GLADE_HOOKUP_OBJECT (window_stadium, hseparator15, "hseparator15");
  GLADE_HOOKUP_OBJECT (window_stadium, button_stadium_ok, "button_stadium_ok");
  GLADE_HOOKUP_OBJECT (window_stadium, alignment20, "alignment20");
  GLADE_HOOKUP_OBJECT (window_stadium, hbox67, "hbox67");
  GLADE_HOOKUP_OBJECT (window_stadium, image60, "image60");
  GLADE_HOOKUP_OBJECT (window_stadium, label107, "label107");
  GLADE_HOOKUP_OBJECT (window_stadium, button_stadium_cancel, "button_stadium_cancel");
  GLADE_HOOKUP_OBJECT_NO_REF (window_stadium, tooltips, "tooltips");

  gtk_window_add_accel_group (GTK_WINDOW (window_stadium), accel_group);

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

