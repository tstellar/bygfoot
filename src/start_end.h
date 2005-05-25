#ifndef START_END_H
#define START_END_H

#include "bygfoot.h"

void
start_new_game(void);

void
start_new_season(void);

void
start_write_variables(void);

void
start_generate_league_teams(void);

void
end_week_round(void);

void
end_week_round_results(void);

void
end_week_round_sort_tables(void);

void
end_week_round_update_fixtures(void);

void
start_week_round(void);

void
start_week(void);

void
start_week_update_user_teams(void);

void
start_week_round_update_user_teams(void);

void
start_week_update_user_finances(void);

void
start_week_update_users(void);

void
end_week_round_autosave(void);

gboolean
query_start_end_season_end(void);

void
start_new_season_team_movements(void);

void
end_week(void);

void
end_season(void);

void
start_week_add_cups(void);

#endif
