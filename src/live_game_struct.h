#ifndef LIVE_GAME_STRUCT_H
#define LIVE_GAME_STRUCT_H

#include "bygfoot.h"
#include "fixture_struct.h"

/** Events happening during a live game.
    @see #LiveGameEvent
    @see #LiveGameUnit */
enum LiveGameEventType
{
    /** This is the 'main' event, nothing in
	particular is happening; one of the teams
	is in possession of the ball. */
    LIVE_GAME_EVENT_GENERAL = 0,
    LIVE_GAME_EVENT_START_MATCH,
    LIVE_GAME_EVENT_HALF_TIME,
    LIVE_GAME_EVENT_EXTRA_TIME,
    LIVE_GAME_EVENT_END_MATCH,
    LIVE_GAME_EVENT_LOST_POSSESSION,
    LIVE_GAME_EVENT_SCORING_CHANCE,
    LIVE_GAME_EVENT_HEADER,
    LIVE_GAME_EVENT_PENALTY,
    LIVE_GAME_EVENT_FREE_KICK,
    LIVE_GAME_EVENT_GOAL,
    LIVE_GAME_EVENT_OWN_GOAL,
    LIVE_GAME_EVENT_POST,
    LIVE_GAME_EVENT_MISSED,
    LIVE_GAME_EVENT_SAVE,
    LIVE_GAME_EVENT_CROSS_BAR,
    LIVE_GAME_EVENT_FOUL,
    LIVE_GAME_EVENT_FOUL_YELLOW,
    LIVE_GAME_EVENT_FOUL_RED,
    LIVE_GAME_EVENT_FOUL_RED_INJURY,
    LIVE_GAME_EVENT_SEND_OFF,
    LIVE_GAME_EVENT_INJURY,
    /** An injury that permits the player to
	continue after some brief time. */
    LIVE_GAME_EVENT_TEMP_INJURY,
    LIVE_GAME_EVENT_PENALTIES,
    LIVE_GAME_EVENT_STADIUM,
    LIVE_GAME_EVENT_STADIUM_BREAKDOWN,
    LIVE_GAME_EVENT_STADIUM_RIOTS,
    LIVE_GAME_EVENT_STADIUM_FIRE,
    LIVE_GAME_EVENT_SUBSTITUTION,
    LIVE_GAME_EVENT_STRUCTURE_CHANGE,
    LIVE_GAME_EVENT_STYLE_CHANGE_ALL_OUT_DEFEND,
    LIVE_GAME_EVENT_STYLE_CHANGE_DEFEND,
    LIVE_GAME_EVENT_STYLE_CHANGE_BALANCED,
    LIVE_GAME_EVENT_STYLE_CHANGE_ATTACK,
    LIVE_GAME_EVENT_STYLE_CHANGE_ALL_OUT_ATTACK,
    LIVE_GAME_EVENT_BOOST_CHANGE_ANTI,
    LIVE_GAME_EVENT_BOOST_CHANGE_OFF,
    LIVE_GAME_EVENT_BOOST_CHANGE_ON,
    LIVE_GAME_EVENT_END
};

enum LiveGameUnitArea
{
    LIVE_GAME_UNIT_AREA_DEFEND = 0,
    LIVE_GAME_UNIT_AREA_MIDFIELD,
    LIVE_GAME_UNIT_AREA_ATTACK,
    LIVE_GAME_UNIT_AREA_END
};

/** Indices for the time variable of th
    #LiveGameUnit struct.  */
enum LiveGameUnitTime
{
    LIVE_GAME_UNIT_TIME_FIRST_HALF = 0,
    LIVE_GAME_UNIT_TIME_SECOND_HALF,
    LIVE_GAME_UNIT_TIME_EXTRA_TIME,
    LIVE_GAME_UNIT_TIME_PENALTIES,
    LIVE_GAME_UNIT_TIME_END
};

