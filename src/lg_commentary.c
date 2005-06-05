#include "free.h"
#include "lg_commentary.h"
#include "live_game.h"
#include "misc.h"
#include "option.h"
#include "player.h"
#include "variables.h"

/** Generate commentary for the live game event in the unit.
    @param unit The unit we comment.
    @param fix The fixture being calculated. */
void
lg_commentary_generate(LiveGameUnit *unit, const Fixture *fix)
{
    gchar *strings[LG_TOKEN_END][2];
    GPtrArray *commentaries = NULL;
    gchar buf[SMALL];
    gint i;

    lg_commentary_set_strings(strings, unit, fix);

    commentaries = lg_commentary_get_commentaries(unit->event.type);

    for(i=0;i<commentaries->len;i++)
    {
	if(lg_commentary_replace_tokens(
	       ((GString*)g_ptr_array_index(commentaries, i))->str, strings, buf))
	    break;
    }

    if(i == commentaries->len)
    {
	g_warning("lg_commentary_generate: didn't find fitting commentary for unit type %d \n",
		  unit->event.type);
	strcpy(buf, "FIXME!");
    }

    unit->event.commentary = g_string_new(buf);

    for(i=LG_TOKEN_ATTENDANCE;i<LG_TOKEN_END;i++)
	g_free(strings[i][1]);

    free_g_string_array(&commentaries);
}

/** Try to replace all special tokens in the string and write the result to dest.
    @param string The commentary string with tokens.
    @param strings The replacements of the tokens.
    @param dest The destination string.
    @return TRUE if we could replace all tokens, FALSE otherwise. */
gboolean
lg_commentary_replace_tokens(const gchar *string, gchar *strings[][2], gchar *dest)
{
    gint i;

    strcpy(dest, string);

    for(i=0;i<LG_TOKEN_END;i++)
    {
	if(query_misc_string_contains(dest, strings[i][0]))
	{
	    if(strings[i][1] == NULL)
		return FALSE;
	    else
		misc_string_replace_token(dest, strings[i][0], strings[i][1]);
	}
    }

    return TRUE;
}

/** Get an array of commentary strings with tokens depending on
    the live game event. */
