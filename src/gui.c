#include "gui.h"
#include "misc.h"
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
    const gchar *current_text = gtk_label_get_text(label);
    gchar buf[SMALL], buf2[SMALL];
    
    strcpy(buf, "");
    strcpy(buf2, "");

    if(number > 1000)
	misc_print_grouped_int(number, buf, FALSE);
    else
	sprintf(buf, "%d", number);

    if(!append)
	sprintf(buf2, "%s", buf);
    else
	sprintf(buf2, "%s%s", current_text, buf);

    gtk_label_set_text(label, buf2);
}

/* Set into or append an integer into a label.
   @param label The label.
   @param number The number.
   @param append Whether or not to append.
   @param precision Float precision to use. */
void
gui_label_set_text_from_float(GtkLabel *label, gfloat number,
			      gboolean append, gint precision)
{
    const gchar *current_text = gtk_label_get_text(label);
    gchar buf[SMALL];
    
    strcpy(buf, "");

    if(!append)
	sprintf(buf, "%.*f", precision, number);
    else
	sprintf(buf, "%s%.*f", current_text, precision, number);

    gtk_label_set_text(label, buf);
}

/* Show a window with a progress bar.
   @param value The value of the progress bar. If set to 1
   or < 0 the progress bar window gets destroyed.
   @param text The text to show in the progress bar. */
void
gui_show_progress(gfloat value, gchar *text)
{
    GtkProgressBar *progressbar = NULL;

    if(value == 1 || value < 0)
    {
	window_destroy(&window.progress, FALSE);
	return;
    }

    if(window.progress == NULL)
	window_create(WINDOW_PROGRESS);
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
