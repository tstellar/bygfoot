/*
   file.c

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

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "file.h"
#include "free.h"
#include "main.h"
#include "misc.h"
#include "option.h"
#include "support.h"
#include "variables.h"

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
   @param warning Whether to show a warning if we don't find the file.
   @return A pointer to the full path string of the file or NULL if
   we didn't find the file. The gchar* must be freed.
   @see #support_directories
   @see file_add_support_directory_recursive()
*/
gchar*
file_find_support_file                       (const gchar     *filename, gboolean warning)
{
    GList *elem = support_directories;

    while (elem)
    {
	gchar *pathname = g_strdup_printf ("%s%s%s", (gchar*)elem->data,
					   G_DIR_SEPARATOR_S, filename);
	if (g_file_test (pathname, G_FILE_TEST_EXISTS) &&
	    !g_file_test(pathname, G_FILE_TEST_IS_DIR))
	    return pathname;
      
	g_free (pathname);
	elem = elem->next;
    }
  
    if(warning)
	g_warning("file_find_support_file: file '%s' not found.", filename);

    return NULL;
}

/** Execute command with 'system' and give a warning if return value is -1.
    @return TRUE on success, FALSE, otherwise. */
gboolean
file_my_system(const gchar *command)
{
    if(system(command) != 0)
    {
	g_warning("file_my_system: system returned -1 when executing '%s'.", command);

	if(!os_is_unix)
	{
	    g_warning("Press RETURN to try to continue.");
	    getchar();
	}

	return FALSE;
    }

    return TRUE;
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
    gchar *support_file = NULL;

    *fil = fopen(filename, bits);

    if(*fil != NULL)
	return TRUE;

    support_file = file_find_support_file(filename, FALSE);
    *fil = fopen(support_file, bits);

    if(*fil != NULL)
    {
	g_free(support_file);
	return TRUE;
    }

    g_warning("Could not open file '%s' in mode '%s'.\n", filename, bits);

    if(abort_program)
	main_exit_program(EXIT_FILE_OPEN_FAILED, NULL);

    return FALSE;
}

/** Create a $HOME/.bygfoot dir and other stuff if necessary. */
void
file_check_home_dir_create_dirs(void)
{
    gint i;
    gchar *dirs[3] =
	{HOMEDIRNAME,
	 HOMEDIRNAME G_DIR_SEPARATOR_S "definitions",
	 HOMEDIRNAME G_DIR_SEPARATOR_S "saves"};
    const gchar *home = g_get_home_dir();
    gchar buf[SMALL];

    for(i=0;i<3;i++)
    {
	sprintf(buf, "%s%s%s", home, G_DIR_SEPARATOR_S, dirs[i]);
	if(!g_file_test(buf, G_FILE_TEST_EXISTS))
	    file_mkdir(buf);
    }
}

/** Add the basic config files to the list of file to copy
    into the user home dir. */
void
file_check_home_dir_get_conf_files(GPtrArray **files_to_copy)
{
    gint i;
    gchar *conf_files[2] =
	{"bygfoot.conf",
	 "bygfoot_user.conf"};
    const gchar *home = g_get_home_dir();
    gchar *conf_file = NULL;
    gchar buf[SMALL];

    for(i=0;i<2;i++)
    {
	sprintf(buf, "%s%s%s%s%s", home, G_DIR_SEPARATOR_S, 
		HOMEDIRNAME, G_DIR_SEPARATOR_S, conf_files[i]);
	if(!g_file_test(buf, G_FILE_TEST_EXISTS))
	{
	    conf_file = file_find_support_file(conf_files[i], TRUE);	    
	    g_ptr_array_add(files_to_copy[0], g_strdup(conf_file));
	    g_ptr_array_add(files_to_copy[1], g_strdup(buf));
	}
    }
}

/** Add the xml definition files to the list of files to copy
    into the home dir. */