/** Indices for the values in #LiveGameStats. */
enum LiveGameStatValue
{
    LIVE_GAME_STAT_VALUE_GOALS_REGULAR = 0,
    LIVE_GAME_STAT_VALUE_SHOTS,
    LIVE_GAME_STAT_VALUE_SHOT_PERCENTAGE,
    LIVE_GAME_STAT_VALUE_POSSESSION,
    LIVE_GAME_STAT_VALUE_PENALTIES,
    LIVE_GAME_STAT_VALUE_FOULS,
    LIVE_GAME_STAT_VALUE_CARDS,
    LIVE_GAME_STAT_VALUE_REDS,
    LIVE_GAME_STAT_VALUE_INJURIES,
    LIVE_GAME_STAT_VALUE_END
};

enum LiveGameStatArray
{
    LIVE_GAME_STAT_ARRAY_SCORERS = 0,
    LIVE_GAME_STAT_ARRAY_YELLOWS,
    LIVE_GAME_STAT_ARRAY_REDS,
    LIVE_GAME_STAT_ARRAY_INJURED,
    LIVE_GAME_STAT_ARRAY_END
};

/** Indices for the team_value array.
    @see game_get_values() */
enum GameTeamValue
{
    GAME_TEAM_VALUE_DEFEND = 0,
    GAME_TEAM_VALUE_MIDFIELD,
    GAME_TEAM_VALUE_ATTACK,
    GAME_TEAM_VALUE_GOALIE,
    GAME_TEAM_VALUE_END
};

/** Some stats for a live game like ball possession,
    shots on goal etc. */
typedef struct
{
    gfloat possession;
    gint values[2][LIVE_GAME_STAT_VALUE_END];
    GPtrArray *players[2][LIVE_GAME_STAT_ARRAY_END];
} LiveGameStats;

/** A struct telling us what's happening at
    a certain moment in a game. */
typedef struct
{
    /** @see #LiveGameEventType */
    gint type;
    /** Verbosity value. The lower the more important
	the event. */
    gint verbosity;
    /** Information about a team and two players
	involved in the event. */
    gint team, player, player2;
    /** The commentary for the event. */
    GString *commentary;

} LiveGameEvent;

/** A struct representing a fraction of a live game. */
typedef struct
{   
    /** Tells us which of the teams is in possession
	of the ball. */
    gint possession;
    /** The area of the pitch the ball is currently in. */
    gint area;
    /** Which minute of the game and which part of
	the game. If 'minute' is -1 we have an event
	like a substitution that doesn't count as a 
	match time consuming event. @see #GameUnitTime */
    gint minute, time;
    
    /** The match result at the time of this unit. */
    gint result[2];

    /** The event belonging to the game unit. */
    LiveGameEvent event;

} LiveGameUnit;

/** A structure storing team settings during a live game
    pause (so that we know what users have changed in pauses. */
typedef struct
{
    gint structure, style;
    gboolean boost;
    gint player_ids[11];

} LiveGameTeamState;

typedef struct
{
    /** The fixture that belongs to the game. */
    Fixture *fix;
    /** Integer determining the fixture (needed because fixture pointers
	can change when new fixtures are added to the fixtures array). */
    gint fix_id;
    /** Substitutions left for the teams. */
    gint subs_left[2];
    /** The team that started the game, 0 or 1. */
    gint started_game;
    /** We keep track of the stadium events because
	there shouldn't be more than one of them in a game. */
    gint stadium_event;
    /** Attacking, defending etc. values for both teams.
	@see #GameTeamValue */
    gfloat team_values[2][GAME_TEAM_VALUE_END];
    /** The home advantage factor. */
    gfloat home_advantage;
    /** The array of units. @see #GameUnit */
    GArray *units;    
    /** Match statistics. @see #LiveGameStats */
    LiveGameStats stats;
    LiveGameTeamState team_state[2];
} LiveGame;

#endif
