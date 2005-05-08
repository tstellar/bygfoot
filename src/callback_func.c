#include "callback_func.h"
#include "callbacks.h"
#include "cup.h"
#include "finance.h"
#include "fixture.h"
#include "game_gui.h"
#include "league.h"
#include "live_game.h"
#include "maths.h"
#include "misc.h"
#include "option.h"
#include "player.h"
#include "start_end.h"
#include "team.h"
#include "transfer.h"
#include "treeview.h"
#include "treeview_helper.h"
#include "user.h"
#include "window.h"

/** Show the users' live games. */
void
callback_show_next_live_game(void)
{
    gint i, j;

    stat0 = STATUS_SHOW_LIVE_GAME;

    for(i=0;i<ligs->len;i++)
	for(j=0;j<lig(i).fixtures->len;j++)
	    if(g_array_index(lig(i).fixtures, Fixture, j).week_number == week &&
	       g_array_index(lig(i).fixtures, Fixture, j).week_round_number == week_round &&
	       fixture_user_team_involved(&g_array_index(lig(i).fixtures, Fixture, j)) != -1 &&
	       g_array_index(lig(i).fixtures, Fixture, j).attendance == -1 &&
	       option_int("int_opt_user_show_live_game",
			  &usr(fixture_user_team_involved(&g_array_index(lig(i).fixtures, Fixture, j))).
			  options))
	    {
		live_game_calculate_fixture(&g_array_index(lig(i).fixtures, Fixture, j));
		return;
	    }

    for(i=0;i<acps->len;i++)
	for(j=0;j<acp(i)->fixtures->len;j++)
	    if(g_array_index(acp(i)->fixtures, Fixture, j).week_number == week &&
	       g_array_index(acp(i)->fixtures, Fixture, j).week_round_number == week_round &&
	       fixture_user_team_involved(&g_array_index(acp(i)->fixtures, Fixture, j)) != -1 &&
	       g_array_index(acp(i)->fixtures, Fixture, j).attendance == -1 &&
	       option_int("int_opt_user_show_live_game",
			  &usr(fixture_user_team_involved(&g_array_index(acp(i)->fixtures, Fixture, j))).
			  options))
	    {
		live_game_calculate_fixture(&g_array_index(acp(i)->fixtures, Fixture, j));
		return;
	    }

    window_destroy(&window.live, TRUE);

    /* no more user games to show: end round. */
    end_week_round();

    setsav0;
}

/** Handle a click on the player list.
    @param idx The player number.
    @param event The type of button click. */
void
callback_player_clicked(gint idx, GdkEventButton *event)
{
    /* Only accept single-clicks right now. */
    if(event->type != GDK_BUTTON_PRESS)
	return;

    if(event->button == 1)
    {
	if(selected_row[0] == -1)
	{
	    selected_row[0] = idx;
	    return;
	}

	player_swap(current_user.tm, selected_row[0],
		    current_user.tm, idx);
	if(opt_user_int("int_opt_user_swap_adapts") == 1 &&
	   current_user.tm->structure !=
	   team_find_appropriate_structure(current_user.tm))
	{
	    team_change_structure(current_user.tm,
				  team_find_appropriate_structure(current_user.tm));
	    team_rearrange(current_user.tm);
	}

	game_gui_write_av_skills();

	selected_row[0] = -1;

	treeview_show_user_player_list();
	if(stat0 == STATUS_MAIN)
	    treeview_show_next_opponent();
    }
    else if(event->button == 3)
    {
	if(stat0 == STATUS_SHOW_TRANSFER_LIST)
	{
	    selected_row[0] = -1;	    
	    transfer_add_remove_user_player(player_of_idx_team(current_user.tm, idx));
	}
	else
	{
	    window_show_menu_player((GdkEvent*)event);
	    selected_row[0] = idx;
	}
    }

    setsav0;
}

/** Show the last match of the current user. 
    @param start Whether we start the replay from the beginning or continue it. */
