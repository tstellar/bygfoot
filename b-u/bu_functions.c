#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "bygfoot-update.h"
#include "bu_functions.h"
#include "bu_callbacks.h"
#include "bu_interface.h"
#include "bu_support.h"
#include <string.h>

void
print_message(gchar *text)
{
    GtkWidget *textview = 
	lookup_widget(update_window, "textview");
    GtkTextBuffer *textbuffer =
        gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
    GtkTextIter iter;
    gchar *converted = g_locale_to_utf8(text, -1, NULL, NULL, NULL);

    if(converted != NULL)
    {
	gtk_text_buffer_get_start_iter(textbuffer, &iter);    
	gtk_text_buffer_insert(textbuffer, &iter, "\n\n", -1);
	gtk_text_buffer_get_start_iter(textbuffer, &iter);    
	gtk_text_buffer_insert(textbuffer, &iter, converted, -1);
	g_free(converted);
    }   
}

void
update_download(gboolean get_version)
{
    GtkWidget *optionmenu_format =
	lookup_widget(update_window, "optionmenu_format");
    GtkWidget *entry_tmpdir = 
	lookup_widget(update_window, "entry_tmpdir");
    GtkWidget *optionmenu_type =
	lookup_widget(update_window, "optionmenu_type");
    gint exit;
    gchar *output = NULL;
    gchar type[SMALL], format[SMALL];
    gchar *formats[5] = {"rpm", "bin", "src", "deb", "srpm"};
    gchar swtch[SMALL];

    if(gtk_option_menu_get_history(GTK_OPTION_MENU(optionmenu_type)) == 0)
	strcpy(type, "stable");
    else
	strcpy(type, "unstable");

    strcpy(format, formats[gtk_option_menu_get_history(GTK_OPTION_MENU(optionmenu_format))]);

    if(get_version)
	strcpy(swtch, "-V");
    else
	strcpy(swtch, "-n");

    g_string_printf(command_line, "%s %s -t %s -T %s -f %s",
		    update_script->str, swtch, gtk_entry_get_text(GTK_ENTRY(entry_tmpdir)),
		    type, format);

    if(!get_version)
	return;

    printf("comml +%s+\n", command_line->str);
	
    g_spawn_command_line_sync(command_line->str, &output, NULL, &exit, NULL);

    if(output != NULL)
    {	
	print_message(output);
	g_free(output);
    }    
}

void
update_src(void)
{
    GtkWidget *entry_packdir = 
	lookup_widget(update_window, "entry_packdir");
    GtkWidget *entry_tmpdir = 
	lookup_widget(update_window, "entry_tmpdir");
    GtkWidget *checkbutton_recompile =
	lookup_widget(update_window, "checkbutton_recompile");
    gchar buf[SMALL];

    sprintf(buf, "%s -p %s -t %s -a", update_script->str,
	    gtk_entry_get_text(GTK_ENTRY(entry_packdir)),
	    gtk_entry_get_text(GTK_ENTRY(entry_tmpdir)));

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_recompile)))
	strcat(buf, " -r");
    else
	strcat(buf, " -R");

    g_string_printf(command_line, "%s", buf);
}

void
callback_update(void)
{
    GtkWidget *entry_countrydir = 
	lookup_widget(update_window, "entry_countrydir");
    GtkWidget *radiobutton_src =
	lookup_widget(update_window, "radiobutton_src");
    GtkWidget *radiobutton_country =
	lookup_widget(update_window, "radiobutton_country");
    gint exit;
    gchar *output = NULL;

    if(gtk_toggle_button_get_active(
	   GTK_TOGGLE_BUTTON(radiobutton_src)))
	update_src();
    else if(gtk_toggle_button_get_active(
		GTK_TOGGLE_BUTTON(radiobutton_country)))
	g_string_printf(command_line, "%s -c -C %s", update_script->str,
			gtk_entry_get_text(GTK_ENTRY(entry_countrydir)));
    else
	update_download(FALSE);

    printf("comml +%s+\n", command_line->str);
	
    g_spawn_command_line_sync(command_line->str, &output, NULL, &exit, NULL);

    if(output != NULL)
    {	
	print_message(output);
	g_free(output);
    }
}