void
file_check_home_dir_get_definition_dir(const gchar *dirname, const gchar *basename,
				       GPtrArray **files_to_copy)
{
    gint i;
    gchar buf[SMALL], buf2[SMALL];
    const gchar *home = g_get_home_dir();
    GPtrArray *dir_contents = NULL;

    sprintf(buf, "%s%s%s%s%s", home, G_DIR_SEPARATOR_S,
	    HOMEDIRNAME, G_DIR_SEPARATOR_S, basename);

    if(!g_file_test(buf, G_FILE_TEST_EXISTS))
	file_mkdir(buf);

    dir_contents = file_dir_get_contents(dirname, "", "");

    for(i=0;i<dir_contents->len;i++)
    {
	sprintf(buf, "%s%s%s%s%s%s%s", home, G_DIR_SEPARATOR_S,
		HOMEDIRNAME, G_DIR_SEPARATOR_S, basename, G_DIR_SEPARATOR_S,
		(gchar*)g_ptr_array_index(dir_contents, i));
    
	if(g_str_has_suffix((gchar*)g_ptr_array_index(dir_contents, i), ".xml") &&
	   !g_file_test(buf, G_FILE_TEST_EXISTS))
	{
        
	    sprintf(buf2, "%s%s%s", dirname, G_DIR_SEPARATOR_S,
		    (gchar*)g_ptr_array_index(dir_contents, i));
	    g_ptr_array_add(files_to_copy[0], g_strdup(buf2));
	    g_ptr_array_add(files_to_copy[1], g_strdup(buf));
	}
	else
	{
	    sprintf(buf, "%s%s%s", dirname, G_DIR_SEPARATOR_S,
		    (gchar*)g_ptr_array_index(dir_contents, i));

	    if(g_file_test(buf, G_FILE_TEST_IS_DIR))
	    {
		sprintf(buf2, "%s%s%s", basename, G_DIR_SEPARATOR_S,
			(gchar*)g_ptr_array_index(dir_contents, i));
		file_check_home_dir_get_definition_dir(buf, buf2, files_to_copy);
	    }
	}
    }
    
    free_gchar_array(&dir_contents);
}

/** Step through the support dirs to find xml definition files
    that get copied into the home dir. */
void
file_check_home_dir_get_definition_files(GPtrArray **files_to_copy)
{
    GList *elem = support_directories;
  
    while(elem != NULL)
    {    
	if(g_str_has_suffix((gchar*)elem->data, "definitions"))
	    file_check_home_dir_get_definition_dir((const gchar*)elem->data, 
						   "definitions",
						   files_to_copy);

	elem = elem->next;
    }
}

/** Execute the copy commands on the files assembled earlier. */
void
file_check_home_dir_copy_files(GPtrArray **files_to_copy)
{
    gint i;
    gchar buf[SMALL];
    const gchar *filename = NULL;
    GtkLabel *label_splash_progress =
	GTK_LABEL(lookup_widget(window.splash, "label_splash_progress"));
    GtkProgressBar *progressbar_splash =
	GTK_PROGRESS_BAR(lookup_widget(window.splash, "progressbar_splash"));
    GtkWidget *hint_buttons[2] = 
	{lookup_widget(window.splash, "button_splash_hint_back"),
	 lookup_widget(window.splash, "button_splash_hint_next")};
    GtkWidget *hbox_buttons = lookup_widget(window.splash, "hbox3");

    if(files_to_copy[0]->len > 0)
    {
	gtk_widget_set_sensitive(hint_buttons[0], FALSE);
	gtk_widget_set_sensitive(hint_buttons[1], FALSE);
	gtk_widget_set_sensitive(hbox_buttons, FALSE);
    }

    for(i=0;i<files_to_copy[0]->len;i++)
    {
	filename = g_strrstr((gchar*)g_ptr_array_index(files_to_copy[0], i), 
			     G_DIR_SEPARATOR_S);
	if(filename == NULL)
	    filename = (gchar*)g_ptr_array_index(files_to_copy[0], i);
	else
	    filename = filename + 1;

	sprintf(buf, _("Copying %s"), filename);
	gtk_label_set_text(label_splash_progress, buf);
	gtk_progress_bar_set_fraction(progressbar_splash, 
				      (gdouble)i /(gdouble)files_to_copy[0]->len);
	while(gtk_events_pending())
	    gtk_main_iteration();

	file_copy_file((gchar*)g_ptr_array_index(files_to_copy[0], i),
		       (gchar*)g_ptr_array_index(files_to_copy[1], i));
    }

    gtk_progress_bar_set_fraction(progressbar_splash, 1);
    gtk_label_set_text(label_splash_progress, _("Ready"));

    while(gtk_events_pending())
	gtk_main_iteration();    

    if(files_to_copy[0]->len > 0)
    {
	gtk_widget_set_sensitive(hint_buttons[0], TRUE);
	gtk_widget_set_sensitive(hint_buttons[1], TRUE);
	gtk_widget_set_sensitive(hbox_buttons, TRUE);
    }
}

