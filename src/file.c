#include "file.h"
#include "free.h"
#include "main.h"
#include "misc.h"
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
   a blank line and copy the line into buf.
   @param fil The file stream.
   @param buf The buffer we fill.
   @return TRUE if the file still contains lines to read, FALSE otherwise. */
gboolean
file_get_next_line(FILE *fil, gchar *buf)
{
    gchar local_buf[BIG];

    strcpy(local_buf, "");

    while( (local_buf[0] == '#' || strlen(local_buf) == 0) &&
	   feof(fil) == 0)
	{
	    fscanf(fil, "%[\n \t]*", local_buf);
	    fscanf(fil, "%[^\n]", local_buf);
	}

    if(local_buf[0] != '#' && strlen(local_buf) != 0)
    {
	if(strlen(local_buf) > 100)
	    g_warning("\n the text file I'm reading contains a line longer than 100 chars.\n\n");

	if(buf != NULL)
	    strcpy(buf, local_buf);
    }

    return (feof(fil) == 0);
}

/** Supply the strings in the config file that belong to the options.
    @param opt_names The array we write the optionstrings into.
    @param player_list_att_names The array we write the player list attribute strings into. */
void
file_write_opt_names(gchar opt_names[][50], gchar player_list_att_names[][PLAYER_LIST_ATTRIBUTE_END][50])
{
    gint i, j;

    strcpy(opt_names[OPT_BOOL_CONF_NEW_ROUND], "confirm_new_week_round");
    strcpy(opt_names[OPT_BOOL_CONF_UNFIT], "confirm_unfit");
    strcpy(opt_names[OPT_BOOL_CONF_QUIT], "confirm_quit");
    strcpy(opt_names[OPT_BOOL_OVERWRITE], "save_will_ovewrite");
    strcpy(opt_names[OPT_BOOL_SHOW_LIVE], "show_live_game");
    strcpy(opt_names[OPT_LIVE_SPEED], "live_game_speed");
    strcpy(opt_names[OPT_BOOL_LIVE_TENDENCY], "show_tendency_bar");
    strcpy(opt_names[OPT_BOOL_MAXIMIZE], "maximize_main_window");

    strcpy(opt_names[OPT_BOOL_NOTIFY], "notify_transfer");
    strcpy(opt_names[OPT_NOTIFY_POS], "notify_pos");
    strcpy(opt_names[OPT_NOTIFY_LEAGUE_UPPER], "notify_league_upper");
    strcpy(opt_names[OPT_NOTIFY_LEAGUE_LOWER], "notify_league_lower");
    strcpy(opt_names[OPT_BOOL_NOTIFY_CUPS], "notify_cups");    
    strcpy(opt_names[OPT_NOTIFY_AGE_UPPER], "notify_age_upper");
    strcpy(opt_names[OPT_NOTIFY_AGE_LOWER], "notify_age_lower");
    strcpy(opt_names[OPT_NOTIFY_SKILL_UPPER], "notify_skill_upper");
    strcpy(opt_names[OPT_NOTIFY_SKILL_LOWER], "notify_skill_lower");
    strcpy(opt_names[OPT_NOTIFY_ETAL_UPPER], "notify_etal_upper");
    strcpy(opt_names[OPT_NOTIFY_ETAL_LOWER], "notify_etal_lower");
    strcpy(opt_names[OPT_NOTIFY_VALUE], "notify_value");

    strcpy(opt_names[OPT_BOOL_JOBS], "show_job_offers");
    strcpy(opt_names[OPT_MESS], "message_style");
    strcpy(opt_names[OPT_BOOL_PREFER_MESS], "prefer_messages");

    strcpy(opt_names[OPT_BOOL_AUTOSAVE], "autosave");
    strcpy(opt_names[OPT_AUTOSAVE_INTERVAL], "autosave_interval");

    strcpy(opt_names[OPT_BOOL_SORT_TRANSFERS], "sort_transfer_list");
    strcpy(opt_names[OPT_SORT_TRANSFERS_ATTRIBUTE], "sort_transfer_attribute");
    strcpy(opt_names[OPT_BOOL_REARRANGE], "reaarrange_adapts");
    strcpy(opt_names[OPT_BOOL_SWAP], "swap_adapts");

    strcpy(opt_names[OPT_HISTORY_TEAM_INTERVAL], "history_team_interval");
    strcpy(opt_names[OPT_HISTORY_PLAYER_INTERVAL], "history_player_interval");
    strcpy(opt_names[OPT_HISTORY_TEAM_MAX], "history_team_max");
    strcpy(opt_names[OPT_HISTORY_PLAYER_MAX], "history_player_max");
    strcpy(opt_names[OPT_BOOL_HISTORY_TEAM_DELETE], "history_team_delete");
    strcpy(opt_names[OPT_BOOL_HISTORY_PLAYER_DELETE], "history_player_delete");

    strcpy(opt_names[OPT_BOOL_BOOST], "boost");
    strcpy(opt_names[OPT_BOOL_COMPRESS], "compress");
    strcpy(opt_names[OPT_BOOL_OBJECTIVE], "objective");
    strcpy(opt_names[OPT_PENALTY_SHOOTER], "penalty_shooter");

    for(i=0;i<2;i++)
    {
	for(j=0;j<PLAYER_LIST_ATTRIBUTE_END;j++)
	    sprintf(player_list_att_names[i][j], "pl%d_att_", i + 1);
	
	strcat(player_list_att_names[i][PLAYER_LIST_ATTRIBUTE_NAME], "name");
	strcat(player_list_att_names[i][PLAYER_LIST_ATTRIBUTE_CPOS], "cpos");
	strcat(player_list_att_names[i][PLAYER_LIST_ATTRIBUTE_POS], "pos");
	strcat(player_list_att_names[i][PLAYER_LIST_ATTRIBUTE_CSKILL], "cskill");
	strcat(player_list_att_names[i][PLAYER_LIST_ATTRIBUTE_SKILL], "skill");
	strcat(player_list_att_names[i][PLAYER_LIST_ATTRIBUTE_FITNESS], "fitness");
	strcat(player_list_att_names[i][PLAYER_LIST_ATTRIBUTE_GAMES], "games");
	strcat(player_list_att_names[i][PLAYER_LIST_ATTRIBUTE_GOALS], "goals");
	strcat(player_list_att_names[i][PLAYER_LIST_ATTRIBUTE_STATUS], "status");
	strcat(player_list_att_names[i][PLAYER_LIST_ATTRIBUTE_CARDS], "cards");
	strcat(player_list_att_names[i][PLAYER_LIST_ATTRIBUTE_AGE], "age");
	strcat(player_list_att_names[i][PLAYER_LIST_ATTRIBUTE_ETAL], "etal");
	strcat(player_list_att_names[i][PLAYER_LIST_ATTRIBUTE_VALUE], "value");
	strcat(player_list_att_names[i][PLAYER_LIST_ATTRIBUTE_WAGE], "wage");
	strcat(player_list_att_names[i][PLAYER_LIST_ATTRIBUTE_CONTRACT], "contract");
	strcat(player_list_att_names[i][PLAYER_LIST_ATTRIBUTE_TEAM], "team");
	strcat(player_list_att_names[i][PLAYER_LIST_ATTRIBUTE_LEAGUE_CUP], "league_cup");
    }
}

