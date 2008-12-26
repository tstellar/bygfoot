/*
   news_struct.h

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

#ifndef NEWS_STRUCT_H
#define NEWS_STRUCT_H

/** Enumeration of news article types. */
enum NewsArticleTypes
{
    NEWS_ARTICLE_TYPE_MATCH = 0, /**< Article about a match. */
    NEWS_ARTICLE_TYPE_FINANCES, /**< Article about user finances. */
    NEWS_ARTICLE_TYPE_STAR_PLAYER_TRANSFER, /**< Article about good players appearing on the transfer list. */
    NEWS_ARTICLE_TYPE_LEAGUE_CHAMPION, /**< Article about teams clinching the championship. */
    NEWS_ARTICLE_TYPE_CUP_QUALIFICATION, /**< Article about teams clinching cup berths. */
    NEWS_ARTICLE_TYPE_RELEGATION, /**< Article about teams getting relegated. */
    NEWS_ARTICLE_TYPE_END
};

/** Structure containing a news title or subtitle with tokens.  */
typedef struct
{
    gchar *text;
    /** Priority of the text (compared to
	the other ones for the same article type).
	The higher the priority the higher the
	probability that the text gets picked. */
    gint priority;
    /** An id to keep track of already used texts
        (so as not to use the same one too frequently). */
    gint id;
    /** A condition (if not fulfilled, the title or subtitle
        doesn't get considered). */
    gchar *condition;

} NewsText;

/** Structure describing a news paper article with tokens.  */
typedef struct
{
    /** Possible article titles (possibly containing tokens). */
    GArray *titles;
    /** Possible article subtitles (possibly containing tokens). */
    GArray *subtitles;
    /** A condition (if not fulfilled, the article doesn't get
	shown). */
    gchar *condition;
    /** Priority of the article. */
    gint priority;
    /** An id to avoid repetitions. */
    gint id;

} NewsArticle;

/** Structure holding an article without tokens (ie. the real deal that's displayed). */
typedef struct
{
    gint week_number, week_round_number;
    gint title_id, subtitle_id;
    gint clid, cup_round;
    gchar *title_small, *title, *subtitle;
    gint id;
    gint user_idx;

} NewsPaperArticle;

/** Structure holding the newspaper for the game. */
typedef struct
{
    /** The array of created articles. */
    GArray *articles;

} NewsPaper;

#endif

