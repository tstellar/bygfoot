#include "cup.h"
#include "free.h"
#include "league.h"
#include "maths.h"
#include "player.h"
#include "team.h"
#include "variables.h"

/**
   Constants determining the probabilities for
   the playing styles of CPU teams.
   @see team_assign_playing_style().
*/
#define CONSTANT_TEAM_PLAYING_STYLE_PROB1 0.1
#define CONSTANT_TEAM_PLAYING_STYLE_PROB2 0.25
#define CONSTANT_TEAM_PLAYING_STYLE_PROB3 0.75
#define CONSTANT_TEAM_PLAYING_STYLE_PROB4 0.9

/**
   Constants determining the probabilities for
   the playing structures of CPU teams.
   @see team_assign_playing_structure().
*/
#define CONSTANT_TEAM_PLAYING_STRUCTURE_PROB1 0.15
#define CONSTANT_TEAM_PLAYING_STRUCTURE_PROB2 0.5
#define CONSTANT_TEAM_PLAYING_STRUCTURE_PROB3 0.7
#define CONSTANT_TEAM_PLAYING_STRUCTURE_PROB4 0.85

/** Kinda hard to explain. 
    @see team_generate_players()
    @see player_generate() */
#define CONSTANT_TEAM_SKILL_VARIANCE 0.075

/**
   Generate a team with default values, e.g. 
   random playing structure and an empty string
   as name.
   @return A new team.
*/
Team
team_new(void)
{
    Team new;

    new.name = g_string_new("");

    new.clid = new.id = -1;
    new.structure = team_assign_playing_structure();
    new.style = team_assign_playing_style();

    new.stadium = team_stadium_new();

    new.players = g_array_new(FALSE, FALSE, sizeof(Player));

    return new;
}

/**
   Return a random playing style.
   @see The #TeamPlayingStyle enumeration.
   @see The #CONSTANT_TEAM_PLAYING_STYLE_PROB1 define.
*/
gint
team_assign_playing_style(void)
{
  gfloat rndom;

  rndom = math_rnd(0,1);

  /* all out defend */
  if(rndom < CONSTANT_TEAM_PLAYING_STYLE_PROB1)
    return -2;
  /* defend */
  else if(rndom < CONSTANT_TEAM_PLAYING_STYLE_PROB2)
    return -1;
  /* balanced */
  else if(rndom < CONSTANT_TEAM_PLAYING_STYLE_PROB3)
    return 0;
  /* attack */
  else if(rndom < CONSTANT_TEAM_PLAYING_STYLE_PROB4)
    return 1;

  /* all out attack */
  return 2;
}

/**
   Return a random playing structure.
   @see The #CONSTANT_TEAM_PLAYING_STRUCTURE_PROB1 define.
*/
gint
team_assign_playing_structure(void)
{
  gfloat rndom = math_rnd(0,1);
    
  if(rndom < CONSTANT_TEAM_PLAYING_STRUCTURE_PROB1)
    return 532;
  else if(rndom < CONSTANT_TEAM_PLAYING_STRUCTURE_PROB2)
    return 442;
  else if(rndom < CONSTANT_TEAM_PLAYING_STRUCTURE_PROB3)
    return 352;
  else if(rndom < CONSTANT_TEAM_PLAYING_STRUCTURE_PROB4)
    return 433;

  return 343;
}

/** Return a stadium struct with default values. */
Stadium
team_stadium_new(void)
{
    Stadium new;

    new.capacity = -1;
    new.safety = math_gauss_disti(80, 100);
    new.average_attendance = 
	new.possible_attendance = 
	new.games = 0;

    return new;
}

/** Decide whether the team specified is the user's team.
    @param tm The team we check 
    @return TRUE if the team is the user's team, FALSE otherwise. */
gboolean
is_my_team(const Team *tm)
{
    return (tm->clid == my_team_clid && tm->id == my_team_id);
}

/* Fill the players array of the team.
   @param tm The team that gets filled. */
void
team_generate_players(Team *tm)
{
    /*d*/
    Player pl;
    gint i;    
    gfloat skill_factor = math_rnd(1 - CONSTANT_TEAM_SKILL_VARIANCE,
				   1 + CONSTANT_TEAM_SKILL_VARIANCE);
    Player new;
    gint average_skill;

    if(tm->clid < ID_CUP_START)
	average_skill = 
	    (gint)rint((gfloat)team_return_league_cup_value_int(tm, LEAGUE_CUP_VALUE_AVERAGE_SKILL) *
		       skill_factor);
    else
	average_skill = 
	    (gint)rint((gfloat)lig(0).average_skill +
		       (gfloat)team_return_league_cup_value_int(tm, LEAGUE_CUP_VALUE_SKILL_DIFF)) *
	    skill_factor;

    average_skill = CLAMP(average_skill, 0, CONSTANT_PLAYER_MAX_SKILL);
    
    for(i=0;i<CONSTANT_TEAM_MAX_PLAYERS;i++)
    {
	new = player_new(tm, average_skill);
	g_array_append_val(tm->players, new);
    }

    /*d*/
/*     if(tm->id == 0) */
/*     { */
/* 	printf("%s %p\n\n", tm->name->str, tm); */
/* 	for(i=0;i<tm->players->len;i++) */
/* 	{ */
/* 	    pl = g_array_index(tm->players, Player, i); */
/* 	    printf("%d %s sk %d tal %d etal %d\n", i, */
/* 		   pl.name->str, pl.skill, pl.talent, pl.etal); */
/* 	    printf("fit %d val %d wag %d con %.1f lsu %d goa %d gam %d\n", */
/* 		   pl.fitness, pl.value, */
/* 		   pl.wage, (gfloat)pl.contract / 52, pl.lsu, pl.goals, pl.games); */
/* 	}    */
/* 	printf("\n");printf("\n"); */
/*     } */
}

