#ifndef XML_LG_COMMENTARY_H
#define XML_LG_COMMENTARY_H

#include "bygfoot.h"

void
xml_lg_commentary_read(const gchar *commentary_file);

void
xml_lg_commentary_read_text         (GMarkupParseContext *context,
				     const gchar         *text,
				     gsize                text_len,  
				     gpointer             user_data,
				     GError             **error);

void
xml_lg_commentary_read_end_element    (GMarkupParseContext *context,
				       const gchar         *element_name,
				       gpointer             user_data,
				       GError             **error);

void
xml_lg_commentary_read_start_element (GMarkupParseContext *context,
				      const gchar         *element_name,
				      const gchar        **attribute_names,
				      const gchar        **attribute_values,
				      gpointer             user_data,
				      GError             **error);

#endif

