#include "maths.h"
#include "misc.h"
#include "player.h"
#include "team.h"
#include "treeview.h"
#include "treeview_cell.h"

#define CONSTANT_TREEVIEW_CELL_COLOR_PLAYER_POS_GOALIE_BG "Black"
#define CONSTANT_TREEVIEW_CELL_COLOR_PLAYER_POS_GOALIE_FG "White"
#define CONSTANT_TREEVIEW_CELL_COLOR_PLAYER_POS_DEFENDER_BG "Darkgreen"
#define CONSTANT_TREEVIEW_CELL_COLOR_PLAYER_POS_DEFENDER_FG "White"
#define CONSTANT_TREEVIEW_CELL_COLOR_PLAYER_POS_MIDFIELDER_BG "Darkblue"
#define CONSTANT_TREEVIEW_CELL_COLOR_PLAYER_POS_MIDFIELDER_FG "White"
#define CONSTANT_TREEVIEW_CELL_COLOR_PLAYER_POS_FORWARD_BG "Darkred"
#define CONSTANT_TREEVIEW_CELL_COLOR_PLAYER_POS_FORWARD_FG "White"

#define CONSTANT_TREEVIEW_CELL_COLOR_PLAYER_INJURY "Lightgreen"
#define CONSTANT_TREEVIEW_CELL_COLOR_PLAYER_BANNED "Red"

#define CONSTANT_TREEVIEW_CELL_LIMIT_PLAYER_FITNESS_BELOW1 85
#define CONSTANT_TREEVIEW_CELL_LIMIT_PLAYER_FITNESS_BELOW2 70
#define CONSTANT_TREEVIEW_CELL_LIMIT_PLAYER_FITNESS_BELOW3 60
#define CONSTANT_TREEVIEW_CELL_COLOR_PLAYER_FITNESS_NORMAL "DarkGreen"
#define CONSTANT_TREEVIEW_CELL_COLOR_PLAYER_FITNESS_BELOW1 "DarkOrange"
#define CONSTANT_TREEVIEW_CELL_COLOR_PLAYER_FITNESS_BELOW2 "OrangeRed"
#define CONSTANT_TREEVIEW_CELL_COLOR_PLAYER_FITNESS_BELOW3 "Red"

#define CONSTANT_TREEVIEW_CELL_LIMIT_PLAYER_CONTRACT_BELOW1 52
#define CONSTANT_TREEVIEW_CELL_LIMIT_PLAYER_CONTRACT_BELOW2 40
#define CONSTANT_TREEVIEW_CELL_LIMIT_PLAYER_CONTRACT_BELOW3 20
#define CONSTANT_TREEVIEW_CELL_COLOR_PLAYER_CONTRACT_NORMAL "DarkGreen"
#define CONSTANT_TREEVIEW_CELL_COLOR_PLAYER_CONTRACT_BELOW1 "DarkOrange"
#define CONSTANT_TREEVIEW_CELL_COLOR_PLAYER_CONTRACT_BELOW2 "OrangeRed"
#define CONSTANT_TREEVIEW_CELL_COLOR_PLAYER_CONTRACT_BELOW3 "Red"

#define CONSTANT_TREEVIEW_CELL_COLOR_PLAYER_BAD_CSKILL_BG "Lightblue"
#define CONSTANT_TREEVIEW_CELL_COLOR_PLAYER_BAD_CSKILL_FG "Red"

/** Font attributes for the live game window result. */
#define CONSTANT_TREEVIEW_CELL_LIVE_GAME_RESULT_ATTRIBUTES "weight='bold' size='large'"

/* #define CONSTANT_TREEVIEW_CELL_COLOR_ "" */
/* #define CONSTANT_TREEVIEW_CELL_COLOR_ "" */
/* #define CONSTANT_TREEVIEW_CELL_COLOR_ "" */
/* #define CONSTANT_TREEVIEW_CELL_COLOR_ "" */