/** Copy some files into the user's home directory. */
gboolean
file_check_home_dir(void)
{
    GPtrArray *files_to_copy[2] = {g_ptr_array_new(),
				   g_ptr_array_new()};

    file_check_home_dir_create_dirs();
    file_check_home_dir_get_conf_files(files_to_copy);
    file_check_home_dir_get_definition_files(files_to_copy);
    file_check_home_dir_copy_files(files_to_copy);

    free_gchar_array(&(files_to_copy[0]));
    free_gchar_array(&(files_to_copy[1]));

    return FALSE;
}

/**
   Retrieve those files in the given directory
   that start with the given prefix and suffix. The file names are stored
   in an array of strings.
   @param dir_name The full path to the directory.
   @param prefix The prefix that files must have to be included.
   @param suffix The suffix that files must have to be included.
   @return A GPtrArray with pointers to the strings of the file
   names. The array must be freed with free_gchar_array().
*/
GPtrArray*
file_dir_get_contents(const gchar *dir_name, const gchar *prefix, const gchar *suffix)
{
    GError *error = NULL;
    GDir *dir = g_dir_open(dir_name, 0, &error);
    GPtrArray *contents = g_ptr_array_new();
    const gchar *file = NULL;

    misc_print_error(&error, FALSE);

    if(dir == NULL)
	return contents;
    
    file = g_dir_read_name(dir);

    if(file == NULL)
	return contents;

    while(file != NULL)
    {
	if(g_str_has_prefix(file, prefix) &&
	   g_str_has_suffix(file, suffix))
	    g_ptr_array_add(contents, (gpointer)g_strdup(file));

	file = g_dir_read_name(dir);
    }

    g_dir_close(dir);

    return contents;
}

/** Return the country definition files found in the support dirs. */
GPtrArray*
file_get_country_files(void)
{
    gint i;
    GList *elem = support_directories;
    GPtrArray *country_files = g_ptr_array_new();
    GPtrArray *dir_contents = NULL;

    while(elem != NULL)
    {
	dir_contents = file_dir_get_contents((gchar*)elem->data, "country_", ".xml");

	for(i=0;i<dir_contents->len;i++)
	    if(!query_misc_string_in_array((gchar*)g_ptr_array_index(dir_contents, i),
					   country_files))
		g_ptr_array_add(country_files, 
				g_strdup((gchar*)g_ptr_array_index(dir_contents, i)));

	free_gchar_array(&dir_contents);

	elem = elem->next;
    }

    return country_files;
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
	if(strlen(buf) > 1000)
	    g_warning("\n the text file I'm reading contains a line longer than 1000 chars.\n\n");

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
    
	sscanf(buf, "%[^ \t]%[ \t]%[^\n]", opt_name, trash, opt_value);
    }

    return (feof(fil) == 0);
}

/** Save an optionlist to a file. */
void
file_save_opt_file(const gchar *filename, OptionList *optionlist)
{
    gint i;
    FILE *fil = NULL;

    file_my_fopen(filename, "w", &fil, TRUE);

    for(i=0;i<optionlist->list->len;i++)
	if(g_str_has_prefix(g_array_index(optionlist->list, Option, i).name, "string_"))
	    fprintf(fil, "%s %s\n", g_array_index(optionlist->list, Option, i).name,
		    g_array_index(optionlist->list, Option, i).string_value);
	else
	    fprintf(fil, "%s %d\n", g_array_index(optionlist->list, Option, i).name,
		    g_array_index(optionlist->list, Option, i).value);
    
    fclose(fil);
}

