#include "misc.h"

/**
   Print the contents of a GError (if it was set).
   If abort_program is TRUE, we free the memory
   and exit the game.
   @param error The GError we check.
   @param abort_program Whether or not we continue or exit the program.
*/
void
misc_print_error(GError *error, gboolean abort_program)
{
    gchar buf[SMALL];

    if(error == NULL)
	return;
    
    sprintf(buf, "%s", error->message);

    g_warning("error message: %s\n", buf);
    /*d*/
/*     show_popup_window(buf); */
    g_error_free(error);

    if(abort_program)
    {
	free_memory();
	
	if(gtk_main_level() > 0)
	    gtk_main_quit();

	exit(EXIT_PRINT_ERROR);
    }
}