/** Load the options at the beginning of a new game from
    the configuration file. */
void
file_load_conf_file(void)
{
    gint i, j;
    gchar opt_names[OPT_END][50];
    gchar player_list_attribute_names[2][PLAYER_LIST_ATTRIBUTE_END][50];
    FILE *fil;
    gchar *conf_file = file_find_support_file("bygfoot.conf");
    gchar buf[SMALL];
    gchar buf2[SMALL];
    gchar trash[SMALL];

    file_my_fopen(conf_file, "r", &fil, TRUE);

    g_free(conf_file);

    file_write_opt_names(opt_names, player_list_attribute_names);

    while(file_get_next_line(fil, buf))
    {
	for(i=0;i<OPT_END;i++)
	    if(strncmp(opt_names[i], buf, strlen(opt_names[i])) == 0)
		sscanf(buf, "%[^-0-9 ]%d", trash, &options[i]);

	for(j=0;j<2;j++)
	    for(i=0;i<PLAYER_LIST_ATTRIBUTE_END;i++)
		if(strncmp(player_list_attribute_names[j][i], buf, strlen(player_list_attribute_names[j][i])) == 0)
		    sscanf(buf, "%[^ ]%[ ]%d", trash, buf2,
			   &player_list_attributes[j].on_off[i]);

	if(strncmp(buf, "font_name", strlen("font_name")) == 0)
	{
	    sscanf(buf, "%[font_name ]%[^\n]", trash, buf2);
	    g_string_printf(font_name, "%s", buf2);
	}

	if(strncmp(buf, "debug", strlen("debug")) == 0)
	    sscanf(buf, "%[debug ]%d", trash, &debug);
    }
}

/** Save the current options to the configuration file. */
void
file_save_conf(void)
{
    gint i, j;
    gchar opt_names[OPT_END][50];
    gchar player_list_att_names[2][PLAYER_LIST_ATTRIBUTE_END][50];
    FILE *fil = NULL;
    gchar *conf_file = file_find_support_file("bygfoot.conf");

    if(!file_my_fopen(conf_file, "w", &fil, FALSE))
	return;
    
    g_free(conf_file);

    file_write_opt_names(opt_names, player_list_att_names);

    for(i=0;i<OPT_PENALTY_SHOOTER;i++)
	fprintf(fil, "%s %d\n", opt_names[i], options[i]);

    fprintf(fil, "%s -1\n", opt_names[OPT_PENALTY_SHOOTER]);

    for(j=0;j<2;j++)
    {
	fprintf(fil, "\n");
	for(i=0;i<PLAYER_LIST_ATTRIBUTE_END;i++)
	    fprintf(fil, "%s %d\n", player_list_att_names[j][i],
		    player_list_attributes[j].on_off[i]);
    }

    fprintf(fil, "\n");
    fprintf(fil, "font_name %s\n", font_name->str);
    fprintf(fil, "debug %d\n", debug);

    fclose(fil);
}
