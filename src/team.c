#include "team.h"

/**
   Playing styles for teams.
   @see The match result calculating functions.
*/
enum TeamPlayingStyle
{
    PLAYING_STYLE_ALL_OUT_DEFEND = -2,
    PLAYING_STYLE_DEFEND,
    PLAYING_STYLE_BALANCED,
    PLAYING_STYLE_ATTACK,
    PLAYING_STYLE_ALL_OUT_ATTACK,
    PLAYING_STYLE_END
};

/**
   Constants determining the probabilities for
   the playing styles of CPU teams.
   @see team_assign_playing_style().
*/
#define CONSTANT_PLAYING_STYLE_PROB1 0.1
#define CONSTANT_PLAYING_STYLE_PROB2 0.25
#define CONSTANT_PLAYING_STYLE_PROB3 0.75
#define CONSTANT_PLAYING_STYLE_PROB4 0.9

/**
   Constants determining the probabilities for
   the playing structures of CPU teams.
   @see team_assign_playing_structure().
*/
#define CONSTANT_PLAYING_STRUCTURE_PROB1 0.15
#define CONSTANT_PLAYING_STRUCTURE_PROB2 0.5
#define CONSTANT_PLAYING_STRUCTURE_PROB3 0.7
#define CONSTANT_PLAYING_STRUCTURE_PROB4 0.85

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
    new.league_cup_id = -1;

    new.nid = -1;
    new.structure = team_assign_playing_structure();
    new.style = team_assign_playing_style();

    new.players = g_array_new(FALSE, FALSE, sizeof(Player));

    return new;
}

/**
   Return a random playing style.
   @see The #TeamPlayingStyle enumeration.
   @see The #CONSTANT_PLAYING_STYLE_PROB1 define.
*/
gint
team_assign_playing_style(void)
{
  gfloat rndom;

  rndom = rnd(0,1);

  /* all out defend */
  if(rndom < CONSTANT_PLAYING_STYLE_PROB1)
    return -2;
  /* defend */
  else if(rndom < CONSTANT_PLAYING_STYLE_PROB2)
    return -1;
  /* balanced */
  else if(rndom < CONSTANT_PLAYING_STYLE_PROB3)
    return 0;
  /* attack */
  else if(rndom < CONSTANT_PLAYING_STYLE_PROB4)
    return 1;

  /* all out attack */
  return 2;
}

/**
   Return a random playing structure.
   @see The #CONSTANT_PLAYING_STRUCTURE_PROB1 define.
*/
gint
team_assign_playing_structure(void)
{
  gfloat rndom = rnd(0,1);
    
  if(rndom < CONSTANT_PLAYING_STRUCTURE_PROB1)
    return 532;
  else if(rndom < CONSTANT_PLAYING_STRUCTURE_PROB2)
    return 442;
  else if(rndom < CONSTANT_PLAYING_STRUCTURE_PROB3)
    return 352;
  else if(rndom < CONSTANT_PLAYING_STRUCTURE_PROB4)
    return 433;

  return 343;
}
