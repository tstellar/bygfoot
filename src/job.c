/*
   job.c

   Bygfoot Football Manager -- a small and simple GTK2-based
   football management game.

   http://bygfoot.sourceforge.net

   Copyright (C) 2005  Gyözö Both (gyboth@bygfoot.com)

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "bet.h"
#include "cup.h"
#include "file.h"
#include "free.h"
#include "job.h"
#include "league.h"
#include "live_game.h"
#include "main.h"
#include "maths.h"
#include "option.h"
#include "start_end.h"
#include "team.h"
#include "transfer.h"
#include "user.h"
#include "variables.h"
#include "xml_country.h"

/** Update the job exchange: remove expired offers and add new ones. */
void
job_update(void)
{
    gint i;
    gint new_offers, int_offers;

    for(i=jobs->len - 1; i >= 0; i--)
    {
	g_array_index(jobs, Job, i).time--;

	if(g_array_index(jobs, Job, i).time <= 0)
	    job_remove(&g_array_index(jobs, Job, i), TRUE);
    }

    if(week % const_int("int_job_update_interval") != 2)
	return;

    new_offers = math_rndi(const_int("int_job_new_offers_lower"),
			   const_int("int_job_new_offers_upper"));
    int_offers = (users->len == 1) ? 
	(gint)rint((gfloat)new_offers *
		   const_float("float_job_international_perc")) : 0;

    for(i=0;i<new_offers - int_offers;i++)
	job_add_new_national();

    job_add_new_international(int_offers);

    /*d*/
/*     for(i=0;i<jobs->len;i++) */
/*     { */
/* 	printf("%d %d %s %s %s %d %d %d\n", */
/* 	       g_array_index(jobs, Job, i).type, */
/* 	       g_array_index(jobs, Job, i).time, */
/* 	       job_get_team(&g_array_index(jobs, Job, i))->name, */
/* 	       g_array_index(jobs, Job, i).league_name, */
/* 	       g_array_index(jobs, Job, i).country_name, */
/* 	       g_array_index(jobs, Job, i).league_layer, */
/* 	       g_array_index(jobs, Job, i).country_rating, */
/* 	       g_array_index(jobs, Job, i).talent_percent); */
/* 	query_job_application_successful(&g_array_index(jobs, Job, i), */
/* 					 &current_user); */
/*     } */
/*     printf("\n"); */
}

/** Add some new international job offers to the job exchange. */
void
job_add_new_international(gint num_of_new)
{
    gint i, k, rndom, idx;
    GPtrArray *country_files = file_get_country_files();
    Country countries[num_of_new];
    Team *tm = NULL;
    League *league = NULL;
    gint team_id = -1;
    Job new_job;

    k = 0;
    for(i=0;i<num_of_new;i++)
    {
	countries[k].leagues = countries[k].cups = NULL;
	countries[k].allcups = NULL;
	countries[k].name = countries[k].symbol = 
	    countries[k].sid = NULL;

	do
	    rndom = math_rndi(0, country_files->len - 1);
	while(g_strrstr((gchar*)g_ptr_array_index(country_files, rndom),
			country.sid));

	idx = job_country_is_in_list(
	    (gchar*)g_ptr_array_index(country_files, rndom), 
	    countries, num_of_new);

	if(idx == -1)
	{
	    idx = k;
	    xml_country_read((gchar*)g_ptr_array_index(country_files, rndom),
			     &countries[k]);
	    k++;
	}

	job_pick_team_from_country(&countries[idx], &tm, &league);

	new_job.country_file = 
	    g_strdup_printf("country_%s.xml", countries[idx].sid);
	new_job.time = math_rndi(const_int("int_job_update_interval") - 1,
				 const_int("int_job_update_interval") + 1);
	new_job.country_name = g_strdup(countries[idx].name);
	new_job.country_rating = countries[idx].rating;
	new_job.league_name = g_strdup(league->name);
	new_job.league_layer = league->layer;

	team_id = job_team_is_in_cup(tm->name);

	if(team_id == -1)
	{
	    team_generate_players_stadium(tm, league->average_talent);
	    g_array_append_val(job_teams, *tm);

	    new_job.team_id = tm->id;
	    new_job.type = JOB_TYPE_INTERNATIONAL;
	}
	else
	{
	    new_job.team_id = team_id;
	    new_job.type = JOB_TYPE_CUP;
	}

	new_job.talent_percent = 
	    (gint)rint((team_get_average_talent(tm) /
			league->average_talent) * 100);

	g_array_append_val(jobs, new_job);
    }

/*     for(i=0;i<k;i++) */
/* 	printf("%d %s\n", i, countries[i].sid); */

    free_gchar_array(&country_files);
}

