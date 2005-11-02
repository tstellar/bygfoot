/*
   Bygfoot Football Manager -- a small and simple GTK2-based
   football management game.

   http://bygfoot.sourceforge.net

   Copyright (C) 2005  Gyözö Both (gyboth@bygfoot.com)

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "file.h"
#include "free.h"
#include "main.h"
#include "misc.h"
#include "strategy_struct.h"
#include "xml_strategy.h"

#define TAG_STRATEGY "strategy"
#define TAG_STRATEGY_SID "sid"
#define TAG_STRATEGY_DESC "desc"
#define TAG_STRATEGY_PRIORITY "priority"

#define TAG_STRATEGY_PREMATCH "prematch"
#define TAG_STRATEGY_PREMATCH_FORMATION "formation"
#define TAG_STRATEGY_PREMATCH_LINEUP "lineup"
#define TAG_STRATEGY_PREMATCH_BOOST "prematch_boost"
#define TAG_STRATEGY_PREMATCH_STYLE "prematch_style"

#define TAG_STRATEGY_MATCH_ACTION "match_action"
#define TAG_STRATEGY_MATCH_ACTION_BOOST "boost"
#define TAG_STRATEGY_MATCH_ACTION_STYLE "style"
#define TAG_STRATEGY_MATCH_ACTION_SUB "sub"
#define TAG_STRATEGY_MATCH_ACTION_SUB_IN_POS "in"
#define TAG_STRATEGY_MATCH_ACTION_SUB_OUT_POS "out"

#define ATT_NAME_MIN_FITNESS "min_fitness"
#define ATT_NAME_COND "cond"
#define ATT_NAME_SUB_PROPERTY "prop"

#define LINEUP_NAME_BEST "best"
#define LINEUP_NAME_WEAKEST "weakest"
#define LINEUP_NAME_FITTEST "fittest"
#define LINEUP_NAME_UNFITTEST "unfittest"

#define BOOST_NAME_ON "on"
#define BOOST_NAME_OFF "off"
#define BOOST_NAME_ANTI "anti"

#define STYLE_NAME_ALL_OUT_DEFEND "all-out-defend"
#define STYLE_NAME_DEFEND "defend"
#define STYLE_NAME_BALANCED "balanced"
#define STYLE_NAME_ATTACK "attack"
#define STYLE_NAME_ALL_OUT_ATTACK "all-out-attack"

#define POS_NAME_GOALIE "goalie"
#define POS_NAME_DEFENDER "defender"
#define POS_NAME_MIDFIELDER "midfielder"
#define POS_NAME_FORWARD "forward"

enum
{
    STATE_STRATEGY = 0,
    STATE_STRATEGY_SID,
    STATE_STRATEGY_DESC,
    STATE_STRATEGY_PRIORITY,
    STATE_STRATEGY_PREMATCH,
    STATE_STRATEGY_PREMATCH_FORMATION,
    STATE_STRATEGY_PREMATCH_LINEUP,
    STATE_STRATEGY_PREMATCH_BOOST,
    STATE_STRATEGY_PREMATCH_STYLE,
    STATE_STRATEGY_MATCH_ACTION,
    STATE_STRATEGY_MATCH_ACTION_BOOST,
    STATE_STRATEGY_MATCH_ACTION_STYLE,
    STATE_STRATEGY_MATCH_ACTION_SUB,
    STATE_STRATEGY_MATCH_ACTION_SUB_IN_POS,
    STATE_STRATEGY_MATCH_ACTION_SUB_OUT_POS,
};

gint state, action_id;

#define curstrat g_array_index(strategies, Strategy, strategies->len - 1)
#define curprematch g_array_index(curstrat.prematch, StrategyPrematch, curstrat.prematch->len - 1)
#define curmatchaction g_array_index(curstrat.match_action, StrategyMatchAction, curstrat.match_action->len - 1)

void
xml_strategy_read_start_element (GMarkupParseContext *context,
				 const gchar         *element_name,
				 const gchar        **attribute_names,
				 const gchar        **attribute_values,
				 gpointer             user_data,
				 GError             **error)
{
    gint atidx = 0;

    if(strcmp(element_name, TAG_STRATEGY) == 0)
	state = STATE_STRATEGY;
    else if(strcmp(element_name, TAG_STRATEGY_SID) == 0)
	state = STATE_STRATEGY_SID;
    else if(strcmp(element_name, TAG_STRATEGY_DESC) == 0)
	state = STATE_STRATEGY_DESC;
    else if(strcmp(element_name, TAG_STRATEGY_PRIORITY) == 0)
	state = STATE_STRATEGY_PRIORITY;
    else if(strcmp(element_name, TAG_STRATEGY_PREMATCH) == 0)
    {
	StrategyPrematch new_prematch;

	state = STATE_STRATEGY_PREMATCH;
	
	new_prematch.condition = NULL;
	new_prematch.lineup = new_prematch.boost = 
	    new_prematch.style = -100;
	new_prematch.min_fitness = 0;
	new_prematch.formations = g_array_new(FALSE, FALSE, sizeof(gint));

	while(attribute_names[atidx] != NULL)
	{
	    if(strcmp(attribute_names[atidx], ATT_NAME_COND) == 0 &&
		new_prematch.condition == NULL)
		new_prematch.condition = 
		    g_strdup(attribute_values[atidx]);
	    else
		g_warning("xml_strategy_read_start_element: unknown attribute %s\n",
			  attribute_names[atidx]);

	    atidx++;
	}

	g_array_append_val(curstrat.prematch, new_prematch);
    }
    else if(strcmp(element_name, TAG_STRATEGY_PREMATCH_FORMATION) == 0)
	state = STATE_STRATEGY_PREMATCH_FORMATION;
    else if(strcmp(element_name, TAG_STRATEGY_PREMATCH_LINEUP) == 0)
    {
	state = STATE_STRATEGY_PREMATCH_LINEUP;

	while(attribute_names[atidx] != NULL)
	{
	    if(strcmp(attribute_names[atidx], ATT_NAME_MIN_FITNESS) == 0)
		curprematch.min_fitness =
		    g_ascii_strtod(attribute_values[atidx], NULL) / 100;
	    else
		g_warning("xml_strategy_read_start_element: unknown attribute %s\n",
			  attribute_names[atidx]);

	    atidx++;
	}
    }
    else if(strcmp(element_name, TAG_STRATEGY_PREMATCH_BOOST) == 0)
	state = STATE_STRATEGY_PREMATCH_BOOST;
    else if(strcmp(element_name, TAG_STRATEGY_PREMATCH_STYLE) == 0)
	state = STATE_STRATEGY_PREMATCH_STYLE;
    else if(strcmp(element_name, TAG_STRATEGY_MATCH_ACTION) == 0)
    {
	StrategyMatchAction new_match_action;

	state = STATE_STRATEGY_MATCH_ACTION;
	new_match_action.sub_condition = NULL;
	
	new_match_action.condition = NULL;
	new_match_action.boost = 
	    new_match_action.style = -100;
	new_match_action.sub_in_pos = -1;
	new_match_action.id = action_id++;

	while(attribute_names[atidx] != NULL)
	{
	    if(strcmp(attribute_names[atidx], ATT_NAME_COND) == 0 &&
	       new_match_action.condition == NULL)
		new_match_action.condition = 
		    g_strdup(attribute_values[atidx]);
	    else
		g_warning("xml_strategy_read_start_element: unknown attribute %s\n",
			  attribute_names[atidx]);

	    atidx++;
	}

	g_array_append_val(curstrat.match_action, new_match_action);
    }
    else if(strcmp(element_name, TAG_STRATEGY_MATCH_ACTION_BOOST) == 0)
	state = STATE_STRATEGY_MATCH_ACTION_BOOST;
    else if(strcmp(element_name, TAG_STRATEGY_MATCH_ACTION_STYLE) == 0)
	state = STATE_STRATEGY_MATCH_ACTION_STYLE;
    else if(strcmp(element_name, TAG_STRATEGY_MATCH_ACTION_SUB) == 0)
    {
	state = STATE_STRATEGY_MATCH_ACTION_SUB;

	while(attribute_names[atidx] != NULL)
	{
	    if(strcmp(attribute_names[atidx], ATT_NAME_COND) == 0 &&
	       curmatchaction.sub_condition == NULL)
		curmatchaction.sub_condition = 
		    g_strdup(attribute_values[atidx]);
	    else
		g_warning("xml_strategy_read_start_element: unknown attribute %s\n",
			  attribute_names[atidx]);

	    atidx++;
	}
    }
    else if(strcmp(element_name, TAG_STRATEGY_MATCH_ACTION_SUB_IN_POS) == 0)
    {
	state = STATE_STRATEGY_MATCH_ACTION_SUB_IN_POS;

	while(attribute_names[atidx] != NULL)
	{
	    if(strcmp(attribute_names[atidx], ATT_NAME_SUB_PROPERTY) == 0)
	    {
		if(strcmp(attribute_values[atidx], LINEUP_NAME_BEST) == 0)
		    curmatchaction.sub_in_prop = STRAT_LINEUP_BEST;
		else if(strcmp(attribute_values[atidx], LINEUP_NAME_WEAKEST) == 0)
		    curmatchaction.sub_in_prop = STRAT_LINEUP_WEAKEST;
		else if(strcmp(attribute_values[atidx], LINEUP_NAME_FITTEST) == 0)
		    curmatchaction.sub_in_prop = STRAT_LINEUP_FITTEST;
		else if(strcmp(attribute_values[atidx], LINEUP_NAME_UNFITTEST) == 0)
		    curmatchaction.sub_in_prop = STRAT_LINEUP_UNFITTEST;
		else
		    g_warning("xml_strategy_read_start_element: unknown property value %s\n",
			      attribute_values[atidx]);
	    }
	    else
		g_warning("xml_strategy_read_start_element: unknown attribute %s\n",
			  attribute_names[atidx]);

	    atidx++;
	}
    }
    else if(strcmp(element_name, TAG_STRATEGY_MATCH_ACTION_SUB_OUT_POS) == 0)
    {
	state = STATE_STRATEGY_MATCH_ACTION_SUB_OUT_POS;

	while(attribute_names[atidx] != NULL)
	{
	    if(strcmp(attribute_names[atidx], ATT_NAME_SUB_PROPERTY) == 0)
	    {
		if(strcmp(attribute_values[atidx], LINEUP_NAME_BEST) == 0)
		    curmatchaction.sub_out_prop = STRAT_LINEUP_BEST;
		else if(strcmp(attribute_values[atidx], LINEUP_NAME_WEAKEST) == 0)
		    curmatchaction.sub_out_prop = STRAT_LINEUP_WEAKEST;
		else if(strcmp(attribute_values[atidx], LINEUP_NAME_FITTEST) == 0)
		    curmatchaction.sub_out_prop = STRAT_LINEUP_FITTEST;
		else if(strcmp(attribute_values[atidx], LINEUP_NAME_UNFITTEST) == 0)
		    curmatchaction.sub_out_prop = STRAT_LINEUP_UNFITTEST;
		else
		    g_warning("xml_strategy_read_start_element: unknown property value %s\n",
			      attribute_values[atidx]);
	    }
	    else
		g_warning("xml_strategy_read_start_element: unknown attribute %s\n",
			  attribute_names[atidx]);

	    atidx++;
	}
    }
    else
	g_warning("xml_strategy_read_start_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

/**
 * The function called by the parser when a closing tag is read.
 * The state variable is changed in this function.
 * @see The GLib manual (Simple XML parser).
 */
