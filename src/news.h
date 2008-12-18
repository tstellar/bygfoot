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

void
news_free_tokens(void);

void
news_set_match_tokens(const LiveGame *live_game);

void
news_generate_match(const LiveGame *live_game);

void
news_select(const GArray *news_array, gchar *title, gchar *subtitle,
            gint *article_id, gint *title_id, gint *subtitle_id);

gint
news_get_title(const GArray *titles, gchar *title, gint *order, 
               gboolean is_title, gboolean ignore_repetition, gint to_check);

gboolean
news_check_article_for_repetition(gint id);

gboolean
news_check_title_for_repetition(gint id, gboolean is_title, gint to_check);

void
news_load_news_file_from_option(void);

void
news_load_news_file(const gchar *news_file, gboolean abort);

void
news_titles_get_order(const GArray *titles, gint *order);

gboolean
news_check_match_relevant(const LiveGame *live_game);

void
news_set_scorer_tokens(const LiveGameStats *stats);

void
news_set_league_cup_tokens(const Fixture *fix);

void
news_set_fixture_tokens(const Fixture *fix);

void
news_set_rank_tokens(const Fixture *fix);

void
news_articles_get_order(const GArray *articles, gint *order);

void
news_set_streak_tokens(const Fixture *fix);

#endif
