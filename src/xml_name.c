#include "file.h"
#include "free.h"
#include "maths.h"
#include "misc.h"
#include "variables.h"
#include "xml_name.h"

#define TAG_NAMES "player_names"
#define TAG_PLAYER_NAME "player_name"
#define TAG_PLAYER_NAME_NAME "name"

enum XmlNameStates
{
    STATE_NAMES = 0,
    STATE_PLAYER_NAME,
    STATE_PLAYER_NAME_NAME,
    STATE_END
};

/** Keep track of the state.  */
gint state;
GPtrArray *temp_array;
GString *new_name;

/** @see xml_league_read_start_element */
void
xml_name_read_start_element (GMarkupParseContext *context,
			     const gchar         *element_name,
			     const gchar        **attribute_names,
			     const gchar        **attribute_values,
			     gpointer             user_data,
			     GError             **error)
{
    if(strcmp(element_name, TAG_NAMES) == 0)
	state = STATE_NAMES;
    else if(strcmp(element_name, TAG_PLAYER_NAME) == 0)
    {
	new_name = g_string_new("");
	state = STATE_PLAYER_NAME;
    }
    else if(strcmp(element_name, TAG_PLAYER_NAME_NAME) == 0)
	state = STATE_PLAYER_NAME_NAME;
    else
	g_warning("xml_name_read_start_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

/** @see xml_league_read_end_element */
void
xml_name_read_end_element    (GMarkupParseContext *context,
			      const gchar         *element_name,
			      gpointer             user_data,
			      GError             **error)
{
    if(strcmp(element_name, TAG_PLAYER_NAME) == 0)
       state = STATE_NAMES;
    else if(strcmp(element_name, TAG_PLAYER_NAME_NAME) == 0)
	state = STATE_PLAYER_NAME;
    else if(strcmp(element_name, TAG_NAMES) != 0)
	g_warning("xml_name_end_start_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

/** @see xml_league_read_text */
void
xml_name_read_text         (GMarkupParseContext *context,
			    const gchar         *text,
			    gsize                text_len,  
			    gpointer             user_data,
			    GError             **error)
{
    gchar buf[text_len + 1];

    strncpy(buf, text, text_len);
    buf[text_len] = '\0';

    if(state == STATE_PLAYER_NAME_NAME)
    {
	g_string_printf(new_name, "%s", buf);
	g_ptr_array_add(temp_array, (gpointer)new_name);
    }
}

/** Fill the player names array with names from the
    player names file. Randomize the order and keep only
    'number_of_names' of them. 
    @param names_file The file the names are read from.
    @param number_of_names Number of player names we keep in memory. 
    If this is -1, we keep all of them. */
void
xml_name_read(const gchar *names_file, gint number_of_names)
{
    gint i;
    gchar *file_name = file_find_support_file(names_file);
    GMarkupParser parser = {xml_name_read_start_element,
			    xml_name_read_end_element,
			    xml_name_read_text, NULL, NULL};
    GMarkupParseContext *context;
    gchar *file_contents;
    gint length;
    GError *error = NULL;
    gchar buf[SMALL];

    context = 
	g_markup_parse_context_new(&parser, 0, NULL, NULL);

    if(!g_file_get_contents(file_name, &file_contents, &length, &error))
    {
	g_warning("xml_name_read: error reading file %s\n", file_name);
	misc_print_error(&error, FALSE);
	return;
    }
    
    state = STATE_NAMES;
    strcpy(buf, file_name);
    g_free(file_name);
    free_g_string_array(&player_names);
    temp_array = g_ptr_array_new();

    if(g_markup_parse_context_parse(context, file_contents, length, &error))
    {
	g_markup_parse_context_end_parse(context, NULL);	
	g_markup_parse_context_free(context);
	g_free(file_contents);
    }
    else
    {
	g_critical("xml_name_read: error parsing file %s\n", buf);
	misc_print_error(&error, TRUE);
    }

    if(number_of_names == -1)
    {
	player_names = temp_array;
	return;
    }

    gint int_array[temp_array->len];
    math_generate_permutation(int_array, 0, temp_array->len - 1);
    player_names = g_ptr_array_new();
    
    for(i=0;i<number_of_names;i++)
    {
	new_name = g_string_new(((GString*)g_ptr_array_index(temp_array, int_array[i]))->str);
	g_ptr_array_add(player_names, (gpointer)new_name);
    }
    
    free_g_string_array(&temp_array);
}
