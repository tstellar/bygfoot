#include "file.h"
#include "misc.h"
#include "team.h"
#include "user.h"
#include "variables.h"
#include "xml.h"
#include "xml_loadsave_live_game.h"
#include "xml_loadsave_users.h"

enum
{
    TAG_USERS = TAG_START_USERS,
    TAG_USER,
    TAG_USER_COUNTER,
    TAG_USER_MONEY,
    TAG_USER_DEBT,
    TAG_USER_MONEY_INS,
    TAG_USER_MONEY_IN,
    TAG_USER_MONEY_OUTS,
    TAG_USER_MONEY_OUT,
    TAG_USER_SCOUT,
    TAG_USER_PHYSIO,
    TAG_END
};

gint state, idx_mon_in, idx_mon_out, idx, team_clid;
User new_user;

void
xml_loadsave_users_start_element (GMarkupParseContext *context,
				    const gchar         *element_name,
				    const gchar        **attribute_names,
				    const gchar        **attribute_values,
				    gpointer             user_data,
				    GError             **error)
{
    gint i;
    gint tag = xml_get_tag_from_name(element_name);
    gboolean valid_tag = FALSE;

    for(i=TAG_USERS;i<TAG_END;i++)
	if(tag == i)
	{
	    state = i;
	    valid_tag = TRUE;
	}

    for(i=TAG_NAME;i<=TAG_ROUND;i++)
	if(tag == i)
	{
	    state = i;
	    valid_tag = TRUE;
	}

    if(state == TAG_USER)
    {
	new_user = user_new();
	idx = idx_mon_out = idx_mon_in = 0;
    }

    if(state == TAG_USER_MONEY_OUTS ||
       state == TAG_USER_MONEY_INS)
	idx = 0;

    if(!valid_tag)
	g_warning("xml_loadsave_users_start_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

void
xml_loadsave_users_end_element    (GMarkupParseContext *context,
				     const gchar         *element_name,
				     gpointer             user_data,
				     GError             **error)
{
    gint tag = xml_get_tag_from_name(element_name);
    
    if(tag == TAG_USER)
    {
	state = TAG_USERS;
	g_array_append_val(users, new_user);
    }
    else if(tag == TAG_USER_COUNTER ||
	    tag == TAG_USER_MONEY ||
	    tag == TAG_USER_DEBT ||
	    tag == TAG_USER_MONEY_INS ||
	    tag == TAG_USER_MONEY_OUTS ||
	    tag == TAG_USER_SCOUT ||
	    tag == TAG_USER_PHYSIO ||
	    tag == TAG_NAME ||
	    tag == TAG_TEAM_CLID ||
	    tag == TAG_TEAM_ID)
    {
	state = TAG_USER;
	if(tag == TAG_USER_COUNTER)
	    idx++;
	else if(tag == TAG_USER_MONEY_OUTS)
	    idx_mon_out++;
	else if(tag == TAG_USER_MONEY_INS)
	    idx_mon_in++;
    }
    else if(tag == TAG_USER_MONEY_OUT)
    {
	state = TAG_USER_MONEY_OUTS;
	idx++;
    }
    else if(tag == TAG_USER_MONEY_IN)
    {
	state = TAG_USER_MONEY_INS;
	idx++;
    }
    else if(tag != TAG_USERS)
	g_warning("xml_loadsave_users_end_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

void
xml_loadsave_users_text         (GMarkupParseContext *context,
				   const gchar         *text,
				   gsize                text_len,  
				   gpointer             user_data,
				   GError             **error)
{
    gchar buf[SMALL];
    gint int_value = -1;

    strncpy(buf, text, text_len);
    buf[text_len] = '\0';

    int_value = (gint)g_ascii_strtod(buf, NULL);

    if(state == TAG_NAME)
	g_string_printf(new_user.name, "%s", buf);
    else if(state == TAG_TEAM_CLID)
	team_clid = int_value;
    else if(state == TAG_TEAM_ID)
	new_user.tm = team_get_pointer_from_ids(team_clid, int_value);
    else if(state == TAG_USER_MONEY)
	new_user.money = int_value;
    else if(state == TAG_USER_DEBT)
	new_user.debt = int_value;
    else if(state == TAG_USER_SCOUT)
	new_user.scout = int_value;
    else if(state == TAG_USER_PHYSIO)
	new_user.physio = int_value;
    else if(state == TAG_USER_COUNTER)
	new_user.counters[idx] = int_value;
    else if(state == TAG_USER_MONEY_IN)
	new_user.money_in[idx_mon_in][idx] = int_value;
    else if(state == TAG_USER_MONEY_OUT)
	new_user.money_out[idx_mon_out][idx] = int_value;
}

void
xml_loadsave_users_read(const gchar *dirname, const gchar *basename)
{
    GMarkupParser parser = {xml_loadsave_users_start_element,
			    xml_loadsave_users_end_element,
			    xml_loadsave_users_text, NULL, NULL};
    GMarkupParseContext *context;
    gchar *file_contents;
    guint length;
    GError *error = NULL;
    gchar file[SMALL];

    sprintf(file, "%s/%s___users.xml", dirname, basename);

    context = 
	g_markup_parse_context_new(&parser, 0, NULL, NULL);

    if(!g_file_get_contents(file, &file_contents, &length, &error))
    {
	g_warning("xml_loadsave_users_read: error reading file %s\n", file);
	misc_print_error(&error, TRUE);
    }

    if(g_markup_parse_context_parse(context, file_contents, length, &error))
    {
	g_markup_parse_context_end_parse(context, NULL);
	g_markup_parse_context_free(context);
	g_free(file_contents);
    }
    else
    {
	g_warning("xml_loadsave_users_read: error parsing file %s\n", file);
	misc_print_error(&error, TRUE);
    }
}

void
xml_loadsave_users_write(const gchar *prefix)
{
    gint i, j, k;
    gchar buf[SMALL];
    FILE *fil = NULL;

    for(i=0;i<users->len;i++)
    {
	sprintf(buf, "%s___user_%02d_options", prefix, i);
	file_save_opt_file(buf, &usr(i).options);

	sprintf(buf, "%s___user_%02d_live_game.xml", prefix, i);
	xml_loadsave_live_game_write(buf, &usr(i).live_game);
    }

    sprintf(buf, "%s___users.xml", prefix);
    file_my_fopen(buf, "w", &fil, TRUE);

    fprintf(fil, "<_%d>\n", TAG_USERS);

    for(i=0;i<users->len;i++)
    {
	fprintf(fil, "<_%d>\n", TAG_USER);

	xml_write_g_string(fil, usr(i).name, TAG_NAME, I1);
	xml_write_int(fil, usr(i).tm->clid, TAG_TEAM_CLID, I1);
	xml_write_int(fil, usr(i).tm->id, TAG_TEAM_ID, I1);
	xml_write_int(fil, usr(i).money, TAG_USER_MONEY, I1);
	xml_write_int(fil, usr(i).debt, TAG_USER_DEBT, I1);
	xml_write_int(fil, usr(i).scout, TAG_USER_SCOUT, I1);
	xml_write_int(fil, usr(i).physio, TAG_USER_PHYSIO, I1);

	for(j=0;j<COUNT_USER_END;j++)
	    xml_write_int(fil, usr(i).counters[j], TAG_USER_COUNTER, I1);

	for(j=0;j<2;j++)
	{
	    fprintf(fil, "%s<_%d>\n", I1, TAG_USER_MONEY_INS);
	    for(k=0;k<MON_IN_END;k++)
		xml_write_int(fil, usr(i).money_in[j][k], TAG_USER_MONEY_IN, I2);	    
	    fprintf(fil, "%s</_%d>\n", I1, TAG_USER_MONEY_INS);
	}

	for(j=0;j<2;j++)
	{
	    fprintf(fil, "%s<_%d>\n", I1, TAG_USER_MONEY_OUTS);
	    for(k=0;k<MON_OUT_END;k++)
		xml_write_int(fil, usr(i).money_out[j][k], TAG_USER_MONEY_OUT, I2);	    
	    fprintf(fil, "%s</_%d>\n", I1, TAG_USER_MONEY_OUTS);
	}

	fprintf(fil, "</_%d>\n", TAG_USER);
    }

    fprintf(fil, "</_%d>\n", TAG_USERS);

    fclose(fil);
}
