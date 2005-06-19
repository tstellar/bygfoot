#include "file.h"
#include "misc.h"
#include "team.h"
#include "transfer.h"
#include "xml.h"
#include "xml_loadsave_transfers.h"

enum
{
    TAG_TRANSFERS = TAG_START_TRANSFERS,
    TAG_TRANSFER,
    TAG_TRANSFER_PLAYER_ID,
    TAG_TRANSFER_TIME,
    TAG_TRANSFER_FEE,
    TAG_TRANSFER_WAGE,
    TAG_TRANSFER_OFFER,
    TAG_TRANSFER_OFFER_TEAM_ID,
    TAG_TRANSFER_OFFER_FEE,
    TAG_TRANSFER_OFFER_WAGE,
    TAG_TRANSFER_OFFER_STATUS,
    TAG_END
};

gint state, feeidx, wageidx;
Transfer new_transfer;
TransferOffer new_offer;

void
xml_loadsave_transfers_start_element (GMarkupParseContext *context,
				      const gchar         *element_name,
				      const gchar        **attribute_names,
				      const gchar        **attribute_values,
				      gpointer             user_data,
				      GError             **error)
{
    gint i;
    gint tag = xml_get_tag_from_name(element_name);
    gboolean valid_tag = FALSE;

    for(i=TAG_TRANSFERS;i<TAG_END;i++)
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

    if(state == TAG_TRANSFER)
    {
	feeidx = wageidx = 0;
	new_transfer.offers = g_array_new(FALSE, FALSE, sizeof(TransferOffer));
    }

    if(!valid_tag)
	g_warning("xml_loadsave_transfers_start_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

void
xml_loadsave_transfers_end_element    (GMarkupParseContext *context,
				     const gchar         *element_name,
				     gpointer             user_data,
				     GError             **error)
{
    gint tag = xml_get_tag_from_name(element_name);
    
    if(tag == TAG_TRANSFER)
    {
	state = TAG_TRANSFERS;
	g_array_append_val(transfer_list, new_transfer);
    }
    else if(tag == TAG_TEAM_ID ||
	    tag == TAG_TRANSFER_PLAYER_ID ||
	    tag == TAG_TRANSFER_TIME ||
	    tag == TAG_TRANSFER_FEE ||
	    tag == TAG_TRANSFER_OFFER ||
	    tag == TAG_TRANSFER_WAGE)
    {
	state = TAG_TRANSFER;
	if(tag == TAG_TRANSFER_FEE)
	    feeidx++;
	else if(tag == TAG_TRANSFER_WAGE)
	    wageidx++;
	else if(tag == TAG_TRANSFER_OFFER)
	    g_array_append_val(new_transfer.offers, new_offer);
    }	    
    else if(tag == TAG_TRANSFER_OFFER_WAGE ||
	    tag == TAG_TRANSFER_OFFER_FEE ||
	    tag == TAG_TRANSFER_OFFER_STATUS ||
	    tag == TAG_TRANSFER_OFFER_TEAM_ID)
	state = TAG_TRANSFER_OFFER;
    else if(tag != TAG_TRANSFERS)
	g_warning("xml_loadsave_transfers_end_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);	
}

void
xml_loadsave_transfers_text         (GMarkupParseContext *context,
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

    if(state == TAG_TEAM_ID)
	new_transfer.tm = team_of_id(int_value);
    else if(state == TAG_TRANSFER_PLAYER_ID)
	new_transfer.id = int_value;
    else if(state == TAG_TRANSFER_TIME)
	new_transfer.time = int_value;
    else if(state == TAG_TRANSFER_FEE)
	new_transfer.fee[feeidx] = int_value;
    else if(state == TAG_TRANSFER_WAGE)
	new_transfer.wage[wageidx] = int_value;
    else if(state == TAG_TRANSFER_OFFER_TEAM_ID)
	new_offer.tm = team_of_id(int_value);
    else if(state == TAG_TRANSFER_OFFER_WAGE)
	new_offer.wage = int_value;
    else if(state == TAG_TRANSFER_OFFER_STATUS)
	new_offer.status = int_value;
    else if(state == TAG_TRANSFER_OFFER_FEE)
	new_offer.fee = int_value;
}

void
xml_loadsave_transfers_read(const gchar *filename)
{
    GMarkupParser parser = {xml_loadsave_transfers_start_element,
			    xml_loadsave_transfers_end_element,
			    xml_loadsave_transfers_text, NULL, NULL};
    GMarkupParseContext *context;
    gchar *file_contents;
    guint length;
    GError *error = NULL;

    context = 
	g_markup_parse_context_new(&parser, 0, NULL, NULL);

    if(!g_file_get_contents(filename, &file_contents, &length, &error))
    {
	g_warning("xml_loadsave_transfers_read: error reading file %s\n", filename);
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
	g_warning("xml_loadsave_transfers_read: error parsing file %s\n", filename);
	misc_print_error(&error, TRUE);
    }
}

void
xml_loadsave_transfers_write(const gchar *prefix)
{
    gint i, j;
    gchar buf[SMALL];
    FILE *fil = NULL;

    sprintf(buf, "%s___transfer_list.xml", prefix);

    file_my_fopen(buf, "w", &fil, TRUE);

    fprintf(fil, "<_%d>\n", TAG_TRANSFERS);

    for(i=0;i<transfer_list->len;i++)
    {
	fprintf(fil, "<_%d>\n", TAG_TRANSFER);

	xml_write_int(fil, trans(i).tm->id, TAG_TEAM_ID, I1);
	xml_write_int(fil, trans(i).id, TAG_TRANSFER_PLAYER_ID, I1);
	xml_write_int(fil, trans(i).time, TAG_TRANSFER_TIME, I1);

	for(j=0;j<QUALITY_END;j++)
	    xml_write_int(fil, trans(i).fee[j], TAG_TRANSFER_FEE, I1);

	for(j=0;j<QUALITY_END;j++)
	    xml_write_int(fil, trans(i).wage[j], TAG_TRANSFER_WAGE, I1);

	for(j=0;j<trans(i).offers->len;j++)
	{
	    fprintf(fil, "%s<_%d>\n", I1, TAG_TRANSFER_OFFER);
	    
	    xml_write_int(fil, transoff(i, j).tm->id, 
			  TAG_TRANSFER_OFFER_TEAM_ID, I2);
	    xml_write_int(fil, transoff(i, j).wage,
			  TAG_TRANSFER_OFFER_WAGE, I2);
	    xml_write_int(fil, transoff(i, j).status,
			  TAG_TRANSFER_OFFER_STATUS, I2);
	    xml_write_int(fil, transoff(i, j).fee, 
			  TAG_TRANSFER_OFFER_FEE, I2);

	    fprintf(fil, "</_%d>\n", TAG_TRANSFER_OFFER);
	}

	fprintf(fil, "%s</_%d>\n", I1, TAG_TRANSFER);
    }

    fprintf(fil, "</_%d>\n", TAG_TRANSFERS);

    fclose(fil);
}