/** Return a certain value from the league or cup struct
    the team belongs to.
    @param tm The team we examine.
    @param value_type This tells us which value to return. 
    @see #LeagueCupValue */
gint
team_return_league_cup_value_int(const Team *tm, gint value_type)
{
    gint idx = tm->clid % 1000;

    if(tm->clid >= ID_CUP_START)
	switch(value_type)
	{
	    case LEAGUE_CUP_VALUE_ID:
		return cp(idx).id;
	    case LEAGUE_CUP_VALUE_LAST_WEEK:
		return cp(idx).last_week;
	    case LEAGUE_CUP_VALUE_WEEK_GAP:
		return cp(idx).week_gap;
	    case LEAGUE_CUP_VALUE_YELLOW_RED:
		return cp(idx).yellow_red;
	    case LEAGUE_CUP_VALUE_AVERAGE_CAPACITY:
		return cp(idx).average_capacity;
	    case LEAGUE_CUP_VALUE_CUP_TYPE:
		return cp(idx).type;
	    case LEAGUE_CUP_VALUE_SKILL_DIFF:
		return cp(idx).skill_diff;
	    default:
		g_warning("team_return_league_cup_value_int: unknown value_type for cups: %d\n", 
			  value_type);
		return -1;
	}

    switch(value_type)
    {
	case LEAGUE_CUP_VALUE_ID:
	    return lig(idx).id;
	case LEAGUE_CUP_VALUE_FIRST_WEEK:
	    return lig(idx).first_week;
	case LEAGUE_CUP_VALUE_WEEK_GAP:
	    return lig(idx).week_gap;
	case LEAGUE_CUP_VALUE_YELLOW_RED:
	    return lig(idx).yellow_red;
	case LEAGUE_CUP_VALUE_AVERAGE_SKILL:
	    return lig(idx).average_skill;
	case LEAGUE_CUP_VALUE_AVERAGE_CAPACITY:
	    return lig(idx).average_capacity;
	default:
	    g_warning("team_return_league_cup_value_int: unknown value_type for leagues: %d\n",
		      value_type);
	    return -1;
    }

    return -1;
}

/** Copy a team to another team. The destination team
    has to be a fully allocated team (because it gets 
    freed before we copy).
    @param source The team we copy.
    @param dest The team we overwrite. */
void
team_copy(const Team *source, Team *dest)
{
    gint i;
    Player new_player;

    free_team(dest);
    
    *dest = *source;

    dest->name = g_string_new(source->name->str);

    dest->players = g_array_new(FALSE, FALSE, sizeof(Player));
    
    for(i=0;i<source->players->len;i++)
    {
	new_player = player_new(dest, CONSTANT_PLAYER_MAX_SKILL);
	free_player(&new_player);
	player_copy(&g_array_index(source->players, Player, i),
		    &new_player);
	g_array_append_val(dest->players, new_player);
    }
}

/** Copy a team to an array in a way that allows us to
    free the team afterwards.
    @param tm The team we copy.
    @param teams_array The array of teams we copy the team to.
    @see team_copy() */
void
team_append_to_array(const Team *tm, GArray *teams_array)
{
    Team new_team = team_new();

    team_copy(tm, &new_team);

    g_array_append_val(teams_array, new_team);
}

/** Copy a team to an array and assign new ids.
    @param tm The team we copy.
    @param teams_array The array of teams we copy the team to.
    @param clid The cup/league id we assign.
    @param id The team id we assign.
 */
void
team_append_to_array_with_ids(const Team *tm, GArray *teams_array, gint clid, gint id)
{
    team_append_to_array(tm, teams_array);

    g_array_index(teams_array, Team, teams_array->len - 1).clid = clid;
    g_array_index(teams_array, Team, teams_array->len - 1).id = id;
}

/** Check whether the team is already part of an
    international cup. We'd like to avoid having Real Madrid
    both in the Champions' League and in the CWC.
    @param tm The team we check (by comparing names).
    @return TRUE if the team's already participating in a cup,
    FALSE otherwise. */
gboolean
is_in_international_cups(const Team *tm)
{
    gint i, j;

    for(i=0;i<cps->len;i++)
	for(j=0;j<cp(i).teams->len;j++)
	    if(cp(i).type == CUP_TYPE_INTERNATIONAL &&
	       strcmp(tm->name->str, g_array_index(cp(i).teams, Team, j).name->str) == 0)
		return TRUE;

    return FALSE;
}