void
callback_show_last_match(gboolean start)
{
    gint i;

    stat4 = -1;

    if(start)
    {
	stat2 = cur_user;

	window_create(WINDOW_LIVE);

	current_user.live_game.fix = 
	    &g_array_index(league_cup_get_fixtures(current_user.live_game.fix_clid),
			   Fixture, current_user.live_game.fix_idx);

	treeview_show_game_stats(GTK_TREE_VIEW(lookup_widget(window.live, "treeview_stats")),
				 &current_user.live_game);
	live_game_set_match(&current_user.live_game);
    }
    else
    {
	gtk_widget_set_sensitive(lookup_widget(window.live, "button_pause"), TRUE);
	gtk_widget_set_sensitive(lookup_widget(window.live, "button_resume"), FALSE);
    }

    for(i=stat3;i<current_user.live_game.units->len;i++)
    {
	game_gui_live_game_show_unit(&g_array_index(current_user.live_game.units, LiveGameUnit, i));

	if(stat4 == STATUS_SHOW_LAST_MATCH_PAUSE ||
	   stat4 == STATUS_SHOW_LAST_MATCH_ABORT)
	{
	    stat3 = i + 1;
	    break;
	}
    }

    if(stat4 == STATUS_SHOW_LAST_MATCH_PAUSE)
    {
	gtk_widget_set_sensitive(lookup_widget(window.live, "button_pause"), FALSE);
	gtk_widget_set_sensitive(lookup_widget(window.live, "button_resume"), TRUE);
    }
    else if(stat4 == STATUS_SHOW_LAST_MATCH_ABORT)
    {
	window_destroy(&window.live, TRUE);
	stat1 = stat2 = stat3 = stat4 = -1;
    }
    else
	stat3 = -1;
}

/** Show the last match stats of the current user. */
void
callback_show_last_match_stats(void)
{
    current_user.live_game.fix = 
	&g_array_index(league_cup_get_fixtures(current_user.live_game.fix_clid),
		       Fixture, current_user.live_game.fix_idx);
    
    treeview_show_game_stats(GTK_TREE_VIEW(lookup_widget(window.main, "treeview_right")),
			     &current_user.live_game);
}

/** Show some fixtures.
    @param type An integer telling us which league/cup and which
    week and round to show. */
void
callback_show_fixtures(gint type)
{
    const Fixture *fix = fixture_get(type, stat1, stat2, stat3,
				     current_user.tm);

    treeview_show_fixtures(GTK_TREE_VIEW(lookup_widget(window.main, "treeview_right")),
			   fix->clid, fix->week_number, fix->week_round_number);
    stat1 = fix->clid;
    stat2 = fix->week_number;
    stat3 = fix->week_round_number;
}

/** Show tables.
    @type Integer telling us whether to show the current user's
    tables or those of the previous/next league/cup. */
void
callback_show_tables(gint type)
{
    gint clid = -1;

    if(type == SHOW_CURRENT)
	clid = current_user.tm->clid;
    else if(type == SHOW_NEXT_LEAGUE)
    {
	clid = league_cup_get_next_clid(stat1);
	while(clid >= ID_CUP_START && cup_has_tables(clid) == -1)
	    clid = league_cup_get_next_clid(clid);
    }
    else if(type == SHOW_PREVIOUS_LEAGUE)
    {
	clid = league_cup_get_previous_clid(stat1);
	while(clid >= ID_CUP_START && cup_has_tables(clid) == -1)
	    clid = league_cup_get_previous_clid(clid);
    }
    else
	g_warning("callback_show_tables: unknown type %d \n", type);

    stat1 = clid;    

    treeview_show_table(GTK_TREE_VIEW(lookup_widget(window.main, "treeview_right")), clid);
}

/** Open the digits window to get a loan. */
void
callback_get_loan(void)
{
    gchar buf[SMALL], buf2[SMALL];
    gint max_loan = 
	finance_team_drawing_credit_loan(current_user.tm, TRUE) + current_user.debt;

    if(max_loan <= 0)
    {
	game_gui_print_message(_("The bank doesn't grant you more money."));
	return;
    }

    stat1 = STATUS_GET_LOAN;
    misc_print_grouped_int(max_loan, buf2, FALSE);
    sprintf(buf, _("You can take out at most %s."), buf2);

    window_show_digits(buf, _("Loan"), max_loan, NULL, 0);
}

/** Open the digits window to pay back a loan. */
void
callback_pay_loan(void)
{
    gchar buf[SMALL], buf2[SMALL];
    gint max_payback = MIN(BUDGET(cur_user), -current_user.debt);

    if(current_user.debt == 0)
    {
	game_gui_print_message(_("You are not indebted."));
	return;
    }

    if(max_payback <= 0)
    {
	game_gui_print_message(_("You don't have enough money to pay back."));
	return;
    }

    stat1 = STATUS_PAY_LOAN;
    misc_print_grouped_int(max_payback, buf2, FALSE);
    sprintf(buf, _("You can pay back at most %s"), buf2);

    window_show_digits(buf, _("Payback"), max_payback, NULL, 0);
}

/** Handle a click on the transfer list. 
    @param idx The index of the selected player in the transfer list. */