/** Render a cell in the team selection treeview.
    @see The GTK reference. */
void
treeview_cell_team_selection(GtkTreeViewColumn *col,
			     GtkCellRenderer   *renderer,
			     GtkTreeModel      *model,
			     GtkTreeIter       *iter,
			     gpointer           user_data)
{
    gint column = GPOINTER_TO_INT(user_data);
    gpointer team_pointer;

    if(strcmp(font_name->str, "0") != 0)
	g_object_set(renderer, "font", font_name->str, NULL);

    gtk_tree_model_get(model, iter, column, &team_pointer, -1);

    if(column == 2)
	g_object_set(renderer, "text", ((Team*)team_pointer)->name->str, NULL);
    else
	g_warning("treeview_cell_team_selection: unknown column: %d\n", column);
}

/** Render an integer. This is only so that we know when to draw nothing.
    @see #CONSTANT_TREEVIEW_CELL_INT_EMPTY */
void
treeview_cell_int_to_cell(GtkTreeViewColumn *col,
			  GtkCellRenderer   *renderer,
			  GtkTreeModel      *model,
			  GtkTreeIter       *iter,
			  gpointer           user_data)
{
    gint column = GPOINTER_TO_INT(user_data);
    gint value;
    gchar  buf[SMALL];

    if(strcmp(font_name->str, "0") != 0)
	g_object_set(renderer, "font", font_name->str, NULL);

    gtk_tree_model_get(model, iter, column, &value, -1);
    
    if(value == CONSTANT_TREEVIEW_CELL_INT_EMPTY)
	strcpy(buf, "");
    else
	sprintf(buf, "%d", value);

    g_object_set(renderer, "text", buf, NULL);
}

/** Render a player list cell. */
void
treeview_cell_player_to_cell(GtkTreeViewColumn *col,
			     GtkCellRenderer   *renderer,
			     GtkTreeModel      *model,
			     GtkTreeIter       *iter,
			     gpointer           user_data)
{
    gint column = treeview_get_col_number_column(col);
    gint attribute = GPOINTER_TO_INT(user_data);
    gchar  buf[SMALL];
    gchar fg_color[SMALL],
	bg_color[SMALL];
    Player *pl;

    if(strcmp(font_name->str, "0") != 0)
	g_object_set(renderer, "font", font_name->str, NULL);

    strcpy(fg_color, "black");
    strcpy(bg_color, "white");
    strcpy(buf, "");

    gtk_tree_model_get(model, iter, column, &pl, -1);

    if(pl == NULL)
    {
	g_object_set(renderer, "text", "", "foreground", fg_color, "background", bg_color, NULL);
	return;
    }

    switch(attribute)
    {
	default:
	    strcpy(buf, pl->name->str);
	    break;
	case PLAYER_LIST_ATTRIBUTE_CPOS:
	    treeview_cell_player_pos_to_cell(renderer, buf, pl->cpos);
	    break;
	case PLAYER_LIST_ATTRIBUTE_POS:
	    treeview_cell_player_pos_to_cell(renderer, buf, pl->pos);
	    break;
	case PLAYER_LIST_ATTRIBUTE_CSKILL:
	    sprintf(buf, "%d", pl->cskill);
	    break;
	case PLAYER_LIST_ATTRIBUTE_SKILL:
	    sprintf(buf, "%d", pl->skill);
	    break;
	case PLAYER_LIST_ATTRIBUTE_FITNESS:
	    treeview_cell_player_fitness_to_cell(renderer, buf, pl->fitness);
	    break;
	case PLAYER_LIST_ATTRIBUTE_GAMES:
	    /*todo*/
	    treeview_cell_player_games_goals_to_cell(buf, pl, FALSE);
	    break;
	case PLAYER_LIST_ATTRIBUTE_GOALS:
	    treeview_cell_player_games_goals_to_cell(buf, pl, TRUE);
	    break;
	case PLAYER_LIST_ATTRIBUTE_STATUS:
	    treeview_cell_player_status_to_cell(renderer, buf, pl);
	    break;
	case PLAYER_LIST_ATTRIBUTE_CARDS:
	    treeview_cell_player_cards_to_cell(buf, pl);
	    break;
	case PLAYER_LIST_ATTRIBUTE_AGE:
	    sprintf(buf, "%d", (gint)rint((gfloat)pl->age / 52));
	    break;
	case PLAYER_LIST_ATTRIBUTE_ETAL:
	    sprintf(buf, "%d", pl->etal);
	    break;
	case PLAYER_LIST_ATTRIBUTE_VALUE:
	    misc_print_grouped_int(pl->value, buf, FALSE);
	    break;
	case PLAYER_LIST_ATTRIBUTE_WAGE:
	    misc_print_grouped_int(pl->wage, buf, FALSE);
	    break;
	case PLAYER_LIST_ATTRIBUTE_CONTRACT:
	    treeview_cell_player_contract_to_cell(renderer, buf, pl->contract);
	    break;
	case PLAYER_LIST_ATTRIBUTE_TEAM:
	    sprintf(buf, "%s", pl->team->name->str);
	    break;
	case PLAYER_LIST_ATTRIBUTE_LEAGUE_CUP:
	    team_get_league_cup_string(pl->team, LEAGUE_CUP_VALUE_NAME, buf);
	    break;
    }

    g_object_set(renderer, "text", buf, NULL);
}

