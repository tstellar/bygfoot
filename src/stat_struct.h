#ifndef STAT_STRUCT_H
#define STAT_STRUCT_H

/** A statistics element holding some
    string and integer values. */
typedef struct
{
    gint team_id;
    gint value1, value2, value3;
    GString *value_string;
} Stat;

/** A structure holding some stat arrays about a league. */
typedef struct
{
    /** The league id. */
    gint clid;
    /** Best offensive and defensive teams. */
    GArray *teams_off, *teams_def;
    /** Best goal getters and goalies. */
    GArray *player_scorers, *player_goalies;
} LeagueStat;

/** A team name and a competition name. */
typedef struct
{
    GString *team_name, 
	*cl_name;
} ChampStat;

/** A season statistics structure. */
typedef struct
{
    /** Which season */
    gint season_number;

    /** League and cup winners. */
    GArray *league_champs;
    GArray *cup_champs;
    
    /** The league stats at the end of the season. */
    GArray *league_stats;
} SeasonStat;

#endif
