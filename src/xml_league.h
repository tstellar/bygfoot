#ifndef XML_LEAGUE_H
#define XML_LEAGUE_H

#include "bygfoot.h"

void
xml_league_read_start_element (GMarkupParseContext *context,
				const gchar         *element_name,
				const gchar        **attribute_names,
				const gchar        **attribute_values,
				gpointer             user_data,
			       GError             **error);

void
xml_league_read_end_element    (GMarkupParseContext *context,
				 const gchar         *element_name,
				 gpointer             user_data,
				GError             **error);

void
xml_league_read_text         (GMarkupParseContext *context,
			       const gchar         *text,
			       gsize                text_len,  
			       gpointer             user_data,
			      GError             **error);

void
xml_league_read(const gchar *league_name, GArray *leagues);

#endif