/** Render a cell of a player contract.
    @param renderer The cell renderer.
    @param buf The string the cell will contain.
    @param contract_time The contract time in weeks. */
void
treeview_cell_player_contract_to_cell(GtkCellRenderer *renderer, gchar *buf, gint contract_time)
{
    sprintf(buf, "%.1f", (gfloat)contract_time / 52);

    if(contract_time < CONSTANT_TREEVIEW_CELL_LIMIT_PLAYER_CONTRACT_BELOW3)
	g_object_set(renderer, "foreground", CONSTANT_TREEVIEW_CELL_COLOR_PLAYER_CONTRACT_BELOW3, NULL);
    else if(contract_time < CONSTANT_TREEVIEW_CELL_LIMIT_PLAYER_CONTRACT_BELOW2)
	g_object_set(renderer, "foreground", CONSTANT_TREEVIEW_CELL_COLOR_PLAYER_CONTRACT_BELOW2, NULL);
    else if(contract_time < CONSTANT_TREEVIEW_CELL_LIMIT_PLAYER_CONTRACT_BELOW1)
	g_object_set(renderer, "foreground", CONSTANT_TREEVIEW_CELL_COLOR_PLAYER_CONTRACT_BELOW1, NULL);
    else
	g_object_set(renderer, "foreground", CONSTANT_TREEVIEW_CELL_COLOR_PLAYER_CONTRACT_NORMAL, NULL);
}

/** Render a cell of player yellow cards.
    @param buf The string the cell will contain.
    @param pl The pointer to the player. */
void
treeview_cell_player_cards_to_cell(gchar *buf, const Player *pl)
{
    gint i;
    Fixture *fix =
	team_get_next_fixture(pl->team);

    if(fix == NULL)
    {
	if(pl->cards->len == 0)
	    strcpy(buf, "0");
	else
	    sprintf(buf, "%d(%d)", g_array_index(pl->cards, PlayerCard, 0).yellow,
		    player_all_cards(pl));

	return;
    }

    for(i=0;i<pl->cards->len;i++)
	if(g_array_index(pl->cards, PlayerCard, 0).clid == fix->clid)
	{
	    sprintf(buf, "%d(%d)", g_array_index(pl->cards, PlayerCard, i).yellow,
		    player_all_cards(pl));
	    return;
	}

    strcpy(buf, "0");
}

/** Render a cell of player status.
    @param render The cell renderer.
    @param buf The string the cell will contain.
    @param pl The pointer to the player. */