GPtrArray*
lg_commentary_get_commentaries(gint event_type)
{
    GPtrArray *commentaries = g_ptr_array_new();

    if(event_type == LIVE_GAME_EVENT_STYLE_CHANGE_ALL_OUT_DEFEND ||
       event_type == LIVE_GAME_EVENT_STYLE_CHANGE_DEFEND ||
       event_type == LIVE_GAME_EVENT_STYLE_CHANGE_BALANCED ||
       event_type == LIVE_GAME_EVENT_STYLE_CHANGE_ATTACK ||
       event_type == LIVE_GAME_EVENT_STYLE_CHANGE_ALL_OUT_ATTACK)
	g_ptr_array_add(commentaries, g_string_new(_("TT has changed their style to EX")));
    else if(event_type == LIVE_GAME_EVENT_BOOST_CHANGE_ANTI ||
	    event_type == LIVE_GAME_EVENT_BOOST_CHANGE_OFF ||
	    event_type == LIVE_GAME_EVENT_BOOST_CHANGE_ON)
	g_ptr_array_add(commentaries, g_string_new(_("TT has changed their boost to EX")));
    else
    {
	switch(event_type)
	{
	    default:
		g_warning("lg_commentary_get_commentaries: unknown event type %d. \n", event_type);
		break;
	    case LIVE_GAME_EVENT_GENERAL:
		g_ptr_array_add(commentaries, g_string_new(_("P2 feeds the ball through to P1")));
		g_ptr_array_add(commentaries, g_string_new(_("P2 threads a great ball to P1 ")));
		g_ptr_array_add(commentaries, g_string_new(_("Lovely pass from P2 releases P1")));
		g_ptr_array_add(commentaries, g_string_new(_("Long floating pass from P2 finds P1")));
		g_ptr_array_add(commentaries, g_string_new(_("Good passing move across the pitch and down the right")));
		g_ptr_array_add(commentaries, g_string_new(_("Quick passing move down the left")));
		g_ptr_array_add(commentaries, g_string_new(_("P2 cross controlled by P1 who slips past his man")));
		g_ptr_array_add(commentaries, g_string_new(_("Another decent move down the wing")));
		g_ptr_array_add(commentaries, g_string_new(_("P1 latches onto P2's pass")));
		g_ptr_array_add(commentaries, g_string_new(_("P2 needles ball through players and finds P1")));
		g_ptr_array_add(commentaries, g_string_new(_("P2 controls it with his chest and passes to P1")));
		g_ptr_array_add(commentaries, g_string_new(_("P2 heads the ball towards P1")));
		g_ptr_array_add(commentaries, g_string_new(_("P1 twisting and turning again")));
		g_ptr_array_add(commentaries, g_string_new(_("P1 rips through the midfield")));
		g_ptr_array_add(commentaries, g_string_new(_("P1 spins past his marker")));
		break;
	    case LIVE_GAME_EVENT_LOST_POSSESSION:
		g_ptr_array_add(commentaries, g_string_new(_("P1 wins the ball with a perfectly timed challenge")));
		g_ptr_array_add(commentaries, g_string_new(_("Bad control by P2 lets P1 steal the ball off him")));
		g_ptr_array_add(commentaries, g_string_new(_("P1 takes the ball away from P2 with a sliding tackle")));
		g_ptr_array_add(commentaries, g_string_new(_("Loose ball falls to P1")));
		break;
	    case LIVE_GAME_EVENT_FOUL:
		g_ptr_array_add(commentaries, g_string_new(_("Oww, nasty tackle on P1")));
		g_ptr_array_add(commentaries, g_string_new(_("Surely that's a sending off offence")));
		g_ptr_array_add(commentaries, g_string_new(_("P2 knocks over P1 with a double footed lunge")));
		g_ptr_array_add(commentaries, g_string_new(_("Crunching challenge on P1")));
		g_ptr_array_add(commentaries, g_string_new(_("P1 gets his legs taken away from him by P2")));
		g_ptr_array_add(commentaries, g_string_new(_("Slicing challenge by P2 on P1")));
		g_ptr_array_add(commentaries, g_string_new(_("Vicious tackle from P2 leaves P1 lying on the floor in agony")));
		g_ptr_array_add(commentaries, g_string_new(_("Ouch that's got to hurt")));
		g_ptr_array_add(commentaries, g_string_new(_("Extremely high boot from P2 sends P1 to the ground")));
		break;
	    case LIVE_GAME_EVENT_SCORING_CHANCE:
		g_ptr_array_add(commentaries, g_string_new(_("P1 charges towards goal and shoots!")));
		g_ptr_array_add(commentaries, g_string_new(_("P1 spins and shoots")));
		g_ptr_array_add(commentaries, g_string_new(_("Long ball from P2 finds P1 at far post")));
		g_ptr_array_add(commentaries, g_string_new(_("P1 can't quite get it under control but his miss kick heads toward goal")));
		g_ptr_array_add(commentaries, g_string_new(_("P1 shoots from 22 yards looks like it's going wide")));
		g_ptr_array_add(commentaries, g_string_new(_("P1 dribbles into the box, turns inside the defence and fires")));
		g_ptr_array_add(commentaries, g_string_new(_("P1 cuts in from the left flank and shoots")));
		g_ptr_array_add(commentaries, g_string_new(_("P1 tries his luck from 30 yards")));
		g_ptr_array_add(commentaries, g_string_new(_("P1 arrows shot towards top left corner")));
		g_ptr_array_add(commentaries, g_string_new(_("P1 shoots from close range")));
		g_ptr_array_add(commentaries, g_string_new(_("Great ball from P2 sends P1 one on one with the goalkeeper")));
		g_ptr_array_add(commentaries, g_string_new(_("P1's overhead kick heads towards the top right hand corner")));
		g_ptr_array_add(commentaries, g_string_new(_("P1 spins past two defenders, wrong foots the keeper and back heels the ball towards goal")));
		g_ptr_array_add(commentaries, g_string_new(_("P2 lays the ball off to P1 who sends a rocket of a shot towards goal")));
		break;
	    case LIVE_GAME_EVENT_GOAL:
		g_ptr_array_add(commentaries, g_string_new(_("Goal!!!")));
		g_ptr_array_add(commentaries, g_string_new(_("And the ball is smacked into the back of the net!!!")));
		g_ptr_array_add(commentaries, g_string_new(_("P1 scores!!!")));
		g_ptr_array_add(commentaries, g_string_new(_("And the ball flies straight past P2 and into the back of the net!!!")));
		g_ptr_array_add(commentaries, g_string_new(_("The ball flies past P2 and it's a goal!!!")));
		g_ptr_array_add(commentaries, g_string_new(_("P1 wheels away in celebration after a great goal sent the fans wild")));
		g_ptr_array_add(commentaries, g_string_new(_("P2's despairing dive can't keep P1's shot out and the ball is in the back of the net!!!")));
		g_ptr_array_add(commentaries, g_string_new(_("The dance ignites the fans, P1 celebrates in his own special fashion")));
		break;
	    case LIVE_GAME_EVENT_MISSED:
		g_ptr_array_add(commentaries, g_string_new(_("P1 fired wide!")));
		g_ptr_array_add(commentaries, g_string_new(_("P1 should have squared the ball went alone and lashed wide")));
		g_ptr_array_add(commentaries, g_string_new(_("Defender clears")));
		g_ptr_array_add(commentaries, g_string_new(_("P1 curls ball over")));
		g_ptr_array_add(commentaries, g_string_new(_("Defender back to block just as it seemed to open up")));
		g_ptr_array_add(commentaries, g_string_new(_("P1 smacks into row Z")));
		g_ptr_array_add(commentaries, g_string_new(_("And the shot creeps just past the post")));
		break;
	    case LIVE_GAME_EVENT_SAVE:
		g_ptr_array_add(commentaries, g_string_new(_("P2's fingertips flicks it wide")));
		g_ptr_array_add(commentaries, g_string_new(_("P2 forced to tip P1's shot over the crossbar")));
		g_ptr_array_add(commentaries, g_string_new(_("Great pace shown by P1 to get to the ball first but P2 produced superb block tackle")));
		g_ptr_array_add(commentaries, g_string_new(_("Low shot taken well to his left by P2")));
		g_ptr_array_add(commentaries, g_string_new(_("P2 makes a superb reflex save to tip wide")));
		g_ptr_array_add(commentaries, g_string_new(_("P2 runs at P1 and blocks shot with ankle")));
		g_ptr_array_add(commentaries, g_string_new(_("Easy save for the goalkeeper")));
		g_ptr_array_add(commentaries, g_string_new(_("Caught well by P2")));
		break;
	    case LIVE_GAME_EVENT_START_MATCH:
		g_ptr_array_add(commentaries, g_string_new(_("It's a crowd of AT here as the whistle blows")));
		g_ptr_array_add(commentaries, g_string_new(_("We're under way")));
		g_ptr_array_add(commentaries, g_string_new(_("The two teams are greeted by a wall of noise as they take to the pitch")));
		g_ptr_array_add(commentaries, g_string_new(_("The crowd roars as the players run out from the tunnel to start the match")));
		g_ptr_array_add(commentaries, g_string_new(_("A cloudless day, gives the impression this match is going to sparkle")));
		g_ptr_array_add(commentaries, g_string_new(_("AT fans can't be wrong, this is the match we have all come to watch")));
		g_ptr_array_add(commentaries, g_string_new(_("The referee gets the nod from the linesman and we are away")));
		g_ptr_array_add(commentaries, g_string_new(_("The clash of the titans kicks off")));
		g_ptr_array_add(commentaries, g_string_new(_("The referees are ready, players set, the battle begins")));
		break;
	    case LIVE_GAME_EVENT_HALF_TIME:
		g_ptr_array_add(commentaries, g_string_new(_("It's half time")));
		g_ptr_array_add(commentaries, g_string_new(_("It's a RE at half time.")));
		g_ptr_array_add(commentaries, g_string_new(_("It's a RE at half time, TL's manager has to find something special to spur his team on.")));
		g_ptr_array_add(commentaries, g_string_new(_("TL heads into the tunnel to await the managers scolding")));
		g_ptr_array_add(commentaries, g_string_new(_("TW are just 45 minutes away from a stunning victory")));
		g_ptr_array_add(commentaries, g_string_new(_("TL's manager cops a rollicking from the fans as he turns from the dugout")));
		g_ptr_array_add(commentaries, g_string_new(_("There are two teams out there and only one of them is playing football")));
		break;
	    case LIVE_GAME_EVENT_EXTRA_TIME:
		g_ptr_array_add(commentaries, g_string_new(_("After a close match the game heads into extra time")));
		g_ptr_array_add(commentaries, g_string_new(_("It's still RE: extra time")));
		g_ptr_array_add(commentaries, g_string_new(_("After MI minutes it's still RE: extra time")));
		g_ptr_array_add(commentaries, g_string_new(_("Who will breakdown first, extra time it is")));
		g_ptr_array_add(commentaries, g_string_new(_("It's down to determination and fitness now as we head into extra time")));
		break;
	    case LIVE_GAME_EVENT_PENALTIES:
		g_ptr_array_add(commentaries, g_string_new(_("Who would have thought MI minutes ago it would come down to this")));
		g_ptr_array_add(commentaries, g_string_new(_("And now we have the tense drama of a penalty shoot out")));
		g_ptr_array_add(commentaries, g_string_new(_("Two teams, equall till now in every respect,face off in penalties")));
		g_ptr_array_add(commentaries, g_string_new(_("The teams cant do it so now it's one on one. Striker against goalie")));
		break;
	    case LIVE_GAME_EVENT_END_MATCH:
		g_ptr_array_add(commentaries, g_string_new(_("The match is over!")));
		g_ptr_array_add(commentaries, g_string_new(_("TW are triumphant!!!")));
		g_ptr_array_add(commentaries, g_string_new(_("The fans boo TL off the pitch")));
		g_ptr_array_add(commentaries, g_string_new(_("After MI minutes the match ends with a RE")));
		g_ptr_array_add(commentaries, g_string_new(_("The referee checks his watch and blows for full time")));
		g_ptr_array_add(commentaries, g_string_new(_("Well that's that. It ends RE")));
		g_ptr_array_add(commentaries, g_string_new(_("RE, what an astonishing result for TW")));
		break;
	    case LIVE_GAME_EVENT_OWN_GOAL:
		g_ptr_array_add(commentaries, g_string_new(_("Ohh no, the ball ricocheted off P1 into the net")));
		g_ptr_array_add(commentaries, g_string_new(_("It's a pinball in the penalty box, finally spinning off P1 across the line")));
		g_ptr_array_add(commentaries, g_string_new(_("Oh that's a great shot and it's deflected off P1 and into the back of the net")));
		g_ptr_array_add(commentaries, g_string_new(_("P1 passes the ball back to his goalie, oh no the keepers missed it! It's an own goal by P1!")));
		g_ptr_array_add(commentaries, g_string_new(_("That little missunderstanding has cost TT a goal")));
		break;
	    case LIVE_GAME_EVENT_HEADER:
		g_ptr_array_add(commentaries, g_string_new(_("P2 chips the ball towards P1 who heads the ball towards goal")));
		g_ptr_array_add(commentaries, g_string_new(_("Diving header by P1")));
		g_ptr_array_add(commentaries, g_string_new(_("He just floated in midair for ages then thunder heads it towards home")));
		break;
	    case LIVE_GAME_EVENT_POST:
		g_ptr_array_add(commentaries, g_string_new(_("Sweet strike by P1, hits the post")));
		g_ptr_array_add(commentaries, g_string_new(_("And the shot glances wide off the post")));
		g_ptr_array_add(commentaries, g_string_new(_("I'm suprised the post has managed to withstand that cracking shot")));
		break;
	    case LIVE_GAME_EVENT_CROSS_BAR:
		g_ptr_array_add(commentaries, g_string_new(_("Ball dinked over P2 but hit crossbar and bounced to safety")));
		g_ptr_array_add(commentaries, g_string_new(_("The ball smacks into the crossbar with P2 stranded")));
		g_ptr_array_add(commentaries, g_string_new(_("P2 seemed to be everywhere at once, finally tipping the ball off the crossbar")));
		break;
	    case LIVE_GAME_EVENT_PENALTY:
		g_ptr_array_add(commentaries, g_string_new(_("P1 steps up to take the penalty")));
		g_ptr_array_add(commentaries, g_string_new(_("P1 has a chance to score with a penalty kick")));
		g_ptr_array_add(commentaries, g_string_new(_("P1 has missed a penalty before but he now has a chance to redeem himself")));
		g_ptr_array_add(commentaries, g_string_new(_("You have to bury the nerves, swallow and concentrate when your taking penalties")));
		break;
	    case LIVE_GAME_EVENT_FREE_KICK:
		g_ptr_array_add(commentaries, g_string_new(_("Short free-kick from P1")));
		g_ptr_array_add(commentaries, g_string_new(_("P1 drives free kick low and hard")));
		g_ptr_array_add(commentaries, g_string_new(_("P1 takes free-kick quickly and curls around wall")));
		g_ptr_array_add(commentaries, g_string_new(_("P1 curls the free kick over the wall")));
		g_ptr_array_add(commentaries, g_string_new(_("Free kick: P1 plays the ball out wide")));
		g_ptr_array_add(commentaries, g_string_new(_("Free kick: P1 hits a long ball up the pitch")));
		g_ptr_array_add(commentaries, g_string_new(_("Free kick whipped across the face of goal by P1")));
		break;
	    case LIVE_GAME_EVENT_FOUL_YELLOW:
		g_ptr_array_add(commentaries, g_string_new(_("P2 for dangerous slide tackle on P1")));
		g_ptr_array_add(commentaries, g_string_new(_("P2 for foul on P1")));
		g_ptr_array_add(commentaries, g_string_new(_("P2 for kicking ball away ")));
		g_ptr_array_add(commentaries, g_string_new(_("P2 for time wasting")));
		g_ptr_array_add(commentaries, g_string_new(_("P2 for handball")));
		g_ptr_array_add(commentaries, g_string_new(_("P2 for stopping P1 taking a quick free-kick")));
		g_ptr_array_add(commentaries, g_string_new(_("P2 is booked for repeated dissent")));
		break;
	    case LIVE_GAME_EVENT_FOUL_RED:
		g_ptr_array_add(commentaries, g_string_new(_("P2 is reprimanded for high kicking")));
		g_ptr_array_add(commentaries, g_string_new(_("P2 is given his marching orders for contempt")));
		g_ptr_array_add(commentaries, g_string_new(_("P2 for malicious showing of studs")));
		g_ptr_array_add(commentaries, g_string_new(_("P2 for intentional handball")));
		g_ptr_array_add(commentaries, g_string_new(_("P2 is sent off for stopping a clear goal scoring opportunity")));
		g_ptr_array_add(commentaries, g_string_new(_("P2 has been red carded for punching P1")));
		g_ptr_array_add(commentaries, g_string_new(_("P2 for swearing at opposition fans")));
		break;
	    case LIVE_GAME_EVENT_FOUL_RED_INJURY:
		g_ptr_array_add(commentaries, g_string_new(_("P2 fouls and the stretcher bearers come racing on with P1 lying in agony on the turf")));
		g_ptr_array_add(commentaries, g_string_new(_("Foul by P2; P1 seems to be injured... he might have to be substituted")));
		break;
	    case LIVE_GAME_EVENT_SEND_OFF:
		g_ptr_array_add(commentaries, g_string_new(_("P1 has been sent off")));
		break;
	    case LIVE_GAME_EVENT_INJURY:
		g_ptr_array_add(commentaries, g_string_new(_("P1 will have to go off after what seems an ankle injury")));
		g_ptr_array_add(commentaries, g_string_new(_("P1 looks like he's badly injured")));
		break;
	    case LIVE_GAME_EVENT_TEMP_INJURY:
		g_ptr_array_add(commentaries, g_string_new(_("P1 has pulled a muscle but should be able to continue")));
		g_ptr_array_add(commentaries, g_string_new(_("P1 has picked up a slight knock")));
		g_ptr_array_add(commentaries, g_string_new(_("P1 has a large cut above his left eye but it has been patched up and he will be able to continue ")));
		break;
	    case LIVE_GAME_EVENT_STADIUM_BREAKDOWN:
		g_ptr_array_add(commentaries, g_string_new(_("TH is undergoing stadium problems")));
		g_ptr_array_add(commentaries, g_string_new(_("The floodlights have just gone out")));
		break;
	    case LIVE_GAME_EVENT_STADIUM_RIOTS:
		g_ptr_array_add(commentaries, g_string_new(_("The peasants are revolting")));
		g_ptr_array_add(commentaries, g_string_new(_("And the fans are throwing flares onto the pitch")));
		g_ptr_array_add(commentaries, g_string_new(_("The riot police have moved in to stop fighting between sets of oppostion supporters")));
		g_ptr_array_add(commentaries, g_string_new(_("A fan has managed to get onto the pitch and has begun to attack the referee")));
		g_ptr_array_add(commentaries, g_string_new(_("The fans pour onto the pitch in a full scale pitch invasion")));
		break;
	    case LIVE_GAME_EVENT_STADIUM_FIRE:
		g_ptr_array_add(commentaries, g_string_new(_("Fire consumes TT's stadium")));
		g_ptr_array_add(commentaries, g_string_new(_("Away fans have set fire to the north stand")));
		g_ptr_array_add(commentaries, g_string_new(_("A flare chucked onto the touchline has started a massive blaze")));
		g_ptr_array_add(commentaries, g_string_new(_("A speaker has gone up in flames and the blaze spreading around the entire stadium")));
		break;
	    case LIVE_GAME_EVENT_SUBSTITUTION:
		g_ptr_array_add(commentaries, g_string_new(_("TT make a substitution, P1 comes on to replace P2")));
		g_ptr_array_add(commentaries, g_string_new(_("P2 goes off to applause from the fans, P1 replaces him")));
		g_ptr_array_add(commentaries, g_string_new(_("P2 is booed off the pitch as he is replaced by P1")));
		break;
	    case LIVE_GAME_EVENT_STRUCTURE_CHANGE:
		g_ptr_array_add(commentaries, g_string_new(_("TT has changed their structure to EX")));
		break;
	}
    }

    return misc_randomise_g_pointer_array(commentaries);
}

