#include "gui.h"
#include "misc.h"
#include "variables.h"

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

