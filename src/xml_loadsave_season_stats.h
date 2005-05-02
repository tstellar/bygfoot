#ifndef XML_LOADSAVE_SEASON_STATS_H
#define XML_LOADSAVE_SEASON_STATS_H

#include "bygfoot.h"

void
xml_loadsave_season_stats_write(const gchar *filename);

void
xml_loadsave_season_stats_read(const gchar *dirname, const gchar *prefix);

void
xml_loadsave_season_stats_text         (GMarkupParseContext *context,
					  const gchar         *text,
					  gsize                text_len,  
					  gpointer             user_data,
					  GError             **error);
void
xml_loadsave_season_stats_end_element    (GMarkupParseContext *context,
					    const gchar         *element_name,
					    gpointer             user_data,
					    GError             **error);
void
xml_loadsave_season_stats_start_element (GMarkupParseContext *context,
					   const gchar         *element_name,
					   const gchar        **attribute_names,
					   const gchar        **attribute_values,
					   gpointer             user_data,
					   GError             **error);

#endif