void
treeview_cell_player_status_to_cell(GtkCellRenderer *renderer, gchar *buf, const Player *pl)
{
    gint i;
    Fixture *fix = team_get_next_fixture(pl->team);

    if(pl->health != PLAYER_INJURY_NONE)
    {
	sprintf(buf, _("INJ(%d)"), pl->recovery);
	g_object_set(renderer, "background", 
		     CONSTANT_TREEVIEW_CELL_COLOR_PLAYER_INJURY, NULL);

	return;
    }

    if(fix == NULL)
    {
	strcpy(buf, "OK");
	return;
    }

    for(i=0;i<pl->cards->len;i++)
	if(g_array_index(pl->cards, PlayerCard, 0).clid == fix->clid)
	{
	    sprintf(buf, _("BAN(%d)"), g_array_index(pl->cards, PlayerCard, 0).red);
	    g_object_set(renderer, "background",
			 CONSTANT_TREEVIEW_CELL_COLOR_PLAYER_BANNED, NULL);
	    return;
	}

    strcpy(buf, "OK");
}

/** Render a cell of player games or goals.
    @param buf The string the cell will contain.
    @param pl The pointer to the player.
    @param goals Whether we render games or goals. */
void
treeview_cell_player_games_goals_to_cell(gchar *buf, const Player *pl, gboolean goals)
{
    gint i;
    Fixture *fix = team_get_next_fixture(pl->team);

    if(fix == NULL)
    {
	if(pl->games_goals->len == 0)
	    strcpy(buf, "0");
	else
	{
	    if(goals)
		sprintf(buf, "%d(%d)",
			g_array_index(pl->games_goals, PlayerGamesGoals, 0).goals,
			player_all_games_goals(pl, goals));
	    else
		sprintf(buf, "%d(%d)",
			g_array_index(pl->games_goals, PlayerGamesGoals, 0).games,
			player_all_games_goals(pl, goals));
	}

	return;
    }

    for(i=0;i<pl->games_goals->len;i++)
	if(g_array_index(pl->games_goals, PlayerGamesGoals, 0).clid == fix->clid)
	{
	    if(goals)
		sprintf(buf, "%d(%d)", g_array_index(pl->games_goals, PlayerGamesGoals, 0).goals,
			player_all_games_goals(pl, goals));
	    else
		sprintf(buf, "%d(%d)", g_array_index(pl->games_goals, PlayerGamesGoals, 0).games,
			player_all_games_goals(pl, goals));
	    
	    return;
	}

    sprintf(buf, "0");
}

/** Render a cell of player fitness.
    @param render The cell renderer.
    @param buf The string the cell will contain.
    @param fitness The fitness of the player. */
void
treeview_cell_player_fitness_to_cell(GtkCellRenderer *renderer, gchar *buf, gint fitness)
{
    sprintf(buf, "%d%%", fitness);

    if(fitness < CONSTANT_TREEVIEW_CELL_LIMIT_PLAYER_FITNESS_BELOW3)
	g_object_set(renderer, "foreground", CONSTANT_TREEVIEW_CELL_COLOR_PLAYER_FITNESS_BELOW3, NULL);
    else if(fitness < CONSTANT_TREEVIEW_CELL_LIMIT_PLAYER_FITNESS_BELOW2)
	g_object_set(renderer, "foreground", CONSTANT_TREEVIEW_CELL_COLOR_PLAYER_FITNESS_BELOW2, NULL);
    else if(fitness < CONSTANT_TREEVIEW_CELL_LIMIT_PLAYER_FITNESS_BELOW1)
	g_object_set(renderer, "foreground", CONSTANT_TREEVIEW_CELL_COLOR_PLAYER_FITNESS_BELOW1, NULL);
    else
	g_object_set(renderer, "foreground", CONSTANT_TREEVIEW_CELL_COLOR_PLAYER_FITNESS_NORMAL, NULL);
}

