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

#include "misc2_callbacks.h"
#include "misc2_interface.h"
#include "support.h"

#define GLADE_HOOKUP_OBJECT(component,widget,name) \
  g_object_set_data_full (G_OBJECT (component), name, \
    gtk_widget_ref (widget), (GDestroyNotify) gtk_widget_unref)

#define GLADE_HOOKUP_OBJECT_NO_REF(component,widget,name) \
  g_object_set_data (G_OBJECT (component), name, widget)

GtkWidget*
create_stadium_window (void)
{
  GtkWidget *stadium_window;
  GtkWidget *vbox1;
  GtkWidget *hbox1;
  GtkWidget *vbox2;
  GtkWidget *label1;
  GtkWidget *hseparator1;
  GtkWidget *radiobutton_seat0;
  GSList *radiobutton_seat0_group = NULL;
  GtkWidget *radiobutton_seat1;
  GtkWidget *radiobutton_seat2;
  GtkWidget *radiobutton_seat3;
  GtkWidget *vseparator1;
  GtkWidget *vbox3;
  GtkWidget *label2;
  GtkWidget *hseparator2;
  GtkWidget *label_cost0;
  GtkWidget *label_cost1;
  GtkWidget *label_cost2;
  GtkWidget *label_cost3;
  GtkWidget *vseparator2;
  GtkWidget *vbox4;
  GtkWidget *label3;
  GtkWidget *hseparator3;
  GtkWidget *label8;
  GtkWidget *label_finished0;
  GtkWidget *label_finished1;
  GtkWidget *label_finished2;
  GtkWidget *hseparator4;
  GtkWidget *hbox2;
  GtkWidget *button_ok;
  GtkWidget *alignment1;
  GtkWidget *hbox3;
  GtkWidget *image1;
  GtkWidget *label12;
  GtkWidget *button_cancel;
  GtkWidget *alignment2;
  GtkWidget *hbox4;
  GtkWidget *image2;
  GtkWidget *label13;
  GtkAccelGroup *accel_group;
  GtkTooltips *tooltips;

  tooltips = gtk_tooltips_new ();

  accel_group = gtk_accel_group_new ();

  stadium_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_container_set_border_width (GTK_CONTAINER (stadium_window), 5);
  gtk_window_set_title (GTK_WINDOW (stadium_window), _("Bygfoot - Stadium improvement"));
  gtk_window_set_position (GTK_WINDOW (stadium_window), GTK_WIN_POS_CENTER);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (stadium_window), vbox1);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox1, FALSE, TRUE, 0);

  vbox2 = gtk_vbox_new (FALSE, 3);
  gtk_widget_show (vbox2);
  gtk_box_pack_start (GTK_BOX (hbox1), vbox2, TRUE, TRUE, 0);

  label1 = gtk_label_new (_("Increase (seats)"));
  gtk_widget_show (label1);
  gtk_box_pack_start (GTK_BOX (vbox2), label1, FALSE, FALSE, 0);

  hseparator1 = gtk_hseparator_new ();
  gtk_widget_show (hseparator1);
  gtk_box_pack_start (GTK_BOX (vbox2), hseparator1, FALSE, TRUE, 0);
  gtk_widget_set_size_request (hseparator1, 1, 10);

  radiobutton_seat0 = gtk_radio_button_new_with_mnemonic (NULL, _("500"));
  gtk_widget_show (radiobutton_seat0);
  gtk_box_pack_start (GTK_BOX (vbox2), radiobutton_seat0, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_seat0), radiobutton_seat0_group);
  radiobutton_seat0_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_seat0));
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radiobutton_seat0), TRUE);

  radiobutton_seat1 = gtk_radio_button_new_with_mnemonic (NULL, _("1000"));
  gtk_widget_show (radiobutton_seat1);
  gtk_box_pack_start (GTK_BOX (vbox2), radiobutton_seat1, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_seat1), radiobutton_seat0_group);
  radiobutton_seat0_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_seat1));

  radiobutton_seat2 = gtk_radio_button_new_with_mnemonic (NULL, _("1500"));
  gtk_widget_show (radiobutton_seat2);
  gtk_box_pack_start (GTK_BOX (vbox2), radiobutton_seat2, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_seat2), radiobutton_seat0_group);
  radiobutton_seat0_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_seat2));

  radiobutton_seat3 = gtk_radio_button_new_with_mnemonic (NULL, _("2000"));
  gtk_widget_show (radiobutton_seat3);
  gtk_box_pack_start (GTK_BOX (vbox2), radiobutton_seat3, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_seat3), radiobutton_seat0_group);
  radiobutton_seat0_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_seat3));

  vseparator1 = gtk_vseparator_new ();
  gtk_widget_show (vseparator1);
  gtk_box_pack_start (GTK_BOX (hbox1), vseparator1, FALSE, TRUE, 0);
  gtk_widget_set_size_request (vseparator1, 10, 1);

  vbox3 = gtk_vbox_new (FALSE, 3);
  gtk_widget_show (vbox3);
  gtk_box_pack_start (GTK_BOX (hbox1), vbox3, TRUE, TRUE, 0);

  label2 = gtk_label_new (_("Costs"));
  gtk_widget_show (label2);
  gtk_box_pack_start (GTK_BOX (vbox3), label2, FALSE, FALSE, 0);

  hseparator2 = gtk_hseparator_new ();
  gtk_widget_show (hseparator2);
  gtk_box_pack_start (GTK_BOX (vbox3), hseparator2, FALSE, TRUE, 0);
  gtk_widget_set_size_request (hseparator2, 1, 10);

  label_cost0 = gtk_label_new (_("label4"));
  gtk_widget_show (label_cost0);
  gtk_box_pack_start (GTK_BOX (vbox3), label_cost0, FALSE, FALSE, 0);
  gtk_misc_set_padding (GTK_MISC (label_cost0), 0, 4);

  label_cost1 = gtk_label_new (_("label5"));
  gtk_widget_show (label_cost1);
  gtk_box_pack_start (GTK_BOX (vbox3), label_cost1, FALSE, FALSE, 0);
  gtk_misc_set_padding (GTK_MISC (label_cost1), 0, 4);

  label_cost2 = gtk_label_new (_("label6"));
  gtk_widget_show (label_cost2);
  gtk_box_pack_start (GTK_BOX (vbox3), label_cost2, FALSE, FALSE, 0);
  gtk_misc_set_padding (GTK_MISC (label_cost2), 0, 4);

  label_cost3 = gtk_label_new (_("label7"));
  gtk_widget_show (label_cost3);
  gtk_box_pack_start (GTK_BOX (vbox3), label_cost3, FALSE, FALSE, 0);
  gtk_misc_set_padding (GTK_MISC (label_cost3), 0, 4);

  vseparator2 = gtk_vseparator_new ();
  gtk_widget_show (vseparator2);
  gtk_box_pack_start (GTK_BOX (hbox1), vseparator2, FALSE, TRUE, 0);
  gtk_widget_set_size_request (vseparator2, 10, 1);

  vbox4 = gtk_vbox_new (FALSE, 3);
  gtk_widget_show (vbox4);
  gtk_box_pack_start (GTK_BOX (hbox1), vbox4, TRUE, TRUE, 0);

  label3 = gtk_label_new (_("Finished (weeks)"));
  gtk_widget_show (label3);
  gtk_box_pack_start (GTK_BOX (vbox4), label3, FALSE, FALSE, 0);

  hseparator3 = gtk_hseparator_new ();
  gtk_widget_show (hseparator3);
  gtk_box_pack_start (GTK_BOX (vbox4), hseparator3, FALSE, TRUE, 0);
  gtk_widget_set_size_request (hseparator3, 1, 10);

  label8 = gtk_label_new (_("Before next match"));
  gtk_widget_show (label8);
  gtk_box_pack_start (GTK_BOX (vbox4), label8, FALSE, FALSE, 0);
  gtk_misc_set_padding (GTK_MISC (label8), 0, 3);

  label_finished0 = gtk_label_new (_("1-2"));
  gtk_widget_show (label_finished0);
  gtk_box_pack_start (GTK_BOX (vbox4), label_finished0, FALSE, FALSE, 0);
  gtk_misc_set_padding (GTK_MISC (label_finished0), 0, 3);

  label_finished1 = gtk_label_new (_("3-4"));
  gtk_widget_show (label_finished1);
  gtk_box_pack_start (GTK_BOX (vbox4), label_finished1, FALSE, FALSE, 0);
  gtk_misc_set_padding (GTK_MISC (label_finished1), 0, 3);

  label_finished2 = gtk_label_new (_("5-6"));
  gtk_widget_show (label_finished2);
  gtk_box_pack_start (GTK_BOX (vbox4), label_finished2, FALSE, FALSE, 0);
  gtk_misc_set_padding (GTK_MISC (label_finished2), 0, 3);

  hseparator4 = gtk_hseparator_new ();
  gtk_widget_show (hseparator4);
  gtk_box_pack_start (GTK_BOX (vbox1), hseparator4, FALSE, TRUE, 0);
  gtk_widget_set_size_request (hseparator4, 1, 10);

  hbox2 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox2);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox2, FALSE, TRUE, 0);

  button_ok = gtk_button_new ();
  gtk_widget_show (button_ok);
  gtk_box_pack_start (GTK_BOX (hbox2), button_ok, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (button_ok), 2);
  gtk_tooltips_set_tip (tooltips, button_ok, _("Return"), NULL);
  gtk_widget_add_accelerator (button_ok, "clicked", accel_group,
                              GDK_Return, 0,
                              GTK_ACCEL_VISIBLE);

  alignment1 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment1);
  gtk_container_add (GTK_CONTAINER (button_ok), alignment1);

  hbox3 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox3);
  gtk_container_add (GTK_CONTAINER (alignment1), hbox3);

  image1 = gtk_image_new_from_stock ("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image1);
  gtk_box_pack_start (GTK_BOX (hbox3), image1, FALSE, FALSE, 0);

  label12 = gtk_label_new_with_mnemonic (_("OK"));
  gtk_widget_show (label12);
  gtk_box_pack_start (GTK_BOX (hbox3), label12, FALSE, FALSE, 0);

  button_cancel = gtk_button_new ();
  gtk_widget_show (button_cancel);
  gtk_box_pack_start (GTK_BOX (hbox2), button_cancel, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (button_cancel), 2);
  gtk_tooltips_set_tip (tooltips, button_cancel, _("Esc"), NULL);
  gtk_widget_add_accelerator (button_cancel, "clicked", accel_group,
                              GDK_Escape, 0,
                              GTK_ACCEL_VISIBLE);

  alignment2 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment2);
  gtk_container_add (GTK_CONTAINER (button_cancel), alignment2);

  hbox4 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox4);
  gtk_container_add (GTK_CONTAINER (alignment2), hbox4);

  image2 = gtk_image_new_from_stock ("gtk-cancel", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image2);
  gtk_box_pack_start (GTK_BOX (hbox4), image2, FALSE, FALSE, 0);

  label13 = gtk_label_new_with_mnemonic (_("Cancel"));
  gtk_widget_show (label13);
  gtk_box_pack_start (GTK_BOX (hbox4), label13, FALSE, FALSE, 0);

  g_signal_connect ((gpointer) stadium_window, "delete_event",
                    G_CALLBACK (on_button_cancel_clicked),
                    NULL);
  g_signal_connect ((gpointer) button_ok, "clicked",
                    G_CALLBACK (on_button_ok_clicked),
                    NULL);
  g_signal_connect ((gpointer) button_cancel, "clicked",
                    G_CALLBACK (on_button_cancel_clicked),
                    NULL);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (stadium_window, stadium_window, "stadium_window");
  GLADE_HOOKUP_OBJECT (stadium_window, vbox1, "vbox1");
  GLADE_HOOKUP_OBJECT (stadium_window, hbox1, "hbox1");
  GLADE_HOOKUP_OBJECT (stadium_window, vbox2, "vbox2");
  GLADE_HOOKUP_OBJECT (stadium_window, label1, "label1");
  GLADE_HOOKUP_OBJECT (stadium_window, hseparator1, "hseparator1");
  GLADE_HOOKUP_OBJECT (stadium_window, radiobutton_seat0, "radiobutton_seat0");
  GLADE_HOOKUP_OBJECT (stadium_window, radiobutton_seat1, "radiobutton_seat1");
  GLADE_HOOKUP_OBJECT (stadium_window, radiobutton_seat2, "radiobutton_seat2");
  GLADE_HOOKUP_OBJECT (stadium_window, radiobutton_seat3, "radiobutton_seat3");
  GLADE_HOOKUP_OBJECT (stadium_window, vseparator1, "vseparator1");
  GLADE_HOOKUP_OBJECT (stadium_window, vbox3, "vbox3");
  GLADE_HOOKUP_OBJECT (stadium_window, label2, "label2");
  GLADE_HOOKUP_OBJECT (stadium_window, hseparator2, "hseparator2");
  GLADE_HOOKUP_OBJECT (stadium_window, label_cost0, "label_cost0");
  GLADE_HOOKUP_OBJECT (stadium_window, label_cost1, "label_cost1");
  GLADE_HOOKUP_OBJECT (stadium_window, label_cost2, "label_cost2");
  GLADE_HOOKUP_OBJECT (stadium_window, label_cost3, "label_cost3");
  GLADE_HOOKUP_OBJECT (stadium_window, vseparator2, "vseparator2");
  GLADE_HOOKUP_OBJECT (stadium_window, vbox4, "vbox4");
  GLADE_HOOKUP_OBJECT (stadium_window, label3, "label3");
  GLADE_HOOKUP_OBJECT (stadium_window, hseparator3, "hseparator3");
  GLADE_HOOKUP_OBJECT (stadium_window, label8, "label8");
  GLADE_HOOKUP_OBJECT (stadium_window, label_finished0, "label_finished0");
  GLADE_HOOKUP_OBJECT (stadium_window, label_finished1, "label_finished1");
  GLADE_HOOKUP_OBJECT (stadium_window, label_finished2, "label_finished2");
  GLADE_HOOKUP_OBJECT (stadium_window, hseparator4, "hseparator4");
  GLADE_HOOKUP_OBJECT (stadium_window, hbox2, "hbox2");
  GLADE_HOOKUP_OBJECT (stadium_window, button_ok, "button_ok");
  GLADE_HOOKUP_OBJECT (stadium_window, alignment1, "alignment1");
  GLADE_HOOKUP_OBJECT (stadium_window, hbox3, "hbox3");
  GLADE_HOOKUP_OBJECT (stadium_window, image1, "image1");
  GLADE_HOOKUP_OBJECT (stadium_window, label12, "label12");
  GLADE_HOOKUP_OBJECT (stadium_window, button_cancel, "button_cancel");
  GLADE_HOOKUP_OBJECT (stadium_window, alignment2, "alignment2");
  GLADE_HOOKUP_OBJECT (stadium_window, hbox4, "hbox4");
  GLADE_HOOKUP_OBJECT (stadium_window, image2, "image2");
  GLADE_HOOKUP_OBJECT (stadium_window, label13, "label13");
  GLADE_HOOKUP_OBJECT_NO_REF (stadium_window, tooltips, "tooltips");

  gtk_window_add_accel_group (GTK_WINDOW (stadium_window), accel_group);

  return stadium_window;
}

