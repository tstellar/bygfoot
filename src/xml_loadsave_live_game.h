#ifndef XML_LOADSAVE_LIVE_GAME_H
#define XML_LOADSAVE_LIVE_GAME_H

#include "bygfoot.h"
#include "live_game_struct.h"

void
xml_loadsave_live_game_write_stats(FILE *fil, const LiveGameStats *stats);

void
xml_loadsave_live_game_write_unit(FILE *fil, const LiveGameUnit *unit);

void
xml_loadsave_live_game_write(const gchar *filename, const LiveGame *live_game);

void
xml_loadsave_live_game_read(const gchar *filename, LiveGame *live_game);

void
xml_loadsave_live_game_text         (GMarkupParseContext *context,
				   const gchar         *text,
				   gsize                text_len,  
				   gpointer             user_data,
				     GError             **error);

void
xml_loadsave_live_game_end_element    (GMarkupParseContext *context,
				     const gchar         *element_name,
				     gpointer             user_data,
				       GError             **error);

void
xml_loadsave_live_game_start_element (GMarkupParseContext *context,
				      const gchar         *element_name,
				      const gchar        **attribute_names,
				      const gchar        **attribute_values,
				      gpointer             user_data,
				      GError             **error);

#endif

