#ifndef XML_LOADSAVE_TABLE_H
#define XML_LOADSAVE_TABLE_H

#include "bygfoot.h"
#include "table_struct.h"

void
xml_loadsave_table_start_element (GMarkupParseContext *context,
				   const gchar         *element_name,
				   const gchar        **attribute_names,
				   const gchar        **attribute_values,
				   gpointer             user_data,
				   GError             **error);

void
xml_loadsave_table_end_element    (GMarkupParseContext *context,
				    const gchar         *element_name,
				    gpointer             user_data,
				    GError             **error);

void
xml_loadsave_table_text         (GMarkupParseContext *context,
				  const gchar         *text,
				  gsize                text_len,  
				  gpointer             user_data,
				  GError             **error);

void
xml_loadsave_table_read(const gchar *filename, Table *table);

void
xml_loadsave_table_write(const gchar *filename, const Table *table);

#endif