/** Find out whether the country file is already loaded and part
    of the country array.
    @param len The length of the array. */
gint
job_country_is_in_list(const gchar *country_file, 
		       const Country *countries, gint len)
{
    gint i;

    for(i=0;i<len;i++)
    {
	if(countries[i].sid == NULL)
	    return -1;
	else if(g_strrstr(country_file, countries[i].sid))
	    return i;
    }

    return -1;
}

/** Add a new national job offer to the job exchange. */
void
job_add_new_national(void)
{
    Job new_job;
    League *league = NULL;
    Team *tm = NULL;

    job_pick_team_from_country(&country, &tm, &league);

    new_job.type = JOB_TYPE_NATIONAL;
    new_job.time = math_rndi(const_int("int_job_update_interval") - 1,
			     const_int("int_job_update_interval") + 1);
    new_job.country_file = NULL;
    new_job.country_name = country.name;
    new_job.country_rating = -1;
    new_job.league_name = league->name;
    new_job.league_layer = league->layer;

    new_job.talent_percent = 
	(gint)rint((team_get_average_talent(tm) /
		    league->average_talent) * 100);
    new_job.team_id = tm->id;

    g_array_append_val(jobs, new_job);
}

void
job_pick_team_from_country(const Country *cntry, Team **tm, League **league)
{
    gint i, rndom;
    gint team_lens[cntry->leagues->len];

    team_lens[0] = g_array_index(cntry->leagues, League, 0).teams->len;

    for(i=1;i<cntry->leagues->len;i++)
	team_lens[i] = team_lens[i - 1] + 
	    g_array_index(cntry->leagues, League, i).teams->len;

    do
    {
	rndom = math_rndi(0, team_lens[cntry->leagues->len - 1] - 1);

	for(i=0;i<cntry->leagues->len;i++)
	    if(rndom < team_lens[i])
	    {
		*tm = (i > 0) ?
		    &g_array_index(g_array_index(
				       cntry->leagues, League, i).teams, 
				   Team, rndom - team_lens[i - 1]) :
		    &g_array_index(g_array_index(
				       cntry->leagues, League, i).teams, 
				   Team, rndom);		
		*league = &g_array_index(cntry->leagues, League, i);
		break;
	    }
    }
    while(team_is_user(*tm) != -1 ||
	  job_team_is_on_list((*tm)->id) != -1);
}

/** Find out whether the team with given id is already on the
    job exchange list.
    @return The index in the jobs array if the team is on the list or -1. */
gint
job_team_is_on_list(gint team_id)
{
    gint i;

    for(i=0;i<jobs->len;i++)
	if(g_array_index(jobs, Job, i).team_id == team_id)
	    return i;

    return -1;
}

/** Find out whether the team with given name is participating
    in an international cup (and thus doesn't have to be generated).
    @return The id of the team if found or -1. */
gint
job_team_is_in_cup(const gchar *team_name)
{
    gint i, j;

    for(i=0;i<acps->len;i++)
	for(j=0;j<acp(i)->teams->len;j++)
	    if(strcmp(team_name, 
		      ((Team*)g_ptr_array_index(acp(i)->teams, j))->name) == 0)
		return ((Team*)g_ptr_array_index(acp(i)->teams, j))->id;

    return -1;
}

