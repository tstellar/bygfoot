#ifndef XML_COUNTRY_H
#define XML_COUNTRY_H

#include "bygfoot.h"
#include "free.h"
#include "gui.h"
#include "league.h"
#include "misc.h"
#include "variables.h"
#include "xml_cup.h"
#include "xml_league.h"

GPtrArray*
xml_country_get_team_names(const gchar *country_name);

void
xml_country_read_start_element (GMarkupParseContext *context,
				const gchar         *element_name,
				const gchar        **attribute_names,
				const gchar        **attribute_values,
				gpointer             user_data,
				GError             **error);
void
xml_country_read_end_element    (GMarkupParseContext *context,
				 const gchar         *element_name,
				 gpointer             user_data,
				 GError             **error);
void
xml_country_read_text         (GMarkupParseContext *context,
			       const gchar         *text,
			       gsize                text_len,  
			       gpointer             user_data,
			       GError             **error);


void
xml_country_read(const gchar *country_name);

#endif
