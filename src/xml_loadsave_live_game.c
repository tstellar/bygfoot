#include "cup.h"
#include "file.h"
#include "fixture.h"
#include "league.h"
#include "live_game.h"
#include "misc.h"
#include "variables.h"
#include "xml.h"
#include "xml_loadsave_live_game.h"

enum
{
    TAG_LIVE_GAME = TAG_START_LIVE_GAME,
    TAG_LIVE_GAME_FIX_CLID,
    TAG_LIVE_GAME_FIX_IDX,
    TAG_LIVE_GAME_UNIT,
    TAG_LIVE_GAME_UNIT_POSSESSION,
    TAG_LIVE_GAME_UNIT_AREA,
    TAG_LIVE_GAME_UNIT_MINUTE,
    TAG_LIVE_GAME_UNIT_TIME,
    TAG_LIVE_GAME_UNIT_RESULT,
    TAG_LIVE_GAME_UNIT_EVENT,
    TAG_LIVE_GAME_UNIT_EVENT_TYPE,
    TAG_LIVE_GAME_UNIT_EVENT_VALUE,
    TAG_LIVE_GAME_UNIT_EVENT_COMMENTARY,
    TAG_LIVE_GAME_STAT,
    TAG_LIVE_GAME_STAT_POSSESSION,
    TAG_LIVE_GAME_STAT_VALUES,
    TAG_LIVE_GAME_STAT_VALUE,
    TAG_LIVE_GAME_STAT_PLAYERS,
    TAG_LIVE_GAME_STAT_PLAYER,
    TAG_LIVE_GAME_STAT_PLAYER_ELEMENT,
    TAG_END
};

gint state, unitidx,
    statvalidx, statvalidx2, statplidx, statplidx2,
    fix_clid;
LiveGameUnit new_unit;
LiveGame *lgame;

