#include "file.h"
#include "free.h"
#include "misc.h"
#include "name.h"
#include "variables.h"
#include "xml_name.h"

#define TAG_NAMES "names"
#define TAG_LAST_NAME "last_name"
#define TAG_FIRST_NAME "first_name"

enum XmlNameStates
{
    STATE_NAMES = 0,
    STATE_LAST_NAME,
    STATE_FIRST_NAME,
    STATE_END
};

/** Keep track of the state.  */
gint state;
/** The name list we read into. */
NameList *nlist;

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
    else if(strcmp(element_name, TAG_FIRST_NAME) == 0)
	state = STATE_FIRST_NAME;
    else if(strcmp(element_name, TAG_LAST_NAME) == 0)
	state = STATE_LAST_NAME;
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
    if(strcmp(element_name, TAG_FIRST_NAME) == 0 ||
       strcmp(element_name, TAG_LAST_NAME) == 0)
       state = STATE_NAMES;
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
    GString *new_name = NULL;

    strncpy(buf, text, text_len);
    buf[text_len] = '\0';

    new_name = g_string_new(buf);

    if(state == STATE_FIRST_NAME)
	g_ptr_array_add(nlist->first_names, new_name);
    else if(state == STATE_LAST_NAME)
	g_ptr_array_add(nlist->last_names, new_name);
}

/** Fill the name list with names from the
    given names file.
    @param sid The sid of the names file we read.
    @param namelist The name list we fill. */
void
xml_name_read(const gchar *sid, NameList *namelist)
{
    gchar *file_name = NULL;
    GMarkupParser parser = {xml_name_read_start_element,
			    xml_name_read_end_element,
			    xml_name_read_text, NULL, NULL};
    GMarkupParseContext *context;
    gchar *file_contents;
    gint length;
    GError *error = NULL;
    gchar buf[SMALL];

    sprintf(buf, "player_names_%s.xml", sid);
    file_name = file_find_support_file(buf, TRUE);

    context = 
	g_markup_parse_context_new(&parser, 0, NULL, NULL);

    if(!g_file_get_contents(file_name, &file_contents, &length, &error))
    {
	g_warning("xml_name_read: error reading file %s\n", file_name);
	misc_print_error(&error, FALSE);
	return;
    }

    free_name_list(namelist, TRUE);
    g_string_assign(namelist->sid, sid);

    nlist = namelist;

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

    g_free(file_name);
}
