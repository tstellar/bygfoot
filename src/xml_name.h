#ifndef XML_NAME_H
#define XML_NAME_H

#include "bygfoot.h"

void
xml_name_read_start_element (GMarkupParseContext *context,
			     const gchar         *element_name,
			     const gchar        **attribute_names,
			     const gchar        **attribute_values,
			     gpointer             user_data,
			     GError             **error);

void
xml_name_read_end_element    (GMarkupParseContext *context,
			      const gchar         *element_name,
			      gpointer             user_data,
			      GError             **error);

void
xml_name_read_text         (GMarkupParseContext *context,
			    const gchar         *text,
			    gsize                text_len,  
			    gpointer             user_data,
			    GError             **error);

void
xml_name_read(const gchar *names_file, gint number_of_names);

#endif