GtkWidget*
create_job_offer_window (void)
{
  GtkWidget *job_offer_window;
  GtkWidget *vbox5;
  GtkWidget *label20;
  GtkWidget *hseparator5;
  GtkWidget *hbox5;
  GtkWidget *vbox6;
  GtkWidget *label15;
  GtkWidget *label16;
  GtkWidget *label17;
  GtkWidget *label18;
  GtkWidget *label19;
  GtkWidget *label14;
  GtkWidget *vseparator3;
  GtkWidget *vbox7;
  GtkWidget *label21;
  GtkWidget *label22;
  GtkWidget *label23;
  GtkWidget *label24;
  GtkWidget *label25;
  GtkWidget *label26;
  GtkWidget *hseparator6;
  GtkWidget *label28;
  GtkWidget *scrolledwindow1;
  GtkWidget *treeview_player_list;
  GtkWidget *hseparator7;
  GtkWidget *label27;
  GtkWidget *hbox6;
  GtkWidget *button_ok;
  GtkWidget *alignment5;
  GtkWidget *hbox9;
  GtkWidget *image5;
  GtkWidget *label31;
  GtkWidget *button_cancel;
  GtkWidget *alignment4;
  GtkWidget *hbox8;
  GtkWidget *image4;
  GtkWidget *label30;

  job_offer_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_container_set_border_width (GTK_CONTAINER (job_offer_window), 6);
  gtk_window_set_title (GTK_WINDOW (job_offer_window), _("Job offer"));
  gtk_window_set_position (GTK_WINDOW (job_offer_window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size (GTK_WINDOW (job_offer_window), -1, 600);

  vbox5 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox5);
  gtk_container_add (GTK_CONTAINER (job_offer_window), vbox5);

  label20 = gtk_label_new (_("label20"));
  gtk_widget_show (label20);
  gtk_box_pack_start (GTK_BOX (vbox5), label20, FALSE, FALSE, 0);

  hseparator5 = gtk_hseparator_new ();
  gtk_widget_show (hseparator5);
  gtk_box_pack_start (GTK_BOX (vbox5), hseparator5, FALSE, TRUE, 0);
  gtk_widget_set_size_request (hseparator5, 1, 10);

  hbox5 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox5);
  gtk_box_pack_start (GTK_BOX (vbox5), hbox5, FALSE, TRUE, 0);

  vbox6 = gtk_vbox_new (FALSE, 3);
  gtk_widget_show (vbox6);
  gtk_box_pack_start (GTK_BOX (hbox5), vbox6, FALSE, TRUE, 0);

  label15 = gtk_label_new (_("Name"));
  gtk_widget_show (label15);
  gtk_box_pack_start (GTK_BOX (vbox6), label15, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC (label15), 2.23517e-10, 0.5);

  label16 = gtk_label_new (_("League"));
  gtk_widget_show (label16);
  gtk_box_pack_start (GTK_BOX (vbox6), label16, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC (label16), 2.23517e-10, 0.5);

  label17 = gtk_label_new (_("Rank"));
  gtk_widget_show (label17);
  gtk_box_pack_start (GTK_BOX (vbox6), label17, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC (label17), 2.23517e-10, 0.5);

  label18 = gtk_label_new (_("Money (approx.)"));
  gtk_widget_show (label18);
  gtk_box_pack_start (GTK_BOX (vbox6), label18, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC (label18), 2.23517e-10, 0.5);

  label19 = gtk_label_new (_("Stadium capacity"));
  gtk_widget_show (label19);
  gtk_box_pack_start (GTK_BOX (vbox6), label19, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC (label19), 2.23517e-10, 0.5);

  label14 = gtk_label_new (_("Stadium safety"));
  gtk_widget_show (label14);
  gtk_box_pack_start (GTK_BOX (vbox6), label14, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC (label14), 0, 0.5);

  vseparator3 = gtk_vseparator_new ();
  gtk_widget_show (vseparator3);
  gtk_box_pack_start (GTK_BOX (hbox5), vseparator3, FALSE, TRUE, 0);
  gtk_widget_set_size_request (vseparator3, 10, 1);

  vbox7 = gtk_vbox_new (FALSE, 3);
  gtk_widget_show (vbox7);
  gtk_box_pack_start (GTK_BOX (hbox5), vbox7, TRUE, TRUE, 0);

  label21 = gtk_label_new (_("label21"));
  gtk_widget_show (label21);
  gtk_box_pack_start (GTK_BOX (vbox7), label21, FALSE, FALSE, 0);

  label22 = gtk_label_new (_("label22"));
  gtk_widget_show (label22);
  gtk_box_pack_start (GTK_BOX (vbox7), label22, FALSE, FALSE, 0);

  label23 = gtk_label_new (_("label23"));
  gtk_widget_show (label23);
  gtk_box_pack_start (GTK_BOX (vbox7), label23, FALSE, FALSE, 0);

  label24 = gtk_label_new (_("label24"));
  gtk_widget_show (label24);
  gtk_box_pack_start (GTK_BOX (vbox7), label24, FALSE, FALSE, 0);

  label25 = gtk_label_new (_("label25"));
  gtk_widget_show (label25);
  gtk_box_pack_start (GTK_BOX (vbox7), label25, FALSE, FALSE, 0);

  label26 = gtk_label_new (_("label26"));
  gtk_widget_show (label26);
  gtk_box_pack_start (GTK_BOX (vbox7), label26, FALSE, FALSE, 0);

  hseparator6 = gtk_hseparator_new ();
  gtk_widget_show (hseparator6);
  gtk_box_pack_start (GTK_BOX (vbox5), hseparator6, FALSE, TRUE, 0);
  gtk_widget_set_size_request (hseparator6, 1, 10);

  label28 = gtk_label_new (_("Player list"));
  gtk_widget_show (label28);
  gtk_box_pack_start (GTK_BOX (vbox5), label28, FALSE, FALSE, 0);

  scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow1);
  gtk_box_pack_start (GTK_BOX (vbox5), scrolledwindow1, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow1), 4);

  treeview_player_list = gtk_tree_view_new ();
  gtk_widget_show (treeview_player_list);
  gtk_container_add (GTK_CONTAINER (scrolledwindow1), treeview_player_list);

  hseparator7 = gtk_hseparator_new ();
  gtk_widget_show (hseparator7);
  gtk_box_pack_start (GTK_BOX (vbox5), hseparator7, FALSE, TRUE, 0);
  gtk_widget_set_size_request (hseparator7, 1, 10);

  label27 = gtk_label_new (_("label27"));
  gtk_widget_show (label27);
  gtk_box_pack_start (GTK_BOX (vbox5), label27, FALSE, FALSE, 0);
  gtk_misc_set_padding (GTK_MISC (label27), 0, 5);

  hbox6 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox6);
  gtk_box_pack_start (GTK_BOX (vbox5), hbox6, FALSE, TRUE, 0);

  button_ok = gtk_button_new ();
  gtk_widget_show (button_ok);
  gtk_box_pack_start (GTK_BOX (hbox6), button_ok, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (button_ok), 2);

  alignment5 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment5);
  gtk_container_add (GTK_CONTAINER (button_ok), alignment5);

  hbox9 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox9);
  gtk_container_add (GTK_CONTAINER (alignment5), hbox9);

  image5 = gtk_image_new_from_stock ("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image5);
  gtk_box_pack_start (GTK_BOX (hbox9), image5, FALSE, FALSE, 0);

  label31 = gtk_label_new_with_mnemonic (_("OK"));
  gtk_widget_show (label31);
  gtk_box_pack_start (GTK_BOX (hbox9), label31, FALSE, FALSE, 0);

  button_cancel = gtk_button_new ();
  gtk_widget_show (button_cancel);
  gtk_box_pack_start (GTK_BOX (hbox6), button_cancel, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (button_cancel), 2);

  alignment4 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment4);
  gtk_container_add (GTK_CONTAINER (button_cancel), alignment4);

  hbox8 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox8);
  gtk_container_add (GTK_CONTAINER (alignment4), hbox8);

  image4 = gtk_image_new_from_stock ("gtk-cancel", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image4);
  gtk_box_pack_start (GTK_BOX (hbox8), image4, FALSE, FALSE, 0);

  label30 = gtk_label_new_with_mnemonic (_("Cancel"));
  gtk_widget_show (label30);
  gtk_box_pack_start (GTK_BOX (hbox8), label30, FALSE, FALSE, 0);

  g_signal_connect ((gpointer) button_ok, "clicked",
                    G_CALLBACK (on_button_offer_ok_clicked),
                    NULL);
  g_signal_connect ((gpointer) button_cancel, "clicked",
                    G_CALLBACK (on_button_offer_cancel_clicked),
                    NULL);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (job_offer_window, job_offer_window, "job_offer_window");
  GLADE_HOOKUP_OBJECT (job_offer_window, vbox5, "vbox5");
  GLADE_HOOKUP_OBJECT (job_offer_window, label20, "label20");
  GLADE_HOOKUP_OBJECT (job_offer_window, hseparator5, "hseparator5");
  GLADE_HOOKUP_OBJECT (job_offer_window, hbox5, "hbox5");
  GLADE_HOOKUP_OBJECT (job_offer_window, vbox6, "vbox6");
  GLADE_HOOKUP_OBJECT (job_offer_window, label15, "label15");
  GLADE_HOOKUP_OBJECT (job_offer_window, label16, "label16");
  GLADE_HOOKUP_OBJECT (job_offer_window, label17, "label17");
  GLADE_HOOKUP_OBJECT (job_offer_window, label18, "label18");
  GLADE_HOOKUP_OBJECT (job_offer_window, label19, "label19");
  GLADE_HOOKUP_OBJECT (job_offer_window, label14, "label14");
  GLADE_HOOKUP_OBJECT (job_offer_window, vseparator3, "vseparator3");
  GLADE_HOOKUP_OBJECT (job_offer_window, vbox7, "vbox7");
  GLADE_HOOKUP_OBJECT (job_offer_window, label21, "label21");
  GLADE_HOOKUP_OBJECT (job_offer_window, label22, "label22");
  GLADE_HOOKUP_OBJECT (job_offer_window, label23, "label23");
  GLADE_HOOKUP_OBJECT (job_offer_window, label24, "label24");
  GLADE_HOOKUP_OBJECT (job_offer_window, label25, "label25");
  GLADE_HOOKUP_OBJECT (job_offer_window, label26, "label26");
  GLADE_HOOKUP_OBJECT (job_offer_window, hseparator6, "hseparator6");
  GLADE_HOOKUP_OBJECT (job_offer_window, label28, "label28");
  GLADE_HOOKUP_OBJECT (job_offer_window, scrolledwindow1, "scrolledwindow1");
  GLADE_HOOKUP_OBJECT (job_offer_window, treeview_player_list, "treeview_player_list");
  GLADE_HOOKUP_OBJECT (job_offer_window, hseparator7, "hseparator7");
  GLADE_HOOKUP_OBJECT (job_offer_window, label27, "label27");
  GLADE_HOOKUP_OBJECT (job_offer_window, hbox6, "hbox6");
  GLADE_HOOKUP_OBJECT (job_offer_window, button_ok, "button_ok");
  GLADE_HOOKUP_OBJECT (job_offer_window, alignment5, "alignment5");
  GLADE_HOOKUP_OBJECT (job_offer_window, hbox9, "hbox9");
  GLADE_HOOKUP_OBJECT (job_offer_window, image5, "image5");
  GLADE_HOOKUP_OBJECT (job_offer_window, label31, "label31");
  GLADE_HOOKUP_OBJECT (job_offer_window, button_cancel, "button_cancel");
  GLADE_HOOKUP_OBJECT (job_offer_window, alignment4, "alignment4");
  GLADE_HOOKUP_OBJECT (job_offer_window, hbox8, "hbox8");
  GLADE_HOOKUP_OBJECT (job_offer_window, image4, "image4");
  GLADE_HOOKUP_OBJECT (job_offer_window, label30, "label30");

  return job_offer_window;
}

