#ifndef XML_MMATCHES_H
#define XML_MMATCHES_H

void
xml_mmatches_write(const gchar *prefix, const GArray *mmatches);

void
xml_mmatches_start_element (GMarkupParseContext *context,
			    const gchar         *element_name,
			    const gchar        **attribute_names,
			    const gchar        **attribute_values,
			    gpointer             user_data,
			    GError             **error);

void
xml_mmatches_end_element    (GMarkupParseContext *context,
			     const gchar         *element_name,
			     gpointer             user_data,
			     GError             **error);

void
xml_mmatches_text         (GMarkupParseContext *context,
			   const gchar         *text,
			   gsize                text_len,  
			   gpointer             user_data,
			   GError             **error);

void
xml_mmatches_read(const gchar *filename, GArray *mmatches);

#endif
