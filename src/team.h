#ifndef TEAM_H
#define TEAM_H

#include "bygfoot.h"
#include "maths.h"
#include "player.h"

typedef struct _Team Team;

/**
   Structure representing a team.
   @see _Player
*/
struct _Team
{
    GString *name;
    /**
       Numerical id of the league or cup the team belongs to.
    */
    gint league_cup_id;

    
    gint nid, /**< Id of the team. */
	structure, /**< Playing structure. @see team_assign_playing_structure() */
	style; /**< Playing style. @see team_assign_playing_style() */

    /* i'm not sure whether we should not have only one global
       variable 'stadium' representing the stadium of the user's team.
       cpu teams don't really need a stadium */
/*     Stadium stadium; */
    /**
       Array of players.
    */
    GArray *players;
};

Team
team_new(void);

gint
team_assign_playing_structure(void);

gint
team_assign_playing_style(void);

#endif