GtkWidget*
create_graph_window (void)
{
  GtkWidget *graph_window;
  GtkWidget *vbox8;
  GtkWidget *label_title;
  GtkWidget *vbox9;
  GtkWidget *hbox10;
  GtkWidget *vbox12;
  GtkWidget *vbox10;
  GtkWidget *label36;
  GtkWidget *label37;
  GtkWidget *label38;
  GtkWidget *label39;
  GtkWidget *label40;
  GtkWidget *label41;
  GtkWidget *vbox11;
  GtkWidget *curve_graph;
  GtkWidget *hruler_week;
  GtkWidget *hruler_season;
  GtkWidget *label33;
  GtkWidget *hseparator8;
  GtkWidget *hbox11;
  GtkWidget *optionmenu_player;
  GtkWidget *menu1;
  GtkWidget *player1;
  GtkWidget *skill_development1;
  GtkWidget *goal_development1;
  GtkWidget *wage_development1;
  GtkWidget *value_development1;
  GtkWidget *optionmenu_team;
  GtkWidget *menu2;
  GtkWidget *finances1;
  GtkWidget *show_money_development1;
  GtkWidget *points1;
  GtkWidget *goal_difference1;
  GtkWidget *goals_for1;
  GtkWidget *goals_against1;
  GtkWidget *money1;
  GtkWidget *average_attendance1;
  GtkWidget *button_close;
  GtkWidget *alignment6;
  GtkWidget *hbox14;
  GtkWidget *image6;
  GtkWidget *label35;
  GtkAccelGroup *accel_group;
  GtkTooltips *tooltips;

  tooltips = gtk_tooltips_new ();

  accel_group = gtk_accel_group_new ();

  graph_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_container_set_border_width (GTK_CONTAINER (graph_window), 6);
  gtk_window_set_title (GTK_WINDOW (graph_window), _("window1"));
  gtk_window_set_position (GTK_WINDOW (graph_window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size (GTK_WINDOW (graph_window), 400, 400);

  vbox8 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox8);
  gtk_container_add (GTK_CONTAINER (graph_window), vbox8);

  label_title = gtk_label_new (_("Statistics"));
  gtk_widget_show (label_title);
  gtk_box_pack_start (GTK_BOX (vbox8), label_title, FALSE, FALSE, 0);

  vbox9 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox9);
  gtk_box_pack_start (GTK_BOX (vbox8), vbox9, TRUE, TRUE, 0);

  hbox10 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox10);
  gtk_box_pack_start (GTK_BOX (vbox9), hbox10, TRUE, TRUE, 0);

  vbox12 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox12);
  gtk_box_pack_start (GTK_BOX (hbox10), vbox12, FALSE, TRUE, 0);

  vbox10 = gtk_vbox_new (TRUE, 50);
  gtk_widget_show (vbox10);
  gtk_box_pack_start (GTK_BOX (vbox12), vbox10, TRUE, TRUE, 0);

  label36 = gtk_label_new (_("label36"));
  gtk_widget_show (label36);
  gtk_box_pack_start (GTK_BOX (vbox10), label36, FALSE, FALSE, 0);

  label37 = gtk_label_new (_("label37"));
  gtk_widget_show (label37);
  gtk_box_pack_start (GTK_BOX (vbox10), label37, FALSE, FALSE, 0);

  label38 = gtk_label_new (_("label38"));
  gtk_widget_show (label38);
  gtk_box_pack_start (GTK_BOX (vbox10), label38, FALSE, FALSE, 0);

  label39 = gtk_label_new (_("label39"));
  gtk_widget_show (label39);
  gtk_box_pack_start (GTK_BOX (vbox10), label39, FALSE, FALSE, 0);

  label40 = gtk_label_new (_("label40"));
  gtk_widget_show (label40);
  gtk_box_pack_start (GTK_BOX (vbox10), label40, FALSE, FALSE, 0);

  label41 = gtk_label_new ("");
  gtk_widget_show (label41);
  gtk_box_pack_start (GTK_BOX (vbox12), label41, FALSE, FALSE, 0);

  vbox11 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox11);
  gtk_box_pack_start (GTK_BOX (hbox10), vbox11, TRUE, TRUE, 0);

  curve_graph = gtk_curve_new ();
  gtk_widget_show (curve_graph);
  gtk_box_pack_start (GTK_BOX (vbox11), curve_graph, TRUE, TRUE, 0);
  gtk_curve_set_range (GTK_CURVE (curve_graph), 0, 1, 0, 1);

  hruler_week = gtk_hruler_new ();
  gtk_widget_show (hruler_week);
  gtk_box_pack_start (GTK_BOX (vbox11), hruler_week, FALSE, FALSE, 0);
  gtk_ruler_set_range (GTK_RULER (hruler_week), 0, 10, 4.75504, 10);

  hruler_season = gtk_hruler_new ();
  gtk_widget_show (hruler_season);
  gtk_box_pack_start (GTK_BOX (vbox11), hruler_season, FALSE, TRUE, 0);
  gtk_ruler_set_range (GTK_RULER (hruler_season), 0, 10, 3.83285, 10);

  label33 = gtk_label_new (_("Weeks / Seasons"));
  gtk_widget_show (label33);
  gtk_box_pack_start (GTK_BOX (vbox9), label33, FALSE, FALSE, 0);

  hseparator8 = gtk_hseparator_new ();
  gtk_widget_show (hseparator8);
  gtk_box_pack_start (GTK_BOX (vbox8), hseparator8, FALSE, TRUE, 0);
  gtk_widget_set_size_request (hseparator8, 1, 10);

  hbox11 = gtk_hbox_new (FALSE, 8);
  gtk_widget_show (hbox11);
  gtk_box_pack_start (GTK_BOX (vbox8), hbox11, FALSE, TRUE, 0);

  optionmenu_player = gtk_option_menu_new ();
  gtk_widget_show (optionmenu_player);
  gtk_box_pack_start (GTK_BOX (hbox11), optionmenu_player, FALSE, FALSE, 0);

  menu1 = gtk_menu_new ();

  player1 = gtk_menu_item_new_with_mnemonic (_("Player"));
  gtk_widget_show (player1);
  gtk_container_add (GTK_CONTAINER (menu1), player1);

  skill_development1 = gtk_menu_item_new_with_mnemonic (_("Skill"));
  gtk_widget_show (skill_development1);
  gtk_container_add (GTK_CONTAINER (menu1), skill_development1);

  goal_development1 = gtk_menu_item_new_with_mnemonic (_("Goals"));
  gtk_widget_show (goal_development1);
  gtk_container_add (GTK_CONTAINER (menu1), goal_development1);

  wage_development1 = gtk_menu_item_new_with_mnemonic (_("Wage"));
  gtk_widget_show (wage_development1);
  gtk_container_add (GTK_CONTAINER (menu1), wage_development1);

  value_development1 = gtk_menu_item_new_with_mnemonic (_("Value"));
  gtk_widget_show (value_development1);
  gtk_container_add (GTK_CONTAINER (menu1), value_development1);

  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenu_player), menu1);

  optionmenu_team = gtk_option_menu_new ();
  gtk_widget_show (optionmenu_team);
  gtk_box_pack_start (GTK_BOX (hbox11), optionmenu_team, FALSE, FALSE, 0);

  menu2 = gtk_menu_new ();

  finances1 = gtk_menu_item_new_with_mnemonic (_("Team"));
  gtk_widget_show (finances1);
  gtk_container_add (GTK_CONTAINER (menu2), finances1);

  show_money_development1 = gtk_menu_item_new_with_mnemonic (_("Rank"));
  gtk_widget_show (show_money_development1);
  gtk_container_add (GTK_CONTAINER (menu2), show_money_development1);

  points1 = gtk_menu_item_new_with_mnemonic (_("Points"));
  gtk_widget_show (points1);
  gtk_container_add (GTK_CONTAINER (menu2), points1);

  goal_difference1 = gtk_menu_item_new_with_mnemonic (_("Goal difference"));
  gtk_widget_show (goal_difference1);
  gtk_container_add (GTK_CONTAINER (menu2), goal_difference1);

  goals_for1 = gtk_menu_item_new_with_mnemonic (_("Goals for"));
  gtk_widget_show (goals_for1);
  gtk_container_add (GTK_CONTAINER (menu2), goals_for1);

  goals_against1 = gtk_menu_item_new_with_mnemonic (_("Goals against"));
  gtk_widget_show (goals_against1);
  gtk_container_add (GTK_CONTAINER (menu2), goals_against1);

  money1 = gtk_menu_item_new_with_mnemonic (_("Money"));
  gtk_widget_show (money1);
  gtk_container_add (GTK_CONTAINER (menu2), money1);

  average_attendance1 = gtk_menu_item_new_with_mnemonic (_("Average attendance"));
  gtk_widget_show (average_attendance1);
  gtk_container_add (GTK_CONTAINER (menu2), average_attendance1);

  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenu_team), menu2);

  button_close = gtk_button_new ();
  gtk_widget_show (button_close);
  gtk_box_pack_start (GTK_BOX (vbox8), button_close, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (button_close), 4);
  gtk_tooltips_set_tip (tooltips, button_close, _("Space / Esc"), NULL);
  gtk_widget_add_accelerator (button_close, "clicked", accel_group,
                              GDK_Escape, 0,
                              GTK_ACCEL_VISIBLE);
  gtk_widget_add_accelerator (button_close, "clicked", accel_group,
                              GDK_space, 0,
                              GTK_ACCEL_VISIBLE);

  alignment6 = gtk_alignment_new (0.5, 0.51, 0, 0);
  gtk_widget_show (alignment6);
  gtk_container_add (GTK_CONTAINER (button_close), alignment6);

  hbox14 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox14);
  gtk_container_add (GTK_CONTAINER (alignment6), hbox14);

  image6 = gtk_image_new_from_stock ("gtk-close", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image6);
  gtk_box_pack_start (GTK_BOX (hbox14), image6, FALSE, FALSE, 0);

  label35 = gtk_label_new_with_mnemonic (_("Close"));
  gtk_widget_show (label35);
  gtk_box_pack_start (GTK_BOX (hbox14), label35, FALSE, FALSE, 0);

  g_signal_connect ((gpointer) graph_window, "delete_event",
                    G_CALLBACK (on_graph_window_delete_event),
                    NULL);
  g_signal_connect ((gpointer) optionmenu_player, "changed",
                    G_CALLBACK (on_optionmenu_player_changed),
                    NULL);
  g_signal_connect ((gpointer) optionmenu_team, "changed",
                    G_CALLBACK (on_optionmenu_team_changed),
                    NULL);
  g_signal_connect ((gpointer) button_close, "clicked",
                    G_CALLBACK (on_button_close_clicked),
                    NULL);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (graph_window, graph_window, "graph_window");
  GLADE_HOOKUP_OBJECT (graph_window, vbox8, "vbox8");
  GLADE_HOOKUP_OBJECT (graph_window, label_title, "label_title");
  GLADE_HOOKUP_OBJECT (graph_window, vbox9, "vbox9");
  GLADE_HOOKUP_OBJECT (graph_window, hbox10, "hbox10");
  GLADE_HOOKUP_OBJECT (graph_window, vbox12, "vbox12");
  GLADE_HOOKUP_OBJECT (graph_window, vbox10, "vbox10");
  GLADE_HOOKUP_OBJECT (graph_window, label36, "label36");
  GLADE_HOOKUP_OBJECT (graph_window, label37, "label37");
  GLADE_HOOKUP_OBJECT (graph_window, label38, "label38");
  GLADE_HOOKUP_OBJECT (graph_window, label39, "label39");
  GLADE_HOOKUP_OBJECT (graph_window, label40, "label40");
  GLADE_HOOKUP_OBJECT (graph_window, label41, "label41");
  GLADE_HOOKUP_OBJECT (graph_window, vbox11, "vbox11");
  GLADE_HOOKUP_OBJECT (graph_window, curve_graph, "curve_graph");
  GLADE_HOOKUP_OBJECT (graph_window, hruler_week, "hruler_week");
  GLADE_HOOKUP_OBJECT (graph_window, hruler_season, "hruler_season");
  GLADE_HOOKUP_OBJECT (graph_window, label33, "label33");
  GLADE_HOOKUP_OBJECT (graph_window, hseparator8, "hseparator8");
  GLADE_HOOKUP_OBJECT (graph_window, hbox11, "hbox11");
  GLADE_HOOKUP_OBJECT (graph_window, optionmenu_player, "optionmenu_player");
  GLADE_HOOKUP_OBJECT (graph_window, menu1, "menu1");
  GLADE_HOOKUP_OBJECT (graph_window, player1, "player1");
  GLADE_HOOKUP_OBJECT (graph_window, skill_development1, "skill_development1");
  GLADE_HOOKUP_OBJECT (graph_window, goal_development1, "goal_development1");
  GLADE_HOOKUP_OBJECT (graph_window, wage_development1, "wage_development1");
  GLADE_HOOKUP_OBJECT (graph_window, value_development1, "value_development1");
  GLADE_HOOKUP_OBJECT (graph_window, optionmenu_team, "optionmenu_team");
  GLADE_HOOKUP_OBJECT (graph_window, menu2, "menu2");
  GLADE_HOOKUP_OBJECT (graph_window, finances1, "finances1");
  GLADE_HOOKUP_OBJECT (graph_window, show_money_development1, "show_money_development1");
  GLADE_HOOKUP_OBJECT (graph_window, points1, "points1");
  GLADE_HOOKUP_OBJECT (graph_window, goal_difference1, "goal_difference1");
  GLADE_HOOKUP_OBJECT (graph_window, goals_for1, "goals_for1");
  GLADE_HOOKUP_OBJECT (graph_window, goals_against1, "goals_against1");
  GLADE_HOOKUP_OBJECT (graph_window, money1, "money1");
  GLADE_HOOKUP_OBJECT (graph_window, average_attendance1, "average_attendance1");
  GLADE_HOOKUP_OBJECT (graph_window, button_close, "button_close");
  GLADE_HOOKUP_OBJECT (graph_window, alignment6, "alignment6");
  GLADE_HOOKUP_OBJECT (graph_window, hbox14, "hbox14");
  GLADE_HOOKUP_OBJECT (graph_window, image6, "image6");
  GLADE_HOOKUP_OBJECT (graph_window, label35, "label35");
  GLADE_HOOKUP_OBJECT_NO_REF (graph_window, tooltips, "tooltips");

  gtk_window_add_accel_group (GTK_WINDOW (graph_window), accel_group);

  return graph_window;
}