/** Get the strings corresponding to the commentary tokens. */
void
lg_commentary_set_strings(gchar *strings[][2], const LiveGameUnit *unit, const Fixture *fix)
{
    gchar buf[SMALL];

    strings[LG_TOKEN_TEAM_HOME][0] = const_str("string_lg_commentary_token_team_home");
    strings[LG_TOKEN_TEAM_HOME][1] = fix->teams[0]->name->str;

    strings[LG_TOKEN_TEAM_AWAY][0] = const_str("string_lg_commentary_token_team_away");
    strings[LG_TOKEN_TEAM_AWAY][1] = fix->teams[1]->name->str;

    strings[LG_TOKEN_TEAM_IN_POSS][0] = const_str("string_lg_commentary_token_team_in_poss");
    strings[LG_TOKEN_TEAM_IN_POSS][1] = fix->teams[unit->possession]->name->str;


    strings[LG_TOKEN_TEAM_NOT_IN_POSS][0] = const_str("string_lg_commentary_token_team_not_in_poss");
    strings[LG_TOKEN_TEAM_NOT_IN_POSS][1] = fix->teams[!unit->possession]->name->str;

    strings[LG_TOKEN_TEAM_LOSING][0] = const_str("string_lg_commentary_token_team_losing");
    strings[LG_TOKEN_TEAM_LOSING][1] = (unit->result[0] == unit->result[1]) ? NULL :
	fix->teams[(unit->result[0] > unit->result[1])]->name->str;
	
    strings[LG_TOKEN_TEAM_WINNING][0] = const_str("string_lg_commentary_token_team_winning");
    strings[LG_TOKEN_TEAM_WINNING][1] = (unit->result[0] == unit->result[1]) ? NULL :
	fix->teams[(unit->result[0] < unit->result[1])]->name->str;

    strings[LG_TOKEN_TEAM][0] = const_str("string_lg_commentary_token_team");
    strings[LG_TOKEN_TEAM][1] = (unit->event.team == -1) ?
	NULL : fix->teams[unit->event.team]->name->str;

    strings[LG_TOKEN_PLAYER1][0] = const_str("string_lg_commentary_token_player1");
    strings[LG_TOKEN_PLAYER1][1] = (unit->event.player == -1) ?
	NULL : lg_commentary_get_player_name(unit, fix, FALSE);

    strings[LG_TOKEN_PLAYER2][0] = const_str("string_lg_commentary_token_player2");
    strings[LG_TOKEN_PLAYER2][1] = (unit->event.player2 == -1) ?
	NULL : lg_commentary_get_player_name(unit, fix, TRUE);
 
    strings[LG_TOKEN_ATTENDANCE][0] = const_str("string_lg_commentary_token_attendance");
    misc_print_grouped_int(fix->attendance, buf, FALSE);
    strings[LG_TOKEN_ATTENDANCE][1] = g_strdup(buf);
    
    strings[LG_TOKEN_RESULT][0] = const_str("string_lg_commentary_token_result");
    sprintf(buf, "%d : %d", unit->result[0], unit->result[1]);
    strings[LG_TOKEN_RESULT][1] = g_strdup(buf);

    strings[LG_TOKEN_MINUTE][0] = const_str("string_lg_commentary_token_minute");
    sprintf(buf, "%d", live_game_unit_get_minute(unit));
    strings[LG_TOKEN_MINUTE][1] = g_strdup(buf);

    strings[LG_TOKEN_EXTRA][0] = const_str("string_lg_commentary_token_extra");
    strings[LG_TOKEN_EXTRA][1] = lg_commentary_get_extra_data(unit, fix);
}

