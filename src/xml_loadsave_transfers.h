#ifndef XML_LOADSAVE_TRANSFERS_H
#define XML_LOADSAVE_TRANSFERS_H

#include "bygfoot.h"

void
xml_loadsave_transfers_start_element (GMarkupParseContext *context,
				    const gchar         *element_name,
				    const gchar        **attribute_names,
				    const gchar        **attribute_values,
				    gpointer             user_data,
				      GError             **error);

void
xml_loadsave_transfers_end_element    (GMarkupParseContext *context,
				     const gchar         *element_name,
				     gpointer             user_data,
				       GError             **error);

void
xml_loadsave_transfers_text         (GMarkupParseContext *context,
				   const gchar         *text,
				   gsize                text_len,  
				   gpointer             user_data,
				     GError             **error);

void
xml_loadsave_transfers_read(const gchar *filename);

void
xml_loadsave_transfers_write(const gchar *prefix);


#endif
