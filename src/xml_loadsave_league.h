#ifndef XML_LOADSAVE_LEAGUE_H
#define XML_LOADSAVE_LEAGUE_H

#include "bygfoot.h"
#include "league_struct.h"

void
xml_loadsave_league_start_element (GMarkupParseContext *context,
				   const gchar         *element_name,
				   const gchar        **attribute_names,
				   const gchar        **attribute_values,
				   gpointer             user_data,
				   GError             **error);

void
xml_loadsave_league_end_element    (GMarkupParseContext *context,
				    const gchar         *element_name,
				    gpointer             user_data,
				    GError             **error);

void
xml_loadsave_league_text         (GMarkupParseContext *context,
				  const gchar         *text,
				  gsize                text_len,  
				  gpointer             user_data,
				  GError             **error);

void
xml_loadsave_league_write(const gchar *prefix, const League *league);

void
xml_loadsave_league_read(const gchar *filename, League *league);

#endif
