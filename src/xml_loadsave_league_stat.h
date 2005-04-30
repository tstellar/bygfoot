#ifndef XML_LOADSAVE_LEAGUE_STAT_H
#define XML_LOADSAVE_LEAGUE_STAT_H

#include "bygfoot.h"
#include "stat_struct.h"

void
xml_loadsave_league_stat_write(const gchar *filename, const LeagueStat *league_stat);

void
xml_loadsave_league_stat_write_stat(FILE *fil, const Stat *stat);

void
xml_loadsave_league_stat_start_element (GMarkupParseContext *context,
					const gchar         *element_name,
					const gchar        **attribute_names,
					const gchar        **attribute_values,
					gpointer             user_data,
					GError             **error);
void
xml_loadsave_league_stat_end_element    (GMarkupParseContext *context,
					 const gchar         *element_name,
					 gpointer             user_data,
					 GError             **error);
void
xml_loadsave_league_stat_text         (GMarkupParseContext *context,
				       const gchar         *text,
				       gsize                text_len,  
				       gpointer             user_data,
				       GError             **error);
void
xml_loadsave_league_stat_read(const gchar *filename, LeagueStat *league_stat);

#endif
