#ifndef XML_LOADSAVE_FIXTURES_H
#define XML_LOADSAVE_FIXTURES_H

#include "bygfoot.h"
#include "fixture_struct.h"

void
xml_loadsave_fixtures_start_element (GMarkupParseContext *context,
				   const gchar         *element_name,
				   const gchar        **attribute_names,
				   const gchar        **attribute_values,
				   gpointer             user_data,
				   GError             **error);

void
xml_loadsave_fixtures_end_element    (GMarkupParseContext *context,
				    const gchar         *element_name,
				    gpointer             user_data,
				    GError             **error);

void
xml_loadsave_fixtures_text         (GMarkupParseContext *context,
				  const gchar         *text,
				  gsize                text_len,  
				  gpointer             user_data,
				  GError             **error);

void
xml_loadsave_fixtures_read(const gchar *filename, GArray *fixtures);

void
xml_loadsave_fixtures_write(const gchar *filename, const GArray *fixtures);

#endif
