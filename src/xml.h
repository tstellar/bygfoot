#ifndef XML_H
#define XML_H

#include "bygfoot.h"
#include "maths.h"
#include "cup_struct.h"

/** Indentations for xml files. */
#define I0 ""
#define I1 "\t"
#define I2 "\t\t"
#define I3 "\t\t\t"
#define I4 "\t\t\t\t"
#define I5 "\t\t\t\t\t"
#define I6 "\t\t\t\t\t\t"
#define I7 "\t\t\t\t\t\t\t"
#define I8 "\t\t\t\t\t\t\t\t"

/** Tags that are used in more than one type of xml file. */
enum XmlTags
{
    TAG_NAME = 0,
    TAG_SYMBOL,
    TAG_SID,
    TAG_SHORT_NAME,
    TAG_ID,
    TAG_WEEK_GAP,
    TAG_YELLOW_RED,
    TAG_TEAM_ID,
    TAG_NAMES_FILE,
    TAG_ROUND
};

/** Starting values for tag enums in the various xml loading source files. */
#define TAG_START_MISC 1000
#define TAG_START_LEAGUE 2000
#define TAG_START_CUP 3000
#define TAG_START_TEAMS 4000
#define TAG_START_FIXTURES 5000
#define TAG_START_TABLE 6000
#define TAG_START_USERS 7000
#define TAG_START_LIVE_GAME 8000
#define TAG_START_PLAYERS 9000
#define TAG_END_PLAYERS 9900
#define TAG_START_LEAGUE_STAT 10000
#define TAG_START_SEASON_STATS 11000
#define TAG_START_TRANSFERS 20000

#define xml_write_g_string(fil, gstring, tag, indent) xml_write_string(fil, (gstring)->str, tag, indent)

#define xml_write_int(fil, value, tag, indent) fprintf(fil, "%s<_%d>%d</_%d>\n", indent, tag, value, tag)

#define xml_write_float(fil, value, tag, indent) fprintf(fil, "%s<_%d>%d</_%d>\n", indent, tag, (gint)rint(value * 10000), tag)

#define xml_get_tag_from_name(name) (gint)g_ascii_strtod(name + 1, NULL)

void
xml_write_string(FILE *fil, const gchar *string, gint tag, const gchar* indent);

void
xml_load_users(const gchar *dirname, const gchar *basename);

void
xml_load_leagues(const gchar *dirname, const gchar *basename);

void
xml_load_league(const gchar *dirname, const gchar *basename, const GPtrArray *dir_contents);

void
xml_load_cups(const gchar *dirname, const gchar *basename);

void
xml_load_cup(Cup *cup, const gchar *dirname, const gchar *basename, const GPtrArray *dir_contents);

void
xml_load_transfers(const gchar *dirname, const gchar *basename);

#endif
