#include "misc.h"
#include "option.h"
#include "player.h"
#include "team.h"
#include "treeview.h"
#include "treeview_cell.h"
#include "user.h"
#include "variables.h"

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

    gtk_tree_model_get(model, iter, column, &team_pointer, -1);

    if(column == 2)
	g_object_set(renderer, "text", ((Team*)team_pointer)->name->str, NULL);
    else
	g_warning("treeview_cell_team_selection: unknown column: %d\n", column);
}

/** Render an integer. This is only so that we know when to draw nothing. */
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

    if(strcmp(current_user.font_name->str, "0") != 0)
	g_object_set(renderer, "font", current_user.font_name->str, NULL);

    gtk_tree_model_get(model, iter, column, &value, -1);
    
    if(value == const_int("int_treeview_cell_int_empty"))
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
    Player *pl;

    if(strcmp(current_user.font_name->str, "0") != 0)
	g_object_set(renderer, "font", current_user.font_name->str, NULL);

    g_object_set(renderer, "text", "", "foreground",
		 const_str("string_treeview_cell_color_default_foreground"),
		 "background",
		 const_str("string_treeview_cell_color_default_background"), NULL);
    strcpy(buf, "");

    gtk_tree_model_get(model, iter, column, &pl, -1);

    if(pl == NULL)
	return;

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
	    treeview_cell_player_cskill_to_cell(renderer, buf, pl);
	    break;
	case PLAYER_LIST_ATTRIBUTE_SKILL:
	    sprintf(buf, "%.*f", opt_int("int_opt_player_precision"), pl->skill);
	    break;
	case PLAYER_LIST_ATTRIBUTE_FITNESS:
	    treeview_cell_player_fitness_to_cell(renderer, buf, pl->fitness);
	    break;
	case PLAYER_LIST_ATTRIBUTE_GAMES:
	    treeview_cell_player_games_goals_to_cell(buf, pl, PLAYER_VALUE_GAMES);
	    break;
	case PLAYER_LIST_ATTRIBUTE_GOALS:
	    treeview_cell_player_games_goals_to_cell(buf, pl, PLAYER_VALUE_GOALS);
	    break;
	case PLAYER_LIST_ATTRIBUTE_SHOTS:
	    treeview_cell_player_games_goals_to_cell(buf, pl, PLAYER_VALUE_SHOTS);
	    break;
	case PLAYER_LIST_ATTRIBUTE_STATUS:
	    treeview_cell_player_status_to_cell(renderer, buf, pl);
	    break;
	case PLAYER_LIST_ATTRIBUTE_CARDS:
	    treeview_cell_player_cards_to_cell(buf, pl);
	    break;
	case PLAYER_LIST_ATTRIBUTE_AGE:
	    sprintf(buf, "%.*f", opt_int("int_opt_player_precision"), pl->age);
	    break;
	case PLAYER_LIST_ATTRIBUTE_ETAL:
	    sprintf(buf, "%.*f", opt_int("int_opt_player_precision"),
		    pl->etal[current_user.scout % 10]);
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
treeview_cell_player_contract_to_cell(GtkCellRenderer *renderer, gchar *buf, gfloat contract_time)
{
    sprintf(buf, "%.*f", 1 + opt_int("int_opt_player_precision"),
	    contract_time);

    if(contract_time < const_float("float_treeview_cell_limit_player_contract_below3"))
	g_object_set(renderer, "foreground", 
		     const_str("string_treeview_cell_color_player_contract_below3"), NULL);
    else if(contract_time < const_float("float_treeview_cell_limit_player_contract_below2"))
	g_object_set(renderer, "foreground", 
		     const_str("string_treeview_cell_color_player_contract_below2"), NULL);
    else if(contract_time < const_float("float_treeview_cell_limit_player_contract_below1"))
	g_object_set(renderer, "foreground", 
		     const_str("string_treeview_cell_color_player_contract_below1"), NULL);
    else
	g_object_set(renderer, "foreground", 
		     const_str("string_treeview_cell_color_player_contract_normal"), NULL);
}

/** Render a cell of player yellow cards.
    @param buf The string the cell will contain.
    @param pl The pointer to the player. */
void
treeview_cell_player_cards_to_cell(gchar *buf, const Player *pl)
{
    gint yellow;
    Fixture *fix =
	team_get_fixture(pl->team, FALSE);

    if(fix == NULL)
    {
	if(pl->cards->len == 0)
	    strcpy(buf, "0");
	else
	{
	    if(opt_user_int("int_opt_user_show_overall"))
		sprintf(buf, "%d(%d)", g_array_index(pl->cards, PlayerCard, 0).yellow,
			player_all_cards(pl));
	    else
		sprintf(buf, "%d", g_array_index(pl->cards, PlayerCard, 0).yellow);
	}

	return;
    }

    yellow = player_card_get(pl, fix->clid, PLAYER_VALUE_CARD_YELLOW);

    if(yellow != -1)
    {
	if(opt_user_int("int_opt_user_show_overall"))
	    sprintf(buf, "%d(%d)", yellow,
		    player_all_cards(pl));
	else
	    sprintf(buf, "%d", yellow);
    }
    else
	strcpy(buf, "0");
}

/** Render a cell of player status.
    @param render The cell renderer.
    @param buf The string the cell will contain.
    @param pl The pointer to the player. */
