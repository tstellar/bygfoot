#include "file.h"
#include "misc.h"
#include "team.h"
#include "xml.h"
#include "xml_loadsave_table.h"

enum
{    
    TAG_TABLE = TAG_START_TABLE,
    TAG_TABLE_ELEMENT,
    TAG_TABLE_ELEMENT_VALUE,
    TAG_END
};

gint state, valueidx, team_clid;
TableElement new_element;
Table *new_table;

void
xml_loadsave_table_start_element (GMarkupParseContext *context,
				    const gchar         *element_name,
				    const gchar        **attribute_names,
				    const gchar        **attribute_values,
				    gpointer             user_data,
				    GError             **error)
{
    gint i;
    gint tag = xml_get_tag_from_name(element_name);
    gboolean valid_tag = FALSE;

    for(i=TAG_TABLE;i<TAG_END;i++)
	if(tag == i)
	{
	    state = i;
	    valid_tag = TRUE;
	}

    for(i=TAG_NAME;i<=TAG_ROUND;i++)
	if(tag == i)
	{
	    state = i;
	    valid_tag = TRUE;
	}
	
    if(tag == TAG_TABLE_ELEMENT)
	valueidx = 0;

    if(!valid_tag)
	g_warning("xml_loadsave_table_start_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

void
xml_loadsave_table_end_element    (GMarkupParseContext *context,
				     const gchar         *element_name,
				     gpointer             user_data,
				     GError             **error)
{
    gint tag = xml_get_tag_from_name(element_name);

    if(tag == TAG_NAME ||
       tag == TAG_ID ||
       tag == TAG_ROUND ||
       tag == TAG_TABLE_ELEMENT)
    {
	state = TAG_TABLE;
	if(tag == TAG_TABLE_ELEMENT)
	    g_array_append_val(new_table->elements, new_element);
    }
    else if(tag == TAG_TABLE_ELEMENT_VALUE ||
	    tag == TAG_TEAM_ID ||
	    tag == TAG_TEAM_CLID)
    {
	state = TAG_TABLE_ELEMENT;
	if(tag == TAG_TABLE_ELEMENT_VALUE)
	    valueidx++;
    }
    else if(tag != TAG_TABLE)
	g_warning("xml_loadsave_table_end_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

void
xml_loadsave_table_text         (GMarkupParseContext *context,
				   const gchar         *text,
				   gsize                text_len,  
				   gpointer             user_data,
				   GError             **error)
{
    gchar buf[SMALL];
    gint int_value = -1;

    strncpy(buf, text, text_len);
    buf[text_len] = '\0';

    int_value = (gint)g_ascii_strtod(buf, NULL);

    if(state == TAG_NAME)
	g_string_printf(new_table->name, "%s", buf);
    else if(state == TAG_ID)
	new_table->clid = int_value;
    else if(state == TAG_ROUND)
	new_table->round = int_value;
    else if(state == TAG_TEAM_CLID)
	team_clid = int_value;
    else if(state == TAG_TEAM_ID)
	new_element.team = team_get_pointer_from_ids(team_clid, int_value);
    else if(state == TAG_TABLE_ELEMENT_VALUE)
	new_element.values[valueidx] = int_value;
}

void
xml_loadsave_table_read(const gchar *filename, Table *table)
{
    GMarkupParser parser = {xml_loadsave_table_start_element,
			    xml_loadsave_table_end_element,
			    xml_loadsave_table_text, NULL, NULL};
    GMarkupParseContext *context;
    gchar *file_contents;
    guint length;
    GError *error = NULL;

    context = 
	g_markup_parse_context_new(&parser, 0, NULL, NULL);

    if(!g_file_get_contents(filename, &file_contents, &length, &error))
    {
	g_warning("xml_loadsave_table_read: error reading file %s\n", filename);
	misc_print_error(&error, TRUE);
    }

    new_table = table;

    if(g_markup_parse_context_parse(context, file_contents, length, &error))
    {
	g_markup_parse_context_end_parse(context, NULL);
	g_markup_parse_context_free(context);
	g_free(file_contents);
    }
    else
    {
	g_warning("xml_loadsave_table_read: error parsing file %s\n", filename);
	misc_print_error(&error, TRUE);
    }
}

void
xml_loadsave_table_write(const gchar *filename, const Table *table)
{
    gint i, j;
    FILE *fil = NULL;

    file_my_fopen(filename, "w", &fil, TRUE);

    fprintf(fil, "<_%d>\n", TAG_TABLE);
    
    xml_write_g_string(fil, table->name, TAG_NAME, I0);
    xml_write_int(fil, table->clid, TAG_ID, I0);
    xml_write_int(fil, table->round, TAG_ROUND, I0);
        
    for(i=0;i<table->elements->len;i++)
    {
	fprintf(fil, "<_%d>\n", TAG_TABLE_ELEMENT);
	
	xml_write_int(fil, g_array_index(table->elements, TableElement, i).team->clid, 
		      TAG_TEAM_CLID, I1);
	xml_write_int(fil, g_array_index(table->elements, TableElement, i).team->id, 
		      TAG_TEAM_ID, I1);

	for(j=0;j<TABLE_END;j++)
	    xml_write_int(fil, g_array_index(table->elements, TableElement, i).values[j], 
			  TAG_TABLE_ELEMENT_VALUE, I1);

	fprintf(fil, "</_%d>\n", TAG_TABLE_ELEMENT);
    }

    fprintf(fil, "</_%d>\n", TAG_TABLE);

    fclose(fil);
}
