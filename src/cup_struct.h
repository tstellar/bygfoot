#ifndef CUP_STRUCT_H
#define CUP_STRUCT_H

#include "bygfoot.h"
#include "table_struct.h"

/** Rules for a round of a cup.
    Cups consist of rounds, e.g. the final counts as
    a round or the round robin games. */
typedef struct
{
    /** Whether there are home and away games or only one leg. 
	Default: TRUE. */
    gboolean home_away;
    /** How many times a match gets repeated if there's a draw.
	Default: 0. */
    gint replay;
    /** Whether the matches are on neutral ground.       
	Default: FALSE. */
    gboolean neutral;
    /** How many round robin groups there are. 
	Default: 0 (ie. no round robin). */
    gint round_robin_number_of_groups;
    /** How many teams advance from each group.
	Default: -1. */
    gint round_robin_number_of_advance;
    /** How many teams advance apart from those that
	are specified already (e.g. the first two advance
	and additionally the best 3 from all the groups.
	Default: 0. */
    gint round_robin_number_of_best_advance;
    /** Number of new teams participating in the cup round 
	(ie. teams that get loaded and are not advancing from a previous
	round). */
    gint new_teams;
    /** The number of byes to be awarded for this cup round.
        The default is enough to bring the next round to a power of two. */
    gint byes;
    /** The teams that got loaded for this cup round.
	Mostly this only happens in the first round. */
    GArray *teams;
    /** Pointers to all teams loaded in the cup round; these
	teams get passed to the fixture generation function
	together with the teams advancing from the previous round. */
    GPtrArray *team_ptrs;
    /** Which new teams come into the cup (@see #CupChooseTeam) */
    GArray *choose_teams;
    /** The round robin tables (in case there is a round robin). */
    GArray *tables;    
} CupRound;

/**
   Rules for choosing teams from the league files.
   This could tell us to select the first three teams
   from the league 'Italy 1' to participate in the cup.
*/
typedef struct
{
    /** The string id of the league we choose from.
	Default: "". */
    GString *sid;
    /** The number of teams chosen.
	Default: -1 (ie. all teams are chosen). */
    gint number_of_teams;
    /** The range from which the teams are chosen,
	e.g. 2 and 5 means the teams from 2 to 5 are chosen
	(given that 'number_of_teams' is 4). 
	Defaults: -1 (ie. the teams are chosen from the whole
	range of teams in the league). */
    gint start_idx, end_idx;
    /** Whether the teams are chosen randomly,
	ie. 3 random teams from the range 1-20.
	Default: FALSE. */
    gboolean randomly;
    /** Whether the team is generated and loaded from
	a league file or taken from one of the country's leagues
	or cups. Default: FALSE. */
    gboolean generate;
} CupChooseTeam;

/** Structure representing a cup. */
typedef struct
{
    /** Name and short name of the cup, a pixmap path,
	and the string id (e.g. england_fa or so).
	Default: "". */
    GString *name, *short_name, *symbol, *sid;
    /** Numerical id. */
    gint id;
    /** An integer specifying which cups are mutually exclusive for
	league teams, e.g. the same team can't participate in the UEFA Cup and
	the Champions' League. */
    gint group;
    /** Last week (typically the week the final
	takes place) and weeks between matchdays.
	Default: -1 and 1. */
    gint last_week, week_gap;
    /** This determines when the cup gets added to the acps
	pointer array (and becomes visible for the user). Also determines
	when the cup fixtures for the first round get written.
	Default: 0 (ie. the cup is visible from the first week). */
    gint add_week;
    /** Number of yellow cards that lead to a missed match.
	Default: 1000 (off). */
    gint yellow_red;
    /** Difference of the average skill for the cup teams compared to
	the league with highest average skill.
	Default: 0. */
    gint talent_diff;
    /** The week and week_round at the beginning of which the fixtures
	have to be updated. */
    gint next_fixture_update_week;
    gint next_fixture_update_week_round;

    /** A GString pointer array of properties (like "national"). */
    GPtrArray *properties;
    /** The rounds of the cup.
	@see #CupRound*/
    GArray *rounds;
    /** Pointer array containing teams that got a bye for a round of the cup. */
    GPtrArray *bye;
    /** The teams belonging to the cup (stored in the cup rounds,
	these are only pointers).
	Relevant only if it's an international one. */
    GPtrArray *teams;
    /** Pointer array with the names of all the teams in the cup.
	Also the teams from the country's leagues. */
    GPtrArray *team_names;
    /** The fixtures of a season for the cup. */
    GArray *fixtures;
} Cup;

#endif
