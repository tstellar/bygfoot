#ifndef FIXTURE_H
#define FIXTURE_H

#include "bygfoot.h"

/** Possible goal types. */
enum GoalType
{
    GOAL_TYPE_REGULAR= 0,
    GOAL_TYPE_OWN,
    GOAL_TYPE_PENALTY,
    GOAL_TYPE_PENALTY_MISSED,
    GOAL_TYPE_PENALTY_SAVE,
    GOAL_TYPE_PENALTY_POST,
    GOAL_TYPE_PENALTY_CROSS,
    GOAL_TYPE_END
};

/** Possible goal times. */
enum GoalTime
{
    /** In regulation. */
    GOAL_TIME_REGULATION = 0,
    /** In extra time. */
    GOAL_TIME_EXTRA,
    /** In penalty shoot-out. */
    GOAL_TIME_PENALTY,
    GOAL_TIME_END
};

/** Structure representing a goal. */
typedef struct
{    
    gint minute, /**< The minute it happened. */
	team_number, /**< The team that scored (0 or 1). */
	time, /**< Whether it was in regulation, extra time or penalties. */
	type; /** What kind of goal it was (e.g. penalty). */

    /** Name of the scorer. */
    GString *scorer_name;
} Goal;

/** Structure representing a fixture, or, in other words,
    a match. */
typedef struct
{
    /** The cup or league the fixture belongs to. */
    gint clid;
    /** The round (in a cup) the fixture belongs to. */
    gint round;    
    /** When it takes place. */
    gint week_number, week_round_number;
    /** The teams involved. */
    gint team_clid[2], team_id[2];
    /** The number of goals for each team in
	regulation, extra time and penalty shoot-out. */
    gint result[2][3];
    /** Whether there's home advantage, this is second leg,
	or the game has to be decided. */
    gboolean home_advantage, second_leg, decisive;
    /** How many people attended and whether there were
	special events. */
    gint attendance, stadium_event;
    /** The goals that were scored. */
    GArray *goals;
} Fixture;

#endif
