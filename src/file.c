#include "file.h"

/**
   Retrieve those files in the given directory
   that start with the given prefix. The file names are stored
   in an array of GStrings.
   @param dir_name The full path to the directory.
   @param prefix The prefix that files must have to be included.
   @return A GPtrArray with pointers to the GStrings of the file
   names. The GStrings and the array must be freed with file_dir_free_contents().
   @see file_dir_free_contents()
*/
GPtrArray*
file_dir_get_contents(const gchar *dir_name, const gchar *prefix)
{
    GError *error = NULL;
    GDir *dir = g_dir_open(dir_name, 0, &error);
    GPtrArray *contents = NULL;
    GString *new = NULL;
    const gchar *file = NULL;

    misc_print_error(error, FALSE);

    if(dir == NULL)
	return NULL;
    
    file = g_dir_read_name(dir);

    if(file != NULL)
	contents = g_ptr_array_new();

    while(file != NULL)
    {
	if(g_str_has_prefix(file, prefix))
	{
	    new = g_string_new(file);
	    g_ptr_array_add(contents, (gpointer)new);
	}
	file = g_dir_read_name(dir);
    }

    g_dir_close(dir);

    return contents;
}

/**
   Free a GPtrArray containing GStrings.
   @param dir_contents The array to be freed.
*/
void
file_dir_free_contents(GPtrArray *dir_contents)
{
    gint i;

    if(dir_contents == NULL)
	return;

    for(i=0;i<dir_contents->len;i++)
	g_string_free((GString*)g_ptr_array_index(dir_contents, i), TRUE);

    g_ptr_array_free(dir_contents, TRUE);
}
