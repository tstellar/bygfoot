#include "gui.h"

/**
   The list of directories the find_support_file() function
   searches for support files (e.g. pixmaps or text files).
   @see find_support_file()
   @see add_support_directory_recursive()
*/
static GList *support_directories = NULL;

/**
   Add the specified directory to the list of directories find_support_file()
   searches for support files.
   Any subdirectories are added recursively.
   @param directory The full path of the directory to be added.
   @see find_support_file()
   @see #support_directories  
*/
void
add_support_directory_recursive                   (const gchar     *directory)
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
	    add_support_directory_recursive(fullpath);
	
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
   @see add_support_directory_recursive()
*/
gchar*
find_support_file                       (const gchar     *filename)
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