/** Load a file containing name - value pairs into
    the specified array. */
void
file_load_opt_file(const gchar *filename, OptionList *optionlist)
{
    gint i;
    gchar opt_name[SMALL], opt_value[SMALL];
    Option new;
    FILE *fil = NULL;

    free_option_list(optionlist, TRUE);

    file_my_fopen(filename, "r", &fil, TRUE);

    while(file_get_next_opt_line(fil, opt_name, opt_value))
    {
	new.name = g_strdup(opt_name);
	if(g_str_has_prefix(opt_name, "string_"))
	{
	    new.string_value = g_strdup(opt_value);
	    new.value = -1;
	}
	else
	{
	    new.string_value = NULL;
	    sscanf(opt_value, "%d", &new.value);
	}
	g_array_append_val(optionlist->list, new);

	if((g_str_has_suffix(new.name, "_unix") && os_is_unix) ||
	   (g_str_has_suffix(new.name, "_win32") && !os_is_unix))
	{
	    strcpy(opt_name, new.name);
	    opt_name[strlen(new.name) - (os_is_unix ? 5 : 6)] = '\0';
	    new.name = g_strdup(opt_name);
	    new.string_value = g_strdup(opt_value);
	    g_array_append_val(optionlist->list, new);
	}
    }

    for(i=0;i<optionlist->list->len;i++)
	g_datalist_set_data(&optionlist->datalist, g_array_index(optionlist->list, Option, i).name,
			    &g_array_index(optionlist->list, Option, i));

    fclose(fil);
}

/** Load the options at the beginning of a new game from
    the configuration files. */
void
file_load_conf_files(void)
{
    gint i;
    gchar *conf_file = file_find_support_file("bygfoot.conf", TRUE);

    file_load_opt_file(conf_file, &options);
    g_free(conf_file);

    file_load_opt_file(opt_str("string_opt_constants_file"), &constants);
    file_load_opt_file(opt_str("string_opt_appearance_file"), &constants_app);
    file_load_opt_file("bygfoot_tokens", &tokens);
    file_load_opt_file("bygfoot_hints", &hints);

    for(i=0;i<tokens.list->len;i++)
	g_array_index(tokens.list, Option, i).value = i;
}

/** Load a user-specific conf file.
    @param user The user we load the file for. */
void
file_load_user_conf_file(User *user)
{
    FILE *fil = NULL;
    gchar *conf_file = NULL, buf[SMALL];

    sprintf(buf, "bygfoot_%s.conf", user->name);
    conf_file = file_find_support_file(buf, FALSE);

    if(conf_file == NULL ||
       !file_my_fopen(conf_file, "r", &fil, FALSE))
    {
	g_free(conf_file);
	conf_file = 
	    file_find_support_file(opt_str("string_opt_default_user_conf_file"), TRUE);
    }

    file_load_opt_file(conf_file, &user->options);
    g_free(conf_file);
}

/** Return the primary support dir (probably './support_files' or 
    the Bygfoot dir in $HOME). */
const gchar*
file_get_first_support_dir(void)
{
    GList *elem = support_directories;
  
    while (elem)
    {
	if(g_str_has_suffix((gchar*)elem->data, HOMEDIRNAME) ||
	   g_str_has_suffix((gchar*)elem->data, "support_files"))
	    return (const gchar*)elem->data;

	elem = elem->next;
    }

    main_exit_program(EXIT_NO_SUPPORT_DIR,
		      "file_get_first_support_dir: no primary support dir found.");

    return NULL;
}

