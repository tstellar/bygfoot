#ifndef XML_CUP_H
#define XML_CUP_H

#include "bygfoot.h"
#include "misc.h"
#include "gui.h"
#include "variables.h"

void
xml_cup_read_text         (GMarkupParseContext *context,
			   const gchar         *text,
			   gsize                text_len,  
			   gpointer             user_data,
			   GError             **error);

void
xml_cup_read_start_element (GMarkupParseContext *context,
			    const gchar         *element_name,
			    const gchar        **attribute_names,
			    const gchar        **attribute_values,
			    gpointer             user_data,
			    GError             **error);

void
xml_cup_read_end_element    (GMarkupParseContext *context,
			     const gchar         *element_name,
			     gpointer             user_data,
			     GError             **error);


void
xml_cup_read(const gchar *file);

#endif
