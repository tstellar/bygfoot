#ifndef XML_LOADSAVE_USERS_H
#define XML_LOADSAVE_USERS_H

#include "bygfoot.h"

void
xml_loadsave_users_start_element (GMarkupParseContext *context,
				   const gchar         *element_name,
				   const gchar        **attribute_names,
				   const gchar        **attribute_values,
				   gpointer             user_data,
				   GError             **error);

void
xml_loadsave_users_end_element    (GMarkupParseContext *context,
				    const gchar         *element_name,
				    gpointer             user_data,
				    GError             **error);

void
xml_loadsave_users_text         (GMarkupParseContext *context,
				  const gchar         *text,
				  gsize                text_len,  
				  gpointer             user_data,
				  GError             **error);

void
xml_loadsave_users_read(const gchar *dirname, const gchar *basename);

void
xml_loadsave_users_write(const gchar *prefix);

#endif