/** Return the first support directory which has the given suffix.*/
const gchar*
file_get_first_support_dir_suffix(const gchar *suffix)
{
    GList *elem = support_directories;
  
    while (elem)
    {
	if(g_str_has_suffix((gchar*)elem->data, suffix))
	    return (const gchar*)elem->data;

	elem = elem->next;
    }

    g_warning("file_get_first_support_dir_suffix: no dir with suffix %s found.",
	      suffix);

    return NULL;    
}

/** Compress the files starting with the prefix.
    @param destfile The name of the file to create. */
void
file_compress_files(const gchar *destfile, const gchar *prefix)
{
    gint i;
    gchar buf[SMALL];
    gchar *basename = g_path_get_basename(prefix),
	*dirname = g_path_get_dirname(prefix),
	*zipbasename = g_path_get_basename(destfile),
	*pwd = g_get_current_dir();
    GPtrArray *files = file_dir_get_contents(dirname, basename, "");
    
    chdir(dirname);
    
    if (os_is_unix)
        sprintf(buf, "%s %s %s", 
		const_str("string_fs_compress_command"), 
		const_str("string_fs_compress_switches"),
		zipbasename);
    else
        sprintf(buf, "\"%s%s%s\" %s %s", pwd, G_DIR_SEPARATOR_S, 
		const_str("string_fs_compress_command"),
		const_str("string_fs_compress_switches"),
		zipbasename);

    for(i=0;i<files->len;i++)
    {
	strcat(buf, " ");
	strcat(buf, (gchar*)g_ptr_array_index(files, i));
    }

    file_my_system(buf);

    chdir(pwd);
    
    sprintf(buf, "%s%s%s*", dirname, G_DIR_SEPARATOR_S, basename);
    file_remove_files(buf);

    free_gchar_array(&files);

    g_free(basename);
    g_free(dirname);
    g_free(zipbasename);
    g_free(pwd);
}

/** Decompress the specified file. */
void
file_decompress(const gchar *filename)
{
    gchar buf[SMALL];
    gchar *dirname = g_path_get_dirname(filename),
	*basename = g_path_get_basename(filename),
	*pwd = g_get_current_dir();

    chdir(dirname);
	
    if (os_is_unix)
	sprintf(buf, "%s %s %s", 
		const_str("string_fs_uncompress_command"), 
		const_str("string_fs_uncompress_switches"),
		basename);
    else
	sprintf(buf, "\"%s%s%s\" %s %s", pwd, G_DIR_SEPARATOR_S,
		const_str("string_fs_uncompress_command"), 
		const_str("string_fs_uncompress_switches"), 
		basename);

    file_my_system(buf);

    g_free(dirname);
    g_free(basename);

    chdir(pwd);
    g_free(pwd);
}

/** Execute the appropriate remove command with 'files'
    as argument (can be directories or a regexp, too). */
void
file_remove_files(const gchar *files)
{
    gchar buf[SMALL];

    if(os_is_unix)
	sprintf(buf, "%s %s", const_str("string_fs_remove_file_command"), files);
    else
	sprintf(buf, "%s \"%s\"", const_str("string_fs_remove_file_command"), files);

    file_my_system(buf);
}

/** Execute the appropriate copy command. */
void
file_copy_file(const gchar *source_file, const gchar *dest_file)
{
    gchar buf[SMALL];

    if(os_is_unix)
	sprintf(buf, "%s %s %s", const_str("string_fs_copy_file_command"),
		source_file, dest_file);
    else
	sprintf(buf, "%s \"%s\" \"%s\"", const_str("string_fs_copy_file_command"),
		source_file, dest_file);

    file_my_system(buf);
}

/** Find out where the Bygfoot directory we can write to resides
    and write the location into the string. */
void
file_get_bygfoot_dir(gchar *dir)
{
    const gchar *home = g_get_home_dir();
    gchar *pwd = g_get_current_dir();
    
    if(os_is_unix)
	sprintf(dir, "%s%s%s", home, G_DIR_SEPARATOR_S, HOMEDIRNAME);
    else
	sprintf(dir, "%s%s", pwd, G_DIR_SEPARATOR_S);

    g_free(pwd);
}