/** Return the name of a player involved in the unit (or NULL). 
    @param player Which player we look for. */
gchar*
lg_commentary_get_player_name(const LiveGameUnit *unit, const Fixture *fix, gboolean player2)
{
    gchar *return_value = NULL;

    if(unit->event.type == LIVE_GAME_EVENT_GENERAL)
	return_value = (player2) ?
	    player_get_last_name(player_of_id_team(fix->teams[unit->possession],
						   unit->event.player2)->name->str) :
	    player_get_last_name(player_of_id_team(fix->teams[unit->possession],
						   unit->event.player)->name->str);
    else if(unit->event.type == LIVE_GAME_EVENT_LOST_POSSESSION)
    {
	return_value = (player2) ?
	    player_get_last_name(player_of_id_team(fix->teams[!unit->possession], 
						   unit->event.player2)->name->str):
	    player_get_last_name(player_of_id_team(fix->teams[unit->possession], 
						   unit->event.player)->name->str);
    }
    else if(unit->event.type == LIVE_GAME_EVENT_SCORING_CHANCE ||
	    unit->event.type == LIVE_GAME_EVENT_HEADER ||
	    unit->event.type == LIVE_GAME_EVENT_PENALTY ||
	    unit->event.type == LIVE_GAME_EVENT_FREE_KICK)
    {	
	return_value = (player2) ?
	    player_get_last_name(player_of_id_team(fix->teams[unit->possession],
						   unit->event.player2)->name->str) :
	    player_get_last_name(player_of_id_team(fix->teams[unit->possession],
						   unit->event.player)->name->str);
    }
    else if(unit->event.type == LIVE_GAME_EVENT_GOAL ||
	    unit->event.type == LIVE_GAME_EVENT_MISSED ||
	    unit->event.type == LIVE_GAME_EVENT_SAVE ||
	    unit->event.type == LIVE_GAME_EVENT_POST ||
	    unit->event.type == LIVE_GAME_EVENT_CROSS_BAR)
    {
	return_value = (player2) ?
	    player_get_last_name(player_of_id_team(fix->teams[!unit->possession], 
						   unit->event.player2)->name->str) :
	    player_get_last_name(player_of_id_team(fix->teams[unit->possession], 
						   unit->event.player)->name->str);
    }
    else if(unit->event.type == LIVE_GAME_EVENT_OWN_GOAL)
	return_value = 
	    player_get_last_name(player_of_id_team(fix->teams[!unit->possession], 
						   unit->event.player)->name->str);
    else if(unit->event.type == LIVE_GAME_EVENT_FOUL ||
	    unit->event.type == LIVE_GAME_EVENT_FOUL_RED ||
	    unit->event.type == LIVE_GAME_EVENT_FOUL_RED_INJURY ||
	    unit->event.type == LIVE_GAME_EVENT_FOUL_YELLOW)
    {
	return_value = (player2) ? 
	    player_get_last_name(player_of_id_team(fix->teams[unit->event.team], 
						   unit->event.player2)->name->str) :
	    player_get_last_name(player_of_id_team(fix->teams[!unit->event.team], 
						   unit->event.player)->name->str);
    }
    else if(unit->event.type == LIVE_GAME_EVENT_SEND_OFF ||
	    unit->event.type == LIVE_GAME_EVENT_INJURY ||
	    unit->event.type == LIVE_GAME_EVENT_TEMP_INJURY ||
	    unit->event.type == LIVE_GAME_EVENT_SUBSTITUTION)
	return_value = (player2) ?
	    player_get_last_name(player_of_id_team(fix->teams[unit->event.team],
						   unit->event.player2)->name->str) :
	    player_get_last_name(player_of_id_team(fix->teams[unit->event.team],
						   unit->event.player)->name->str);
    else
	g_warning("lg_commentary_get_player_name: unknown event type %d\n",
		  unit->event.type);

    return return_value;
}

