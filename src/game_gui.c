#include <unistd.h>

#include "game_gui.h"
#include "treeview.h"
#include "support.h"
#include "window.h"

/** Constants determining the live game area scale
    behaviour. */
#define CONSTANT_GAME_GUI_LIVE_GAME_SCALE_ATTACK 0.3
#define CONSTANT_GAME_GUI_LIVE_GAME_SCALE_CHANCE 0.45
#define CONSTANT_GAME_GUI_LIVE_GAME_SCALE_RANGE 10.0
#define CONSTANT_GAME_GUI_LIVE_GAME_SCALE_CHANCE_COLOR "orange"
#define CONSTANT_GAME_GUI_LIVE_GAME_SCALE_GOAL_COLOR "red"
#define CONSTANT_GAME_GUI_LIVE_GAME_SCALE_MISS_COLOR "lightgreen"

/** Show the live game in the live game window.
    @param unit The current unit we show. */
void
game_gui_live_game_show_unit(const LiveGameUnit *unit)
{
    gchar buf[SMALL];
    gfloat fraction = (gfloat)live_game_unit_get_minute(unit) / 90;
    GtkProgressBar *progress_bar;

    if(live_game.window == NULL)
    {
	live_game.window = window_create(WINDOW_LIVE);
	treeview_live_game_show_initial_commentary(unit);
	
	gtk_range_set_range(
	    GTK_RANGE(lookup_widget(live_game.window, "hscale_area")), 0,
	    CONSTANT_GAME_GUI_LIVE_GAME_SCALE_RANGE);
    }
    else
	treeview_live_game_show_commentary(unit);

    treeview_live_game_show_result(unit);

    game_gui_live_game_set_hscale(unit,
				  GTK_HSCALE(lookup_widget(live_game.window, "hscale_area")));    

    sprintf(buf, "%d.", live_game_unit_get_minute(unit));
    progress_bar = GTK_PROGRESS_BAR(lookup_widget(live_game.window, "progressbar_live"));
    gtk_progress_bar_set_fraction(progress_bar, (fraction > 1) ? 1 : fraction);
    gtk_progress_bar_set_text(progress_bar, buf);
    usleep(500500 + options[OPT_LIVE_SPEED] * 50000);
    while(gtk_events_pending())
	gtk_main_iteration();

    if(unit->event.type == LIVE_GAME_EVENT_END_MATCH)
	gtk_widget_set_sensitive(lookup_widget(live_game.window, "button_live_close"), TRUE);

}

/** Set the area scale position and color in the live game window.
    @param unit The current unit.
    @param hscale The scale widget. */
void
game_gui_live_game_set_hscale(const LiveGameUnit *unit, GtkHScale *hscale)
{
    GdkColor color;

    gtk_widget_modify_bg(GTK_WIDGET(hscale), GTK_STATE_NORMAL, NULL);

    if(unit->area == LIVE_GAME_UNIT_AREA_MIDFIELD)
	gtk_range_set_value(GTK_RANGE(hscale),
			    CONSTANT_GAME_GUI_LIVE_GAME_SCALE_RANGE / 2);
    else if(unit->event.type == LIVE_GAME_EVENT_GOAL)
    {
	gdk_color_parse(CONSTANT_GAME_GUI_LIVE_GAME_SCALE_GOAL_COLOR, &color);
	gtk_widget_modify_bg(GTK_WIDGET(hscale), GTK_STATE_NORMAL, &color);

	gtk_range_set_value(GTK_RANGE(hscale), 
			    CONSTANT_GAME_GUI_LIVE_GAME_SCALE_RANGE * (unit->possession == 0));
    }
    else if(unit->event.type == LIVE_GAME_EVENT_SCORING_CHANCE ||
	    unit->event.type == LIVE_GAME_EVENT_PENALTY ||
	    unit->event.type == LIVE_GAME_EVENT_FREE_KICK)
    {
	gdk_color_parse(CONSTANT_GAME_GUI_LIVE_GAME_SCALE_CHANCE_COLOR, &color);
	gtk_widget_modify_bg(GTK_WIDGET(hscale), GTK_STATE_NORMAL, &color);

	gtk_range_set_value(GTK_RANGE(hscale), 
			    CONSTANT_GAME_GUI_LIVE_GAME_SCALE_RANGE / 2 +
			    (CONSTANT_GAME_GUI_LIVE_GAME_SCALE_RANGE * 
			     CONSTANT_GAME_GUI_LIVE_GAME_SCALE_CHANCE *
			     ((unit->possession == 0) ? 1 : -1)));
    }
    else if(unit->event.type == LIVE_GAME_EVENT_POST ||
	    unit->event.type == LIVE_GAME_EVENT_MISSED ||
	    unit->event.type == LIVE_GAME_EVENT_SAVE ||
	    unit->event.type == LIVE_GAME_EVENT_CROSS_BAR)
    {
	gdk_color_parse(CONSTANT_GAME_GUI_LIVE_GAME_SCALE_MISS_COLOR, &color);
	gtk_widget_modify_bg(GTK_WIDGET(hscale), GTK_STATE_NORMAL, &color);
    }
    else if(unit->area == LIVE_GAME_UNIT_AREA_ATTACK)
	gtk_range_set_value(GTK_RANGE(hscale),
			    CONSTANT_GAME_GUI_LIVE_GAME_SCALE_RANGE / 2 +
			    (CONSTANT_GAME_GUI_LIVE_GAME_SCALE_RANGE * 
			     CONSTANT_GAME_GUI_LIVE_GAME_SCALE_ATTACK *
			     ((unit->possession == 0) ? 1 : -1)));
    else if(unit->area == LIVE_GAME_UNIT_AREA_DEFEND)
	gtk_range_set_value(GTK_RANGE(hscale),
			    CONSTANT_GAME_GUI_LIVE_GAME_SCALE_RANGE / 2 +
			    (CONSTANT_GAME_GUI_LIVE_GAME_SCALE_RANGE * 
			     CONSTANT_GAME_GUI_LIVE_GAME_SCALE_ATTACK *
			     ((unit->possession == 0) ? -1 : 1)));
    else
	g_warning("game_gui_live_game_set_hscale: don't know what to do!\n");

    if(debug)
	printf("***** area %d value %.1f\n", unit->area, gtk_range_get_value(GTK_RANGE(hscale)));
}