GtkWidget*
create_progressbar_window (void)
{
  GtkWidget *progressbar_window;
  GtkWidget *progressbar;

  progressbar_window = gtk_window_new (GTK_WINDOW_POPUP);
  gtk_window_set_title (GTK_WINDOW (progressbar_window), _("window1"));
  gtk_window_set_position (GTK_WINDOW (progressbar_window), GTK_WIN_POS_CENTER);

  progressbar = gtk_progress_bar_new ();
  gtk_widget_show (progressbar);
  gtk_container_add (GTK_CONTAINER (progressbar_window), progressbar);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (progressbar_window, progressbar_window, "progressbar_window");
  GLADE_HOOKUP_OBJECT (progressbar_window, progressbar, "progressbar");

  return progressbar_window;
}

GtkWidget*
create_window_warning (void)
{
  GtkWidget *window_warning;
  GtkWidget *vbox13;
  GtkWidget *image7;
  GtkWidget *label_warning;
  GtkWidget *button_warning;

  window_warning = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_container_set_border_width (GTK_CONTAINER (window_warning), 10);
  gtk_window_set_title (GTK_WINDOW (window_warning), _("window1"));
  gtk_window_set_position (GTK_WINDOW (window_warning), GTK_WIN_POS_CENTER);
  gtk_window_set_modal (GTK_WINDOW (window_warning), TRUE);

  vbox13 = gtk_vbox_new (FALSE, 10);
  gtk_widget_show (vbox13);
  gtk_container_add (GTK_CONTAINER (window_warning), vbox13);

  image7 = gtk_image_new_from_stock ("gtk-dialog-warning", GTK_ICON_SIZE_DIALOG);
  gtk_widget_show (image7);
  gtk_box_pack_start (GTK_BOX (vbox13), image7, FALSE, TRUE, 0);

  label_warning = gtk_label_new (_("label42"));
  gtk_widget_show (label_warning);
  gtk_box_pack_start (GTK_BOX (vbox13), label_warning, FALSE, FALSE, 0);

  button_warning = gtk_button_new_from_stock ("gtk-close");
  gtk_widget_show (button_warning);
  gtk_box_pack_start (GTK_BOX (vbox13), button_warning, FALSE, FALSE, 0);

  g_signal_connect ((gpointer) window_warning, "delete_event",
                    G_CALLBACK (on_button_warning_clicked),
                    NULL);
  g_signal_connect ((gpointer) window_warning, "destroy_event",
                    G_CALLBACK (on_button_warning_clicked),
                    NULL);
  g_signal_connect ((gpointer) button_warning, "clicked",
                    G_CALLBACK (on_button_warning_clicked),
                    NULL);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (window_warning, window_warning, "window_warning");
  GLADE_HOOKUP_OBJECT (window_warning, vbox13, "vbox13");
  GLADE_HOOKUP_OBJECT (window_warning, image7, "image7");
  GLADE_HOOKUP_OBJECT (window_warning, label_warning, "label_warning");
  GLADE_HOOKUP_OBJECT (window_warning, button_warning, "button_warning");

  return window_warning;
}

