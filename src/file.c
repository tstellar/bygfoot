#include "file.h"
#include "free.h"
#include "main.h"
#include "misc.h"
#include "support.h"

/**
   The list of directories the file_find_support_file() function
   searches for support files (e.g. pixmaps or text files).
   @see file_find_support_file()
   @see file_add_support_directory_recursive()
*/
static GList *support_directories = NULL;

/**
   Add the specified directory to the list of directories file_find_support_file()
   searches for support files.
   Any subdirectories are added recursively.
   @param directory The full path of the directory to be added.
   @see file_find_support_file()
   @see #support_directories  
*/
void
file_add_support_directory_recursive                   (const gchar     *directory)
{
    GDir *newdir =
	g_dir_open(directory, 0, NULL);
    const gchar *file;
    gchar *fullpath;

    if(newdir == NULL)
	return;

    add_pixmap_directory(directory);
    support_directories = g_list_prepend (support_directories,
					  g_strdup (directory));

    while(TRUE)
    {
	file = g_dir_read_name(newdir);

	if(file == NULL)
	    break;

	fullpath = g_strdup_printf ("%s%s%s", directory,
				    G_DIR_SEPARATOR_S, file);

	if(g_file_test(fullpath, G_FILE_TEST_IS_DIR))
	    file_add_support_directory_recursive(fullpath);
	
	g_free(fullpath);
    }

    g_dir_close(newdir);
}

/**
   Search the list of support directories for a given file and return
   the full path name.
   The return value must be freed.
   @param filename The name of the file we look for (without path).
   @return A pointer to the full path string of the file or NULL if
   we didn't find the file. The gchar* must be freed.
   @see #support_directories
   @see file_add_support_directory_recursive()
*/
gchar*
file_find_support_file                       (const gchar     *filename)
{
  GList *elem;

  /* We step through each of the pixmaps directory to find it. */
  elem = support_directories;
  while (elem)
    {
      gchar *pathname = g_strdup_printf ("%s%s%s", (gchar*)elem->data,
                                         G_DIR_SEPARATOR_S, filename);
      if (g_file_test (pathname, G_FILE_TEST_EXISTS))
        return pathname;

      g_free (pathname);
      elem = elem->next;
    }
  return NULL;
}

/** A custom function opening files.
    @param filename The full path to the file or a file name from the support files.
    @param bits The mode we use, e.g. "r" or "w". @see fopen()
    @param fil The file pointer that will point to the opened stream.
    @param abort_program Whether to abort the program if we encounter an error.
    @return TRUE on success, FALSE otherwise. */
gboolean
my_fopen(const gchar *filename, gchar *bits, FILE **fil, gboolean abort_program)
{
    gchar buf[SMALL];
    gchar *support_file = file_find_support_file(filename);
    *fil = fopen(filename, bits);

    if(*fil != NULL)
    {
	g_free(support_file);
	return TRUE;
    }

    *fil = fopen(support_file, bits);
    if(*fil != NULL)
    {
	g_free(support_file);
	return TRUE;
    }

    sprintf(buf, "Could not open file '%s' in mode '%s'.\n", filename, bits);

    g_warning(buf);
    /*d*/
/*     show_popup_window(buf, NULL); */

    if(abort_program)
	main_exit_program(EXIT_FILE_OPEN_FAILED);

    return FALSE;
}

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
