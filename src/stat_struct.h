#ifndef STAT_STRUCT_H
#define STAT_STRUCT_H

/** A statistics element holding some
    string and integer values. */
typedef struct
{
    Team *tm;
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

#endif
