/*
   Bygfoot Football Manager -- a small and simple GTK2-based
   football management game.

   http://bygfoot.sourceforge.net

   Copyright (C) 2005  Gyözö Both (gyboth@bygfoot.com)

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

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