void
xml_loadsave_live_game_start_element (GMarkupParseContext *context,
				      const gchar         *element_name,
				      const gchar        **attribute_names,
				      const gchar        **attribute_values,
				      gpointer             user_data,
				    GError             **error)
{
    gint i;
    gint tag = xml_get_tag_from_name(element_name);
    gboolean valid_tag = FALSE;

    for(i=TAG_LIVE_GAME;i<TAG_END;i++)
	if(tag == i)
	{
	    state = i;
	    valid_tag = TRUE;
	}

    if(tag == TAG_LIVE_GAME)
	statvalidx = statplidx = 0;
    else if(tag == TAG_LIVE_GAME_UNIT ||
	    tag == TAG_LIVE_GAME_UNIT_EVENT)
	unitidx = 0;
    else if(tag == TAG_LIVE_GAME_STAT_VALUES)
	statvalidx2 = 0;
    else if(tag == TAG_LIVE_GAME_STAT_PLAYERS)
	statplidx2 = 0;    

    if(!valid_tag)
	g_warning("xml_loadsave_live_game_start_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

void
xml_loadsave_live_game_end_element    (GMarkupParseContext *context,
				     const gchar         *element_name,
				     gpointer             user_data,
				     GError             **error)
{
    gint tag = xml_get_tag_from_name(element_name);
    
    if(tag == TAG_LIVE_GAME_FIX_IDX ||
       tag == TAG_LIVE_GAME_FIX_CLID ||
       tag == TAG_LIVE_GAME_UNIT ||
       tag == TAG_LIVE_GAME_STAT)
    {
	state = TAG_LIVE_GAME;
	if(tag == TAG_LIVE_GAME_UNIT)
	    g_array_append_val(lgame->units, new_unit);
    }
    else if(tag == TAG_LIVE_GAME_UNIT_POSSESSION ||
	    tag == TAG_LIVE_GAME_UNIT_AREA ||
	    tag == TAG_LIVE_GAME_UNIT_MINUTE ||
	    tag == TAG_LIVE_GAME_UNIT_TIME ||
	    tag == TAG_LIVE_GAME_UNIT_RESULT ||
	    tag == TAG_LIVE_GAME_UNIT_EVENT)
    {
	state = TAG_LIVE_GAME_UNIT;
	if(tag == TAG_LIVE_GAME_UNIT_RESULT)
	    unitidx++;
    }
    else if(tag == TAG_LIVE_GAME_UNIT_EVENT_TYPE ||
	    tag == TAG_LIVE_GAME_UNIT_EVENT_VALUE ||
	    tag == TAG_LIVE_GAME_UNIT_EVENT_COMMENTARY)
    {
	state = TAG_LIVE_GAME_UNIT_EVENT;
	if(tag == TAG_LIVE_GAME_UNIT_EVENT_VALUE)
	    unitidx++;
    }
    else if(tag == TAG_LIVE_GAME_STAT_POSSESSION ||
	    tag == TAG_LIVE_GAME_STAT_PLAYERS ||
	    tag == TAG_LIVE_GAME_STAT_VALUES)
    {
	state = TAG_LIVE_GAME_STAT;
	if(tag == TAG_LIVE_GAME_STAT_VALUES)
	    statvalidx++;
	else if(tag == TAG_LIVE_GAME_STAT_PLAYERS)
	    statplidx++;
	    
    }
    else if(tag == TAG_LIVE_GAME_STAT_PLAYER)
    {
	state = TAG_LIVE_GAME_STAT_PLAYERS;
	statplidx2++;
    }
    else if(tag == TAG_LIVE_GAME_STAT_VALUE)
    {
	state = TAG_LIVE_GAME_STAT_VALUES;
	statvalidx2++;
    }
    else if(tag == TAG_LIVE_GAME_STAT_PLAYER_ELEMENT)
	state = TAG_LIVE_GAME_STAT_PLAYER;
    else if(tag != TAG_LIVE_GAME)
	g_warning("xml_loadsave_live_game_end_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);	
}

void
xml_loadsave_live_game_text         (GMarkupParseContext *context,
				   const gchar         *text,
				   gsize                text_len,  
				   gpointer             user_data,
				   GError             **error)
{
    gchar buf[SMALL];
    gint int_value = -1;
    GString *new_string = NULL;

    strncpy(buf, text, text_len);
    buf[text_len] = '\0';

    int_value = (gint)g_ascii_strtod(buf, NULL);

    if(state == TAG_LIVE_GAME_FIX_CLID)
	fix_clid = int_value;
    else if(state == TAG_LIVE_GAME_FIX_IDX)
	lgame->fix = &g_array_index(league_cup_get_fixtures(fix_clid),
				    Fixture, int_value);
    else if(state == TAG_LIVE_GAME_UNIT_POSSESSION)
	new_unit.possession = int_value;
    else if(state == TAG_LIVE_GAME_UNIT_AREA)
	new_unit.area = int_value;
    else if(state == TAG_LIVE_GAME_UNIT_MINUTE)
	new_unit.minute = int_value;
    else if(state == TAG_LIVE_GAME_UNIT_TIME)
	new_unit.time = int_value;
    else if(state == TAG_LIVE_GAME_UNIT_RESULT)
	new_unit.result[unitidx] = int_value;
    else if(state == TAG_LIVE_GAME_UNIT_EVENT_TYPE)
	new_unit.event.type = int_value;
    else if(state == TAG_LIVE_GAME_UNIT_EVENT_VALUE)
	new_unit.event.values[unitidx] = int_value;
    else if(state == TAG_LIVE_GAME_UNIT_EVENT_COMMENTARY)
	new_unit.event.commentary = g_string_new(buf);
    else if(state == TAG_LIVE_GAME_STAT_POSSESSION)
	lgame->stats.possession = int_value;
    else if(state == TAG_LIVE_GAME_STAT_VALUE)
	lgame->stats.values[statvalidx][statvalidx2] = int_value;
    else if(state == TAG_LIVE_GAME_STAT_PLAYER_ELEMENT)
    {
	new_string = g_string_new(buf);
	g_ptr_array_add(lgame->stats.players[statplidx][statplidx2],
			new_string);
    }
}

void
xml_loadsave_live_game_read(const gchar *filename, LiveGame *live_game)
{
    GMarkupParser parser = {xml_loadsave_live_game_start_element,
			    xml_loadsave_live_game_end_element,
			    xml_loadsave_live_game_text, NULL, NULL};
    GMarkupParseContext *context;
    gchar *file_contents;
    guint length;
    GError *error = NULL;

    context = 
	g_markup_parse_context_new(&parser, 0, NULL, NULL);

    if(!g_file_get_contents(filename, &file_contents, &length, &error))
    {
	g_warning("xml_loadsave_live_game_read: error reading file %s\n", filename);
	misc_print_error(&error, TRUE);
    }

    lgame = live_game;

    if(g_markup_parse_context_parse(context, file_contents, length, &error))
    {
	g_markup_parse_context_end_parse(context, NULL);
	g_markup_parse_context_free(context);
	g_free(file_contents);
    }
    else
    {
	g_warning("xml_loadsave_live_game_read: error parsing file %s\n", filename);
	misc_print_error(&error, TRUE);
    }
}

void
xml_loadsave_live_game_write(const gchar *filename, const LiveGame *live_game)
{
    gint i;
    FILE *fil = NULL;

    file_my_fopen(filename, "w", &fil, TRUE);

    fprintf(fil, "<_%d>\n", TAG_LIVE_GAME);

    if(live_game->fix != NULL)
    {
	xml_write_int(fil, live_game->fix->clid, TAG_LIVE_GAME_FIX_CLID, I0);
	xml_write_int(fil, fixture_get_index(live_game->fix), TAG_LIVE_GAME_FIX_IDX, I0);
    }

    for(i=0;i<live_game->units->len;i++)
	xml_loadsave_live_game_write_unit(fil,
					  &g_array_index(live_game->units,
							 LiveGameUnit, i));

    xml_loadsave_live_game_write_stats(fil, &live_game->stats);

    fprintf(fil, "</_%d>\n", TAG_LIVE_GAME);

    fclose(fil);
}

void
xml_loadsave_live_game_write_unit(FILE *fil, const LiveGameUnit *unit)
{
    gint i;

    fprintf(fil, "<_%d>\n", TAG_LIVE_GAME_UNIT);

    xml_write_int(fil, unit->possession,
		  TAG_LIVE_GAME_UNIT_POSSESSION, I1);
    xml_write_int(fil, unit->area,
		  TAG_LIVE_GAME_UNIT_AREA, I1);
    xml_write_int(fil, unit->minute,
		  TAG_LIVE_GAME_UNIT_MINUTE, I1);
    xml_write_int(fil, unit->time,
		  TAG_LIVE_GAME_UNIT_TIME, I1);
    xml_write_int(fil, unit->result[0],
		  TAG_LIVE_GAME_UNIT_RESULT, I1);
    xml_write_int(fil, unit->result[1],
		  TAG_LIVE_GAME_UNIT_RESULT, I1);

    fprintf(fil, "%s<_%d>\n", I1, TAG_LIVE_GAME_UNIT_EVENT);
	
    xml_write_int(fil,
		  unit->event.type,
		  TAG_LIVE_GAME_UNIT_EVENT_TYPE, I2);

    xml_write_g_string(fil, unit->event.commentary,
		       TAG_LIVE_GAME_UNIT_EVENT_COMMENTARY, I2);
    for(i=0;i<MAX(LIVE_GAME_EVENT_VALUE1_END, LIVE_GAME_EVENT_VALUE2_END);i++)
	xml_write_int(fil, unit->event.values[i],
		      TAG_LIVE_GAME_UNIT_EVENT_VALUE, I2);

    fprintf(fil, "%s</_%d>\n", I1, TAG_LIVE_GAME_UNIT_EVENT);

    fprintf(fil, "</_%d>\n", TAG_LIVE_GAME_UNIT);
}

void
xml_loadsave_live_game_write_stats(FILE *fil, const LiveGameStats *stats)
{
    gint i, j, k;
    
    fprintf(fil, "<_%d>\n", TAG_LIVE_GAME_STAT);
    
    xml_write_float(fil, stats->possession, 
		    TAG_LIVE_GAME_STAT_POSSESSION, I1);

    for(i=0;i<2;i++)
    {
	fprintf(fil, "%s<_%d>\n", I1, TAG_LIVE_GAME_STAT_VALUES);	
	for(j=0;j<LIVE_GAME_STAT_VALUE_END;j++)
	    xml_write_int(fil, stats->values[i][j], 
			  TAG_LIVE_GAME_STAT_VALUE, I2);
	fprintf(fil, "%s</_%d>\n", I1, TAG_LIVE_GAME_STAT_VALUES);
    }
    
    for(i=0;i<2;i++)
    {
	fprintf(fil, "%s<_%d>\n", I1, TAG_LIVE_GAME_STAT_PLAYERS);
	for(j=0;j<LIVE_GAME_STAT_ARRAY_END;j++)
	{
	    fprintf(fil, "%s<_%d>\n", I1, TAG_LIVE_GAME_STAT_PLAYER);
	    for(k=0;k<stats->players[i][j]->len;k++)
		xml_write_g_string(fil, 
				   (GString*)g_ptr_array_index(stats->players[i][j], k),
				   TAG_LIVE_GAME_STAT_PLAYER_ELEMENT, I2);
	    fprintf(fil, "%s</_%d>\n", I1, TAG_LIVE_GAME_STAT_PLAYER);
	}
	fprintf(fil, "%s</_%d>\n", I1, TAG_LIVE_GAME_STAT_PLAYERS);
    }

    fprintf(fil, "</_%d>\n", TAG_LIVE_GAME_STAT);
}