/** Return some extra data depending on the unit type. */
gchar*
lg_commentary_get_extra_data(const LiveGameUnit *unit, const Fixture *fix)
{
    gchar buf[SMALL];

    switch(unit->event.type)
    {
	default:
	    return NULL;
	    break;
	case LIVE_GAME_EVENT_STRUCTURE_CHANGE:
	    sprintf(buf, "%d",
		    fix->teams[unit->event.team]->structure);
	    break;
	case LIVE_GAME_EVENT_STYLE_CHANGE_ALL_OUT_DEFEND:
	    strcpy(buf, "ALL OUT DEFEND");
	    break;
	case LIVE_GAME_EVENT_STYLE_CHANGE_DEFEND:
	    strcpy(buf, "DEFEND");
	    break;
	case LIVE_GAME_EVENT_STYLE_CHANGE_BALANCED:
	    strcpy(buf, "BALANCED");
	    break;
	case LIVE_GAME_EVENT_STYLE_CHANGE_ATTACK:
	    strcpy(buf, "ATTACK");
	    break;
	case LIVE_GAME_EVENT_STYLE_CHANGE_ALL_OUT_ATTACK:
	    strcpy(buf, "ALL OUT ATTACK");
	    break;
	case LIVE_GAME_EVENT_BOOST_CHANGE_ANTI:
	    strcpy(buf, "ANTI");
	    break;
	case LIVE_GAME_EVENT_BOOST_CHANGE_OFF:
	    strcpy(buf, "OFF");
	    break;
	case LIVE_GAME_EVENT_BOOST_CHANGE_ON:
	    strcpy(buf, "ON");
	    break;
    }

    return g_strdup(buf);
}
