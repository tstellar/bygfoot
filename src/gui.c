/*
   gui.c

   Bygfoot Football Manager -- a small and simple GTK2-based
   football management game.

   http://bygfoot.sourceforge.net

   Copyright (C) 2005  Gyözö Both (gyboth@bygfoot.com)

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "gui.h"
#include "misc.h"
#include "option.h"
#include "support.h"
#include "variables.h"
#include "window.h"

/* Set into or append an integer into a label.
   @param label The label.
   @param number The number.
   @param append Whether or not to append.*/
void
gui_label_set_text_from_int(GtkLabel *label, gint number, gboolean append)
{
#ifdef DEBUG
    printf("gui_label_set_text_from_int\n");
#endif

    const gchar *current_text = gtk_label_get_text(label);
    gchar buf[SMALL], buf2[SMALL];
    
    strcpy(buf, "");
    strcpy(buf2, "");

    if(number > 1000)
	misc_print_grouped_int(number, buf);
    else
	sprintf(buf, "%d", number);

    if(!append)
	sprintf(buf2, "%s", buf);
    else
	sprintf(buf2, "%s%s", current_text, buf);

    gtk_label_set_text(label, buf2);
}

gdouble
gui_get_progress_bar_fraction(void)
{
    if(sett_int("int_opt_goto_mode"))
        return 0;
    
    return gtk_progress_bar_get_fraction(
        GTK_PROGRESS_BAR(lookup_widget(window.progress, "progressbar")));
}

/* Show a window with a progress bar.
   @param value The value of the progress bar. If set to 1
   or < 0 the progress bar window gets destroyed.
   @param text The text to show in the progress bar.
   @param pictype What kind of picture to show above the progress bar. */
void
gui_show_progress(gfloat value, const gchar *text, gint pictype)
{
#ifdef DEBUG
    printf("gui_show_progress\n");
#endif

    GtkProgressBar *progressbar = NULL;

    if(sett_int("int_opt_goto_mode"))
    {
	window_destroy(&window.progress);        
        return;  
    } 

    if(value == 1 || value < 0)
    {
	window_destroy(&window.progress);
	return;
    }

    if(window.progress == NULL)
	window_show_progress(pictype);

    progressbar = GTK_PROGRESS_BAR(lookup_widget(window.progress, "progressbar"));

    if(value >= 0 && value < 1)
	gtk_progress_bar_set_fraction(progressbar, value);
    else
	gtk_progress_bar_pulse(progressbar);

    if(text != NULL)
	gtk_progress_bar_set_text(progressbar, text);

    while(gtk_events_pending())
	gtk_main_iteration();
}

/** Set either the right pair of arrows atop the right
    treeview or the left pair or both to the specified
    sensitivity state. */
void
gui_set_arrow_pair(gint pair, gboolean state)
{
#ifdef DEBUG
    printf("gui_set_arrow_pair\n");
#endif

    gint i, j;
    GtkWidget *buttons[2][2] =
	{{lookup_widget(window.main ,"button_cl_back"),
	  lookup_widget(window.main ,"button_cl_forward")},
	 {lookup_widget(window.main ,"button_browse_back"),
	  lookup_widget(window.main ,"button_browse_forward")}};

    if(pair < 3)
	for(i=0;i<2;i++)
	    gtk_widget_set_sensitive(buttons[pair][i], state);
    else
	for(i=0;i<2;i++)
	    for(j=0;j<2;j++)
		gtk_widget_set_sensitive(buttons[i][j], state);
}

/** Examine the status variable and set the
    sensitivity of the arrows atop the right treeview
    accordingly. */
void
gui_set_arrows(void)
{
#ifdef DEBUG
    printf("gui_set_arrows\n");
#endif

    gui_set_arrow_pair(3, FALSE);

    if(stat0 == STATUS_SHOW_FIXTURES ||
       stat0 == STATUS_BROWSE_TEAMS ||
       stat0 == STATUS_SHOW_SEASON_HISTORY)
	gui_set_arrow_pair(3, TRUE);
    else if(stat0 == STATUS_SHOW_PLAYER_INFO ||
	    stat0 == STATUS_SHOW_FIXTURES_WEEK)
	gui_set_arrow_pair(1, TRUE);
    else if(stat0 == STATUS_SHOW_TABLES ||
	    stat0 == STATUS_SHOW_PLAYER_LIST ||
	    stat0 == STATUS_SHOW_LEAGUE_STATS)
	gui_set_arrow_pair(0, TRUE);
}

/** Set the sensitivity of the live game window meters and their
    eventboxes. */
void
gui_set_sensitive_lg_meters(gboolean state)
{
#ifdef DEBUG
    printf("gui_set_sensitive_lg_meters\n");
#endif

    gtk_widget_set_sensitive(lookup_widget(window.live, "image_lg_style"), state);
    gtk_widget_set_sensitive(lookup_widget(window.live, "eventbox_lg_style"), state);
    gtk_widget_set_sensitive(lookup_widget(window.live, "image_lg_boost"), state);
    gtk_widget_set_sensitive(lookup_widget(window.live, "eventbox_lg_boost"), state);    
}