void
callback_transfer_list_clicked(gint idx)
{
    gchar buf[SMALL];
    Transfer *tr = &trans(idx);

    if(tr->tm == current_user.tm)
    {
	transfer_remove_player(idx);
	on_button_transfers_clicked(NULL, NULL);
	setsav0;
	return;
    }
    
    if(current_user.tm->players->len == const_int("int_team_max_players"))
    {
	game_gui_show_warning(_("Your roster is already full. You can't buy more players."));
	return;
    }

    sprintf(buf, _("You are making an offer for %s. Your scout's recommendations for value and wage are preset."),
	    player_of_id_team(tr->tm, tr->id)->name->str);
    stat1 = STATUS_SHOW_TRANSFER_LIST;
    stat2 = idx;

    window_show_digits(buf, _("Fee"), tr->fee[current_user.scout % 10],
		       _("Wage"), tr->wage[current_user.scout % 10]);
}

/** Show the contract window for the player with the specified index. */
void
callback_offer_new_contract(gint idx)
{
    gint i;
    gchar buf[SMALL];
    Player *pl = player_of_idx_team(current_user.tm, idx);
    gfloat scout_dev = math_rnd(-const_float("float_transfer_scout_deviance_wage"),
				const_float("float_transfer_scout_deviance_wage")) *
	(current_user.scout % 10 + 1);
    GtkSpinButton *spinbuttons[4];

    if(pl->contract >= 2)
    {
	game_gui_show_warning(_("You can't offer a new contract if the old one is still above 2 years."));
	return;
    }
    else if(pl->offers == const_int("int_contract_max_offers"))
    {
	game_gui_show_warning(_("The player won't negotiate with you anymore."));
	return;
    }

    stat1 = player_assign_wage(pl);
    statp = (gpointer)pl;

    if(pl->age < pl->peak_age)
	stat1 = MAX(stat1, pl->wage);
    else
	stat1 = MIN(stat1, pl->wage);

    window_create(WINDOW_CONTRACT);

    sprintf(buf, "You are negotiating with %s about a new contract. Pay attention to what you're doing; if you don't come to terms with him within %d offers, he's going to leave your team after his current contract expires (unless you sell him).\nYour scout's recommendations are preset:", pl->name->str,
	    const_int("int_contract_max_offers"));
    gtk_label_set_text(GTK_LABEL(lookup_widget(window.contract, "label_contract")), buf);

    for(i=0;i<4;i++)
    {
	sprintf(buf, "spinbutton_contract%d", i + 1);
	spinbuttons[i] = GTK_SPIN_BUTTON(lookup_widget(window.contract, buf));

	gtk_spin_button_set_value(spinbuttons[i],
				  rint((gfloat)stat1 * 
				       (1 + (i * const_float("float_contract_scale_factor") *
					     powf(-1, (pl->age > pl->peak_age)))) *
				       (1 + scout_dev)));
    }

    setsav0;
}

/** Show the player list of a team in the browse-teams mode. */
void
callback_show_team(gint type)
{
    GtkTreeView *treeview_right = GTK_TREE_VIEW(lookup_widget(window.main, "treeview_right"));
    const Team *tm;
    const GArray *teams = NULL;
    const GPtrArray *teamsp = NULL;
    gint len = -1;

    if(type == SHOW_CURRENT)
    {
	tm = (const Team*)treeview_helper_get_pointer(treeview_right, 2);
	stat1 = team_get_index(tm);
	stat2 = tm->clid;
    }
    else
    {
	if(type == SHOW_NEXT_LEAGUE)
	{
	    stat2 = league_cup_get_next_clid(stat2);
	    while(stat2 >= ID_CUP_START && cup_from_clid(stat2)->teams->len == 0)
		stat2 = league_cup_get_next_clid(stat2);
	}
	else if(type == SHOW_PREVIOUS_LEAGUE)
	{
	    stat2 = league_cup_get_previous_clid(stat2);
	    while(stat2 >= ID_CUP_START && cup_from_clid(stat2)->teams->len == 0)
		stat2 = league_cup_get_previous_clid(stat2);
	}

	if(stat2 < ID_CUP_START)
	{
	    teams = (GArray*)league_cup_get_teams(stat2);
	    len = teams->len;
	}
	else
	{
	    teamsp = (GPtrArray*)league_cup_get_teams(stat2);
	    len = teamsp->len;
	}

	if(type == SHOW_NEXT)
	    stat1 = (stat1 == len - 1) ? 0 : stat1 + 1;
	else if(type == SHOW_PREVIOUS)
	    stat1 = (stat1 == 0) ? len - 1 : stat1 - 1;
	else
	    stat1 = 0;

	if(stat2 < ID_CUP_START)
	    tm = &g_array_index(teams, Team, stat1);
	else
	    tm = (Team*)g_ptr_array_index(teamsp, stat1);	
    }

    stat0 = STATUS_BROWSE_TEAMS;

    if(tm != current_user.tm)
	treeview_show_player_list_team(treeview_right, tm, current_user.scout % 10);
    else
	callback_show_team((type == SHOW_PREVIOUS) ? SHOW_PREVIOUS : SHOW_NEXT);
}

