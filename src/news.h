/*
   news.h

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

#ifndef NEWS_H
#define NEWS_H

#include "bygfoot.h"
#include "news_struct.h"

/* void */
/* news_free_tokens(void); */

/* void */
/* news_initialize(const Fixture *fix); */

/* gboolean */
/* news_check_news(const LGNews *news, gchar *dest); */

/* gboolean */
/* query_news_is_repetition(gint id); */

/* void */
/* news_get_order(const GArray *commentaries, gint *order); */

/* void */
/* news_set_basic_tokens(const LiveGameUnit *unit, const Fixture *fix);; */

/* void */
/* news_set_player_tokens(const LiveGameUnit *unit, const Fixture *fix); */

/* void */
/* news_set_team_tokens(const LiveGameUnit *unit, const Fixture *fix); */

/* void */
/* news_set_stats_tokens(const LiveGameStats *stats); */

/* void */
/* news_generate(const LiveGame *live_game, LiveGameUnit *unit,  */
/* 		       const gchar *event_name, gint ev_type); */

/* gchar* */
/* news_get_extra_data(const LiveGameUnit *unit, const Fixture *fix); */

void
news_load_news_file_from_option(void);

void
news_load_news_file(const gchar *news_file, gboolean abort);

/* void */
/* news_test_load_token_file(const gchar *token_file); */

/* void */
/* news_test(const gchar *news_file, const gchar* token_file, */
/* 		   const gchar *event_name, gint number_of_passes); */

/* gint */
/* news_select(const GArray *commentaries, gchar *buf); */

#endif