void
treeview_cell_player_status_to_cell(GtkCellRenderer *renderer, gchar *buf, const Player *pl)
{
    gint ban = player_is_banned(pl);

    if(pl->health != PLAYER_INJURY_NONE)
    {
	sprintf(buf, _("INJ(%d)"), pl->recovery);
	g_object_set(renderer, "background", 
		     const_str("string_treeview_cell_color_player_injury"), NULL);

	return;
    }

    if(ban > 0)
    {
	sprintf(buf, _("BAN(%d)"), ban);
	g_object_set(renderer, "background",
		     const_str("string_treeview_cell_color_player_banned"), NULL);
    }
    else
	strcpy(buf, _("OK"));

    if(ban == -1)
	g_object_set(renderer, "background",
		     const_str("string_treeview_cell_color_player_yellow_danger"), NULL);
}

/** Render a cell of player games or goals.
    @param buf The string the cell will contain.
    @param pl The pointer to the player.
    @param goals Whether we render games or goals. */
void
treeview_cell_player_games_goals_to_cell(gchar *buf, const Player *pl, gint type)
{
    Fixture *fix = team_get_fixture(pl->team, FALSE);
    gint clid = pl->team->clid;

    if(pl->games_goals->len == 0)
    {
	strcpy(buf, "0");
	return;
    }

    if(fix != NULL)
	clid = fix->clid;

    if(opt_user_int("int_opt_user_show_overall"))
	sprintf(buf, "%d(%d)", player_games_goals_get(pl, clid, type),
		player_all_games_goals(pl, PLAYER_VALUE_GOALS));
    else
	sprintf(buf, "%d", player_games_goals_get(pl, clid, type));
}

/** Render a cell of player fitness.
    @param render The cell renderer.
    @param buf The string the cell will contain.
    @param fitness The fitness of the player. */
void
treeview_cell_player_fitness_to_cell(GtkCellRenderer *renderer, gchar *buf, gfloat fitness)
{
    sprintf(buf, "%d%%", (gint)rint(fitness * 100));

    if(fitness < const_float("float_treeview_cell_limit_player_fitness_below3"))
	g_object_set(renderer, "foreground", 
		     const_str("string_treeview_cell_color_player_fitness_below3"), NULL);
    else if(fitness < const_float("float_treeview_cell_limit_player_fitness_below2"))
	g_object_set(renderer, "foreground", 
		     const_str("string_treeview_cell_color_player_fitness_below2"), NULL);
    else if(fitness < const_float("float_treeview_cell_limit_player_fitness_below2"))
	g_object_set(renderer, "foreground", 
		     const_str("string_treeview_cell_color_player_fitness_below2"), NULL);
    else
	g_object_set(renderer, "foreground", 
		     const_str("string_treeview_cell_color_player_fitness_normal"), NULL);
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
	    g_object_set(renderer, "background", 
			 const_str("string_treeview_cell_color_player_pos_goalie_bg"),
			 "foreground", 
			 const_str("string_treeview_cell_color_player_pos_goalie_fg"), NULL);
	    break;
	case PLAYER_POS_DEFENDER:
	    strcpy(buf, "D");
	    g_object_set(renderer, "background", 
			 const_str("string_treeview_cell_color_player_pos_defender_bg"),
			 "foreground", 
			 const_str("string_treeview_cell_color_player_pos_defender_fg"), NULL);
	    break;
	case PLAYER_POS_MIDFIELDER:
	    strcpy(buf, "M");
	    g_object_set(renderer, "background", 
			 const_str("string_treeview_cell_color_player_pos_midfielder_bg"),
			 "foreground", 
			 const_str("string_treeview_cell_color_player_pos_midfielder_fg"), NULL);
	    break;
	case PLAYER_POS_FORWARD:
	    strcpy(buf, "F");
	    g_object_set(renderer, "background", 
			 const_str("string_treeview_cell_color_player_pos_forward_bg"),
			 "foreground", 
			 const_str("string_treeview_cell_color_player_pos_forward_fg"), NULL);
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
    sprintf(buf, "%.*f", opt_int("int_opt_player_precision"),
	    player_get_game_skill(pl, FALSE));
	    
    if(pl->cskill < pl->skill)
	g_object_set(renderer, "background", 
		     const_str("string_treeview_cell_color_player_bad_cskill_bg"),
		     "foreground", 
		     const_str("string_treeview_cell_color_player_bad_cskill_fg"), NULL);
    else
	g_object_set(renderer, "background", 
		     const_str("string_treeview_cell_color_default_background"),
		     "foreground", 
		     const_str("string_treeview_cell_color_default_foreground"), NULL);
	
}

/** Render the result (team names and goals) in the live game view. */
void
treeview_cell_live_game_result(GtkTreeViewColumn *col,
			       GtkCellRenderer   *renderer,
			       GtkTreeModel      *model,
			       GtkTreeIter       *iter,
			       gpointer           user_data)
{
    
    gint column = treeview_get_col_number_column(col);
    gchar  buf[SMALL];    
    Fixture *fix = NULL;
    LiveGameUnit *unit = NULL;

    if(strcmp(current_user.font_name->str, "0") != 0)
	g_object_set(renderer, "font", current_user.font_name->str, NULL);

    strcpy(buf, "");

    if(column == 0 || column == 2)
    {
	gtk_tree_model_get(model, iter, column, &fix, -1);
	sprintf(buf, "<span %s>%s</span>", 
		const_str("string_treeview_cell_live_game_result_attributes"),
		fix->teams[column == 2]->name->str);
    }
    else if(column == 1)
    {
	gtk_tree_model_get(model, iter, column, &unit, -1);
	sprintf(buf, "<span %s>%d : %d</span>", 
		const_str("string_treeview_cell_live_game_result_attributes"),
		unit->result[0], unit->result[1]);

    }

    g_object_set(renderer, "markup", buf, NULL);
}