/** Show a sortable list of all players in a league or cup. */
void
callback_show_player_list(gint type)
{
    GArray *teams = NULL;

    switch(type)
    {
	default:
	    g_warning("callback_show_player_list: unknown type %d \n", type);
	    break;
	case SHOW_CURRENT:
	    stat1 = current_user.tm->clid;
	    break;
	case SHOW_NEXT_LEAGUE:
	    stat1 = league_cup_get_next_clid(stat1);
	    break;
	case SHOW_PREVIOUS_LEAGUE:
	    stat1 = league_cup_get_previous_clid(stat1);
	    break;
    }

    teams = league_cup_get_teams(stat1);

    treeview_show_all_players(teams);
}

/** Fire a player. */
void
callback_fire_player(gint idx)
{
    gchar buf[SMALL], buf2[SMALL];
    Player *pl = player_of_idx_team(current_user.tm, idx);

    stat1 = STATUS_FIRE_PLAYER;
    stat2 = idx;
    stat3 = (gint)rint(pl->wage * const_float("float_player_fire_wage_factor") * pl->contract);

    misc_print_grouped_int(stat3, buf2, FALSE);

    sprintf(buf, _("You want to fire %s. Since his contract expires in %.1f years, he demands a compensation of %s. Do you accept?"), pl->name->str, pl->contract, buf2);

    window_show_yesno(buf);
}

/** Show fixtures by week and round (as opposed to
    competition-wise).
    @param type Whether to show current, next or previous weeks. */
void
callback_show_fixtures_week(gint type)
{
    switch(type)
    {
	default:
	    g_warning("callback_show_fixtures_week: unknown type %d \n", type);
	    break;
	case SHOW_CURRENT:
	    if(week == 1 && week_round == 1)
	    {
		stat1 = week;
		stat2 = week_round;
	    }
	    else if(week_round == 1)
	    {
		stat1 = week - 1;
		stat2 = fixture_get_last_week_round(week - 1);
	    }
	    else
	    {
		stat1 = week;
		stat2 = week_round - 1;
	    }
	    break;
	case SHOW_NEXT:
	    fixture_get_next_week(&stat1, &stat2);
	    break;
	case SHOW_PREVIOUS:
	    fixture_get_previous_week(&stat1, &stat2);
	    break;
    }

    treeview_show_fixtures_week(stat1, stat2);
}

/** Show a page with the information in the league stats
    structure. */
void
callback_show_league_stats(gint type)
{
    switch(type)
    {
	default:
	    g_warning("callback_show_league_stats: unknown type %d \n", type);
	    break;
	case SHOW_CURRENT:
	    stat1 = current_user.tm->clid;
	    break;
	case SHOW_NEXT_LEAGUE:
	    stat1 = league_cup_get_next_clid(stat1);
	    while(stat1 >= ID_CUP_START)
		stat1 = league_cup_get_next_clid(stat1);
	    break;
	case SHOW_PREVIOUS_LEAGUE:
	    stat1 = league_cup_get_previous_clid(stat1);
	    while(stat1 >= ID_CUP_START)
		stat1 = league_cup_get_previous_clid(stat1);
	    break;
    }

    treeview_show_league_stats(stat1);
}

/** Show the appropriate season history page in the right treeview. */
void
callback_show_season_history(gint type)
{
    switch(type)
    {
	default:
	    g_warning("callback_show_season_history: unknown type %d \n", type);
	    break;
	case SHOW_CURRENT:
	    stat1 = -1;
	    stat2 = season - 2;
	    break;
	case SHOW_NEXT_LEAGUE:
	    if(stat1 == -1)
		stat1 = 0;
	    else if(stat1 == ligs->len - 1)
		stat1 = -1;
	    else
		stat1++;
	    break;
	case SHOW_PREVIOUS_LEAGUE:
	    if(stat1 == -1)
		stat1 = ligs->len - 1;
	    else if(stat1 == 0)
		stat1 = -1;
	    else
		stat1--;
	    break;
	case SHOW_NEXT:
	    stat2 = (stat2 + 1) % (season - 1);
	    break;
	case SHOW_PREVIOUS:
	    stat2 = (stat2 == 0) ? season - 2 : stat2 - 1;
	    break;
    }
    
    treeview_show_season_history(stat1, stat2);
}
