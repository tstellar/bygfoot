#include "cup.h"
#include "fixture.h"
#include "league.h"
#include "main.h"
#include "maths.h"
#include "player.h"
#include "start_end.h"
#include "team.h"
#include "transfer.h"
#include "variables.h"
#include "xml_name.h"


/** Generate the teams etc. */
void
start_new_game(void)
{
    start_write_variables();
    xml_name_read(PLAYER_NAMES_FILE, -1);
    start_generate_league_teams();
    start_new_season();
    xml_name_read(PLAYER_NAMES_FILE, 1000);
}

/** Make new fixtures, nullify things etc. */
void
start_new_season(void)
{
    start_load_cup_teams();
}

/** Fill some global variables with default values at the
    beginning of a new game. */
void
start_write_variables(void)
{
    season = week = week_round = 1;
    scout = physio = QUALITY_AVERAGE;
    
    fixtures = g_array_new(FALSE, FALSE, sizeof(Fixture));
    transfer_list = g_array_new(FALSE, FALSE, sizeof(TransferPlayer));        
}

/** Generate the teams in the leagues. */
void
start_generate_league_teams(void)
{
    gint i, j;    
    Team *tm;

    if(ligs->len == 0)
    {
	g_warning("start_generate_league_teams: no leagues found. there must be at least one league in the game.\n");
	main_exit_program(EXIT_NO_LEAGUES);
    }

    for(i=0;i<ligs->len;i++)
    {
	for(j=0;j<lig(i).teams->len;j++)
	{
	    tm = &g_array_index(lig(i).teams, Team, j);
	    tm->stadium.capacity = 
		math_gauss_disti((gint)(lig(i).average_capacity * 0.66),
				 (gint)(lig(i).average_capacity * 1.33));
	    
	    team_generate_players(tm);
	}
    }
}

/** Load the names from the xml files for the cups
    and generate the teams. */
void
start_load_cup_teams(void)
{
    gint i, j;

    for(i=0;i<cps->len;i++)
	if(cp(i).type == CUP_TYPE_INTERNATIONAL)
	{
	    cup_load_choose_teams(&cp(i));
	    cup_load_choose_team_user(&cp(i));

	    for(j=0;j<cp(i).teams->len;j++)
	    {
		printf("%d %s clid %d id %d\n", j,
		       g_array_index(cp(i).teams, Team, j).name->str,
		       g_array_index(cp(i).teams, Team, j).clid,
		       g_array_index(cp(i).teams, Team, j).id);
		printf("%s %d %d\n", 
		       g_array_index(g_array_index(cp(i).teams, Team, j).players,
				     Player, 0).name->str,
		       g_array_index(g_array_index(cp(i).teams, Team, j).players,
				     Player, 0).skill,
		       g_array_index(g_array_index(cp(i).teams, Team, j).players,
				     Player, 0).talent);
	    }
	}
}