/** Render a cell of player position or cposition.
    @param render The cell renderer.
    @param buf The string the cell will contain.
    @param pos The position of the player. */
void
treeview_cell_player_pos_to_cell(GtkCellRenderer *renderer, gchar *buf, gint pos)
{
    switch(pos)
    {
	default:
	    strcpy(buf, "G");
	    g_object_set(renderer, "background", CONSTANT_TREEVIEW_CELL_COLOR_PLAYER_POS_GOALIE_BG,
			 "foreground", CONSTANT_TREEVIEW_CELL_COLOR_PLAYER_POS_GOALIE_FG, NULL);
	    break;
	case PLAYER_POS_DEFENDER:
	    strcpy(buf, "D");
	    g_object_set(renderer, "background", CONSTANT_TREEVIEW_CELL_COLOR_PLAYER_POS_DEFENDER_BG,
			 "foreground", CONSTANT_TREEVIEW_CELL_COLOR_PLAYER_POS_DEFENDER_FG, NULL);
	    break;
	case PLAYER_POS_MIDFIELDER:
	    strcpy(buf, "M");
	    g_object_set(renderer, "background", CONSTANT_TREEVIEW_CELL_COLOR_PLAYER_POS_MIDFIELDER_BG,
			 "foreground", CONSTANT_TREEVIEW_CELL_COLOR_PLAYER_POS_MIDFIELDER_FG, NULL);
	    break;
	case PLAYER_POS_FORWARD:
	    strcpy(buf, "F");
	    g_object_set(renderer, "background", CONSTANT_TREEVIEW_CELL_COLOR_PLAYER_POS_FORWARD_BG,
			 "foreground", CONSTANT_TREEVIEW_CELL_COLOR_PLAYER_POS_FORWARD_FG, NULL);
	    break;
    }
}

/** Render a cell of player cskill.
    @param render The cell renderer.
    @param buf The string the cell will contain.
    @param pl The pointer to the player. */
void
treeview_cell_player_cskill_to_cell(GtkCellRenderer *renderer, gchar *buf, const Player *pl)
{
    sprintf(buf, "%d", (gint)rint((gfloat)pl->cskill * 
				  powf((gfloat)pl->fitness, 
				       CONSTANT_PLAYER_FITNESS_IMPACT_ON_SKILL)));
	    
    if(pl->cskill < pl->skill)
	g_object_set(renderer, "background", CONSTANT_TREEVIEW_CELL_COLOR_PLAYER_BAD_CSKILL_BG,
		     "foreground", CONSTANT_TREEVIEW_CELL_COLOR_PLAYER_BAD_CSKILL_FG, NULL);
}

/** Render the result (team names and goals) in the live game view. */
void
treeview_cell_live_game_result(GtkTreeViewColumn *col,
			       GtkCellRenderer   *renderer,
			       GtkTreeModel      *model,
			       GtkTreeIter       *iter,
			       gpointer           user_data)
{
    
    gint i, result[2];
    gint column = treeview_get_col_number_column(col);
    gchar  buf[SMALL];    
    Fixture *fix;

    if(strcmp(font_name->str, "0") != 0)
	g_object_set(renderer, "font", font_name->str, NULL);

    strcpy(buf, "");

    gtk_tree_model_get(model, iter, column, &fix, -1);

    if(fix != NULL)
    {
	for(i=0;i<2;i++)
	    result[i] = math_sum_int_array(fix->result[i], 3);

	if(column== 1 || column == 3)
	    sprintf(buf, "<span %s>%s</span>", CONSTANT_TREEVIEW_CELL_LIVE_GAME_RESULT_ATTRIBUTES,
		    fix->teams[column == 3]->name->str);
	else
	    sprintf(buf, "<span %s>%d : %d</span>", CONSTANT_TREEVIEW_CELL_LIVE_GAME_RESULT_ATTRIBUTES,
		    result[0], result[1]);
    }

    g_object_set(renderer, "markup", buf, NULL);
}