/** Find the team going with the job offer. */
Team*
job_get_team(const Job *job)
{
    gint i, j;

    if(job->type == JOB_TYPE_NATIONAL)
	return team_of_id(job->team_id);
    else if(job->type == JOB_TYPE_INTERNATIONAL)
    {
	for(i=0;i<job_teams->len;i++)
	    if(g_array_index(job_teams, Team, i).id == job->team_id)
		return &g_array_index(job_teams, Team, i);
    }
    else
    {
	for(i=0;i<acps->len;i++)
	    for(j=0;j<acp(i)->teams->len;j++)
		if(((Team*)g_ptr_array_index(acp(i)->teams, j))->id ==
		   job->team_id)
		    return (Team*)g_ptr_array_index(acp(i)->teams, j);
    }

    main_exit_program(EXIT_POINTER_NOT_FOUND,
		      "job_get_team: team not found (league %s, country %s.",
		      job->league_name, job->country_name);
    
    return NULL;
}

/** Find out whether the user's application for the job gets
    accepted. */
gboolean
query_job_application_successful(const Job *job, const User *user)
{
    gfloat success_needed;
    const Team *tm = job_get_team(job);
    gfloat user_av_skill = team_get_average_skill(user->tm, FALSE),
	job_av_skill = team_get_average_skill(tm, FALSE);

    success_needed = (job_av_skill - user_av_skill) * 
	(gfloat)const_int("int_job_application_points_per_av_skill");

    success_needed += 
	((gfloat)(job->league_layer - league_from_clid(user->tm->clid)->layer) *
	 (gfloat)const_int("int_job_application_points_per_layer"));

    if(job->type != JOB_TYPE_NATIONAL)
    {
	success_needed += 
	    (gfloat)const_int("int_job_application_points_international");

	success_needed +=
	    ((gfloat)(job->country_rating - country.rating) *
	     (gfloat)const_int("int_job_application_points_per_rating"));
    }

    /*d*/
/*     printf("%s avd %.1f ld %d crd %d %.0f\n", tm->name, */
/* 	   job_av_skill - user_av_skill, */
/* 	   job->league_layer - league_from_clid(user->tm->clid)->layer, */
/* 	   job->country_rating - country.rating, */
/* 	   success_needed); */

    return (user->counters[COUNT_USER_SUCCESS] >= success_needed);
}

/** Remove a job from the jobs array.
    @param free Whether to free memory occupied by the job
    and its team. */
void
job_remove(Job *job, gboolean free_tm)
{
    gint i;

    for(i=0;i<jobs->len;i++)
	if(&g_array_index(jobs, Job, i) == job)
	{
	    free_job(job, free_tm);
	    g_array_remove_index(jobs, i);
	    break;
	}
}

/** Change the game so that the country is
    used that's specified in the job. */
void
job_change_country(Job *job)
{
    gint i, j, k;
    Team tm = *(job_get_team(job));
    gint season_temp = season + 1;

    for(i=transfer_list->len - 1;i>=0;i--)
	transfer_remove_player(i);
    
    free_bets(TRUE);

    /* There's only one user (otherwise
       international job offers are disabled). */
    for(i=0;i<2;i++)
    {
	g_array_free(usr(0).bets[i], TRUE);
	usr(0).bets[i] = g_array_new(FALSE, FALSE, sizeof(BetUser));
    }

    live_game_reset(&usr(0).live_game, NULL, FALSE);

    free_country(&country, TRUE);

    xml_country_read(job->country_file, &country);

    stat5 = STATUS_GENERATE_TEAMS;
    for(i=0;i<ligs->len;i++)
	for(j=0;j<lig(i).teams->len;j++)
	    if(strcmp(g_array_index(lig(i).teams, Team, j).name,
		      tm.name) != 0)
		team_generate_players_stadium(&g_array_index(lig(i).teams, Team, j), 0);
	    else
	    {
		tm.id = g_array_index(lig(i).teams, Team, j).id;
		tm.clid = g_array_index(lig(i).teams, Team, j).clid;
		job->team_id = tm.id;
		free_team(&g_array_index(lig(i).teams, Team, j));
		g_array_index(lig(i).teams, Team, j) = tm;

		for(k=0;k<g_array_index(lig(i).teams, Team, j).players->len;k++)
		    g_array_index(g_array_index(lig(i).teams, Team, j).players,
				  Player, k).team = 
			&g_array_index(lig(i).teams, Team, j);
	    }
    stat5 = -1;

    /* Set season to 1 so that some special things
       in the start_new_season function don't get applied. */
    season = 1;
    start_new_season();
    season = season_temp;
}
