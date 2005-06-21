#ifndef XML_TEAM_H
#define XML_TEAM_H

#include "bygfoot.h"

void
xml_team_read(Team *tm, const gchar *def_file);

void
xml_team_read_text         (GMarkupParseContext *context,
			   const gchar         *text,
			   gsize                text_len,  
			   gpointer             user_data,
			    GError             **error);

void
xml_team_read_end_element    (GMarkupParseContext *context,
			      const gchar         *element_name,
			      gpointer             user_data,
			      GError             **error);

void
xml_team_read_start_element (GMarkupParseContext *context,
			    const gchar         *element_name,
			    const gchar        **attribute_names,
			    const gchar        **attribute_values,
			    gpointer             user_data,
			     GError             **error);


#endif