void
xml_strategy_read_end_element    (GMarkupParseContext *context,
				  const gchar         *element_name,
				  gpointer             user_data,
				  GError             **error)
{
    if(strcmp(element_name, TAG_STRATEGY_SID) == 0 ||
       strcmp(element_name, TAG_STRATEGY_DESC) == 0 ||
       strcmp(element_name, TAG_STRATEGY_PRIORITY) == 0 ||
       strcmp(element_name, TAG_STRATEGY_MATCH_ACTION) == 0 ||
       strcmp(element_name, TAG_STRATEGY_PREMATCH) == 0)
	state = STATE_STRATEGY;
    else if(strcmp(element_name, TAG_STRATEGY_PREMATCH_FORMATION) == 0 ||
	    strcmp(element_name, TAG_STRATEGY_PREMATCH_LINEUP) == 0 ||
	    strcmp(element_name, TAG_STRATEGY_PREMATCH_BOOST) == 0 ||
	    strcmp(element_name, TAG_STRATEGY_PREMATCH_STYLE) == 0)
	state = STATE_STRATEGY_PREMATCH;
    else if(strcmp(element_name, TAG_STRATEGY_MATCH_ACTION_SUB) == 0 ||
	    strcmp(element_name, TAG_STRATEGY_MATCH_ACTION_STYLE) == 0 ||
	    strcmp(element_name, TAG_STRATEGY_MATCH_ACTION_BOOST) == 0)
	state = STATE_STRATEGY_MATCH_ACTION;
    else if(strcmp(element_name, TAG_STRATEGY_MATCH_ACTION_SUB_IN_POS) == 0 ||
	    strcmp(element_name, TAG_STRATEGY_MATCH_ACTION_SUB_OUT_POS) == 0)
	state = STATE_STRATEGY_MATCH_ACTION_SUB;
    else if(strcmp(element_name, TAG_STRATEGY) != 0)
	g_warning("xml_strategy_read_end_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

/**
 * The function called by the parser when the text between tags is read.
 * This function is responsible for filling in the variables (e.g. team names)
 * when a file gets loaded.
 * @see The GLib manual (Simple XML parser).
 */
void
xml_strategy_read_text         (GMarkupParseContext *context,
				const gchar         *text,
				gsize                text_len,  
				gpointer             user_data,
				GError             **error)
{
    gchar buf[text_len + 1];
    gint int_value;

    strncpy(buf, text, text_len);
    buf[text_len] = '\0';

    int_value = (gint)g_ascii_strtod(buf, NULL);

    if(state == STATE_STRATEGY_SID)
	misc_string_assign(&curstrat.sid, buf);
    else if(state == STATE_STRATEGY_DESC)
	misc_string_assign(&curstrat.desc, buf);
    else if(state == STATE_STRATEGY_PRIORITY)
    {
	if(strategies->len == 1)
	    curstrat.priority = int_value;
	else
	    curstrat.priority = 
		g_array_index(strategies, Strategy, strategies->len - 2).priority + 
		int_value;
    }
    else if(state == STATE_STRATEGY_PREMATCH_FORMATION)
	g_array_append_val(curprematch.formations, int_value);
    else if(state == STATE_STRATEGY_PREMATCH_LINEUP)
    {
	if(strcmp(buf, LINEUP_NAME_BEST) == 0)
	    curprematch.lineup = STRAT_LINEUP_BEST;
	else if(strcmp(buf, LINEUP_NAME_WEAKEST) == 0)
	    curprematch.lineup = STRAT_LINEUP_WEAKEST;
	else if(strcmp(buf, LINEUP_NAME_FITTEST) == 0)
	    curprematch.lineup = STRAT_LINEUP_FITTEST;
	else
	    g_warning(
		"xml_strategy_read_text: unknown lineup type %s\n", buf);
    }
    else if(state == STATE_STRATEGY_PREMATCH_BOOST)
    {
	if(strcmp(buf, BOOST_NAME_ON) == 0)
	    curprematch.boost = 1;
	else if(strcmp(buf, BOOST_NAME_OFF) == 0)
	    curprematch.boost = 0;
	else if(strcmp(buf, BOOST_NAME_ANTI) == 0)
	    curprematch.boost = -1;
	else
	    g_warning(
		"xml_strategy_read_text: unknown boost type %s\n", buf);
    }
    else if(state == STATE_STRATEGY_PREMATCH_STYLE)
    {
	if(strcmp(buf, STYLE_NAME_ALL_OUT_DEFEND) == 0)
	    curprematch.style = -2;
	else if(strcmp(buf, STYLE_NAME_DEFEND) == 0)
	    curprematch.style = -1;
	else if(strcmp(buf, STYLE_NAME_BALANCED) == 0)
	    curprematch.style = 0;
	else if(strcmp(buf, STYLE_NAME_ATTACK) == 0)
	    curprematch.style = 1;
	else if(strcmp(buf, STYLE_NAME_ALL_OUT_ATTACK) == 0)
	    curprematch.style = 2;
	else
	    g_warning(
		"xml_strategy_read_text: unknown style type %s\n", buf);
    }
    else if(state == STATE_STRATEGY_MATCH_ACTION_STYLE)
    {
	if(strcmp(buf, STYLE_NAME_ALL_OUT_DEFEND) == 0)
	    curmatchaction.style = -2;
	else if(strcmp(buf, STYLE_NAME_DEFEND) == 0)
	    curmatchaction.style = -1;
	else if(strcmp(buf, STYLE_NAME_BALANCED) == 0)
	    curmatchaction.style = 0;
	else if(strcmp(buf, STYLE_NAME_ATTACK) == 0)
	    curmatchaction.style = 1;
	else if(strcmp(buf, STYLE_NAME_ALL_OUT_ATTACK) == 0)
	    curmatchaction.style = 2;
	else
	    g_warning(
		"xml_strategy_read_text: unknown style type %s\n", buf);
    }
    else if(state == STATE_STRATEGY_MATCH_ACTION_BOOST)
    {
	if(strcmp(buf, BOOST_NAME_ON) == 0)
	    curmatchaction.boost = 1;
	else if(strcmp(buf, BOOST_NAME_OFF) == 0)
	    curmatchaction.boost = 0;
	else if(strcmp(buf, BOOST_NAME_ANTI) == 0)
	    curmatchaction.boost = -1;
	else
	    g_warning(
		"xml_strategy_read_text: unknown boost type %s\n", buf);
    }
    else if(state == STATE_STRATEGY_MATCH_ACTION_SUB_IN_POS ||
	    state == STATE_STRATEGY_MATCH_ACTION_SUB_OUT_POS)
    {
	GPtrArray *positions = misc_separate_strings(buf);
	gint i, *pos;

	if(state == STATE_STRATEGY_MATCH_ACTION_SUB_IN_POS)
	{
	    if(positions->len > 1)
	    {
		free_gchar_array(&positions);
		main_exit_program(EXIT_STRATEGY_ERROR, 
				  "xml_strategy_read_text: too many sub_in positions: %s\n",
				  buf);
	    }

	    pos = &curmatchaction.sub_in_pos;
	}
	else
	    pos = &curmatchaction.sub_out_pos;

	*pos = 9;
	
	for(i=0;i<positions->len;i++)
	    if(strcmp((gchar*)g_ptr_array_index(positions, i), 
		      POS_NAME_GOALIE) == 0)
		*pos *= 10;
	    else if(strcmp((gchar*)g_ptr_array_index(positions, i), 
			   POS_NAME_DEFENDER) == 0)
		*pos = (*pos * 10) + 1;
	    else if(strcmp((gchar*)g_ptr_array_index(positions, i),
			   POS_NAME_MIDFIELDER) == 0)
		*pos = (*pos * 10) + 2;
	    else if(strcmp((gchar*)g_ptr_array_index(positions, i), 
			   POS_NAME_FORWARD) == 0)
		*pos = (*pos * 10) + 3;
	    else
		g_warning(
		    "xml_strategy_read_text: unknown position %s\n", 
		    (gchar*)g_ptr_array_index(positions, i));

	if(*pos < 100)
	    *pos = *pos % 10;

	free_gchar_array(&positions);
    }
}

/** Add the strategy described in the file
    to the strategies array. */
void
xml_strategy_read(const gchar *filename)
{
    gint i;
    Strategy new_strat;
    GMarkupParser parser = {xml_strategy_read_start_element,
			    xml_strategy_read_end_element,
			    xml_strategy_read_text, NULL, NULL};
    GMarkupParseContext *context;
    gchar *file_contents;
    gint length;
    GError *error = NULL;

    context = 
	g_markup_parse_context_new(&parser, 0, NULL, NULL);

    if(!g_file_get_contents(filename, &file_contents, &length, &error))
    {
	g_warning("xml_strategy_read: error reading file %s\n", filename);
	misc_print_error(&error, TRUE);
	return;
    }

    new_strat.sid = new_strat.desc = NULL;
    new_strat.priority = 1;
    new_strat.prematch = g_array_new(FALSE, FALSE, sizeof(StrategyPrematch));
    new_strat.match_action = g_array_new(FALSE, FALSE, sizeof(StrategyMatchAction));

    g_array_append_val(strategies, new_strat);

    action_id = 0;

    if(g_markup_parse_context_parse(context, file_contents, length, &error))
    {
	g_markup_parse_context_end_parse(context, NULL);	
	g_markup_parse_context_free(context);
	g_free(file_contents);
    }
    else
    {
	g_critical("xml_strategy_read: error parsing file %s\n", filename);
	misc_print_error(&error, TRUE);
    }


    for(i=0;i<curstrat.match_action->len;i++)
    {
	if(g_array_index(curstrat.match_action, StrategyMatchAction, i).condition == NULL)
	    main_exit_program(EXIT_STRATEGY_ERROR, "xml_strategy_read: unconditional match action encountered in file %s. match actions MUST have conditions.", filename);

/* 	/\*d*\/ */
/* 	printf("bo %d st %d subinpos %d subinprop %d suboutpos %d suboutprop %d\n",  */
/* 	       g_array_index(curstrat.match_action, StrategyMatchAction, i).boost, */
/* 	       g_array_index(curstrat.match_action, StrategyMatchAction, i).style, */
/* 	       g_array_index(curstrat.match_action, StrategyMatchAction, i).sub_in_pos, */
/* 	       g_array_index(curstrat.match_action, StrategyMatchAction, i).sub_in_prop, */
/* 	       g_array_index(curstrat.match_action, StrategyMatchAction, i).sub_out_pos, */
/* 	       g_array_index(curstrat.match_action, StrategyMatchAction, i).sub_out_prop); */
    }
}

/** Load all strategy files found in the appropriate folder. */
void
xml_strategy_load_strategies(void)
{
    gint i, j, k;
    const gchar *strategydir = file_get_first_support_dir_suffix("strategy");
    GPtrArray *files = NULL;
    gchar buf[SMALL];
  
    if(strategydir == NULL)
	main_exit_program(EXIT_STRATEGY_ERROR, 
			  "xml_strategy_load_strategies: strategy directory not found.");

    files = file_dir_get_contents(strategydir, "strategy_", ".xml");

    if(files->len == 0)
    {
	g_ptr_array_free(files, TRUE);
	main_exit_program(EXIT_STRATEGY_ERROR, 
			  "xml_strategy_load_strategies: no CPU strategies found.");
    }

    for(i=0;i<files->len;i++)
    {
	sprintf(buf, "%s%s%s", strategydir, G_DIR_SEPARATOR_S,
		(const gchar*)g_ptr_array_index(files, i));
	xml_strategy_read(buf);
    }

    free_gchar_array(&files);

    /* Fill secondary prematches with default values. */
    for(i=0;i<strategies->len;i++)
    {
	StrategyPrematch *prem0 = &g_array_index(
	    g_array_index(strategies, Strategy, i).prematch,
	    StrategyPrematch, 0),
	    *premcur = NULL;

	if(prem0->formations->len == 0)
	    main_exit_program(EXIT_STRATEGY_ERROR,
			      "xml_strategy_read (%s): The primary prematch should have at least one formation.",
			      g_array_index(strategies, Strategy, i).sid);

	for(j=1;j<g_array_index(strategies, Strategy, i).prematch->len;j++)
	{
	    premcur = &g_array_index(
		g_array_index(strategies, Strategy, i).prematch,
		StrategyPrematch, j);

	    if(premcur->formations->len == 0)
		for(k=0;k<prem0->formations->len;k++)
		    g_array_append_val(premcur->formations,
				       g_array_index(prem0->formations, gint, k));
								       
	    
	    if(premcur->boost == -100)
		premcur->boost = prem0->boost;
	    if(premcur->style == -100)
		premcur->style = prem0->style;
	    if(premcur->lineup == -100)
		premcur->lineup = prem0->lineup;
	    if(premcur->min_fitness == -100)
		premcur->min_fitness = prem0->min_fitness;
	}
    }
}
