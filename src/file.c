#include "file.h"
#include "free.h"
#include "main.h"
#include "misc.h"
#include "option.h"
#include "support.h"
#include "variables.h"

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
file_my_fopen(const gchar *filename, gchar *bits, FILE **fil, gboolean abort_program)
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
	main_exit_program(EXIT_FILE_OPEN_FAILED, NULL);

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

    misc_print_error(&error, FALSE);

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

/** Write the first directory called 'definitions' from the support
    directories array into the buffer.
    @param dir The string buffer we write the directory path into. */
void
file_get_definitions_dir(gchar *dir)
{
    GList *elem;

    strcpy(dir, "");

    elem = support_directories;
  
    while(elem != NULL)
    {
	if(g_str_has_suffix((gchar*)elem->data, "definitions") ||
	   g_str_has_suffix((gchar*)elem->data, "definitions/"))
	{
	    strcpy(dir, (gchar*)elem->data);
	    break;
	}
	
	elem = elem->next;
    }

    if(strlen(dir) == 0)
	main_exit_program(EXIT_DIR_OPEN_FAILED,
			  "Didn't find definitions directory.\n");
}

/** Read the file until the next line that's not a comment or
    a blank line. Split the line into the part before and after
    the first white space and copy them into the char arrays.
    Trailing and leading white spaces and trailing comments are stripped.
    @param fil The file stream.
    @param opt_name The first char array (an option name, mostly).
    @param opt_value The second array (an option value, mostly).
    @return TRUE if the file still contains lines to read, FALSE otherwise. */
gboolean
file_get_next_opt_line(FILE *fil, gchar *opt_name, gchar *opt_value)
{
    gint i;
    gchar trash[SMALL];
    gchar buf[BIG];

    strcpy(buf, "");
    strcpy(opt_name, "");
    strcpy(opt_value, "");

    while( (buf[0] == '#' || strlen(buf) == 0) &&
	   feof(fil) == 0)
	{
	    fscanf(fil, "%[\n \t]*", buf);
	    fscanf(fil, "%[^\n]", buf);
	}

    if(buf[0] != '#' && strlen(buf) != 0)
    {
	if(strlen(buf) > 100)
	    g_warning("\n the text file I'm reading contains a line longer than 100 chars.\n\n");

	for(i=0;i<strlen(buf);i++)
	    if(buf[i] == '#')
	    {
		buf[i] = '\0';
		break;
	    }

	for(i=strlen(buf) - 1;i>0;i--)
	    if(buf[i] == '\t' || buf[i] == ' ')
		buf[i] = '\0';
	    else
		break;
	
	sscanf(buf, "%[^ \t]%[^a-zA-Z0-9_-]%[^\n]", opt_name, trash, opt_value);
    }

    return (feof(fil) == 0);
}

/** Load a file containing name - value pairs into
    the specified array. */
void
file_load_opt_file(FILE *fil, OptionList *optionlist)
{
    gint i;
    gchar opt_name[SMALL], opt_value[SMALL];
    Option new;

    free_option_array(&optionlist->list, TRUE);
    g_datalist_init(&optionlist->datalist);

    while(file_get_next_opt_line(fil, opt_name, opt_value))
    {
	new.name = g_string_new(opt_name);	
	if(g_str_has_prefix(opt_name, "string_"))
	{
	    new.string_value = g_string_new(opt_value);
	    new.value = -1;
	}
	else
	{
	    new.string_value = NULL;
	    sscanf(opt_value, "%d", &new.value);
	}

	g_array_append_val(optionlist->list, new);
    }

    for(i=0;i<optionlist->list->len;i++)
	g_datalist_set_data(&optionlist->datalist, g_array_index(optionlist->list, Option, i).name->str,
			    &g_array_index(optionlist->list, Option, i));

    fclose(fil);
}

/** Load the options at the beginning of a new game from
    the configuration files. */
void
file_load_conf_files(void)
{
    FILE *fil = NULL;
    gchar *conf_file = file_find_support_file("bygfoot.conf");

    file_my_fopen(conf_file, "r", &fil, TRUE);
    g_free(conf_file);

    file_load_opt_file(fil, &options);

    file_my_fopen(opt_str("string_opt_constants_file"), "r", &fil, TRUE);
    file_load_opt_file(fil, &constants);
}

/** Load a user-specific conf file.
    @param user The user we load the file for. */
void
file_load_user_conf_file(User *user)
{
    FILE *fil = NULL;
    gchar *conf_file = NULL, buf[SMALL];

    sprintf(buf, "bygfoot_%s.conf", user->name->str);
    conf_file = file_find_support_file(buf);
    
    if(conf_file == NULL ||
       !file_my_fopen(conf_file, "r", &fil, FALSE))
    {
	g_free(conf_file);
	conf_file = file_find_support_file(opt_str("string_opt_default_user_conf_file"));
	file_my_fopen(conf_file, "r", &fil, TRUE);
    }

    file_load_opt_file(fil, &user->options);
}
