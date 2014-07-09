cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
set(CMAKE_LEGACY_CYGWIN_WIN32 0)

project("To Do List")

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB_PKG glib-2.0)

if (GLIB_PKG_FOUND)
    message(Found glib-2.0)
    # include_directories(${GLIB_PKG_INCLUDE_DIRS})
endif (GLIB_PKG_FOUND)

 pkg_check_modules (GTK2 REQUIRED gtk+-2.0)
 # Setup CMake to use GTK+, tell the compiler where to look for headers
 # and to the linker where to look for libraries
 # Setup CMake to use GTK+, tell the compiler where to look for headers
 include_directories (${GTK2_INCLUDE_DIRS})
 link_directories (${GTK2_LIBRARY_DIRS})
 # Add other flags to the compiler
 add_definitions (${GTK2_CFLAGS_OTHER})
 pkg_check_modules (GETTEXT REQUIRED)

add_executable(bygfoot src/bet.c src/bet.h src/finance.h src/fixture.h src/game_gui.h src/league.h src/main.h src/maths.h src/misc.h src/option.h src/player.h src/support.h src/treeview2.h src/user.h src/callback_func.c src/callback_func.h src/callbacks.h src/cup.h src/gui.h src/live_game.h src/start_end.h src/team.h src/transfer.h src/treeview.h src/treeview_helper.h src/window.h src/callbacks.c src/debug.h src/free.h src/game.h src/job.h src/load_save.h src/misc2_callback_func.h src/table.h src/cup.c src/variables.h src/xml_league.h src/debug.c src/strategy.h src/file.c src/file.h src/language.h src/finance.c src/fixture.c src/free.c src/bet_struct.h src/lg_commentary_struct.h src/strategy_struct.h src/game.c src/game_gui.c src/gui.c src/interface.c src/interface.h src/job.c src/xml_country.h src/language.c src/lg_commentary.h src/lg_commentary.c src/xml_lg_commentary.h src/league.c src/stat.h src/live_game.c src/misc_callback_func.h src/load_save.c src/xml.h src/xml_loadsave_cup.h src/xml_loadsave_jobs.h src/xml_loadsave_league.h src/xml_loadsave_leagues_cups.h src/xml_loadsave_misc.h src/xml_loadsave_season_stats.h src/xml_loadsave_transfers.h src/xml_loadsave_users.h src/main.c src/job_struct.h src/misc_callbacks.h src/name_struct.h src/stat_struct.h src/transfer_struct.h src/xml_strategy.h src/maths.c src/misc.c src/misc_callbacks.c src/misc_callback_func.c src/xml_name.h src/misc2_callbacks.c src/misc2_callbacks.h src/misc2_interface.h src/misc3_callbacks.c src/misc3_callbacks.h src/misc3_interface.h src/misc2_callback_func.c src/misc_interface.c src/misc_interface.h src/misc2_interface.c src/misc3_interface.c src/name.c src/name.h src/news.c src/news.h src/xml_news.h src/option.c src/option_gui.c src/option_gui.h src/training.h src/options_callbacks.c src/options_callbacks.h src/options_interface.h src/options_interface.c src/player.c src/start_end.c src/youth_academy.h src/stat.c src/table_struct.h src/strategy.c src/support.c src/table.c src/team.c src/xml_team.h src/training.c src/enums.h src/training_callbacks.c src/bygfoot.h src/training_callbacks.h src/training_interface.h src/training_struct.h src/training_interface.c src/transfer.c src/treeview.c src/treeview2.c src/treeview_helper.c src/user.c src/xml_mmatches.h src/window.c src/xml.c src/xml_loadsave_fixtures.h src/xml_loadsave_league_stat.h src/xml_loadsave_live_game.h src/xml_loadsave_table.h src/xml_loadsave_teams.h src/xml_loadsave_cup.c src/xml_loadsave_fixtures.c src/xml_loadsave_jobs.c src/xml_loadsave_league.c src/xml_loadsave_leagues_cups.c src/league_struct.h src/xml_loadsave_league_stat.c src/xml_loadsave_live_game.c src/xml_loadsave_misc.c src/xml_loadsave_newspaper.c src/xml_loadsave_newspaper.h src/xml_loadsave_players.c src/xml_loadsave_players.h src/xml_loadsave_season_stats.c src/xml_loadsave_table.c src/xml_loadsave_teams.c src/xml_loadsave_transfers.c src/xml_loadsave_users.c src/xml_cup.c src/xml_cup.h src/xml_country.c src/xml_league.c src/cup_struct.h src/xml_lg_commentary.c src/xml_mmatches.c src/xml_name.c src/xml_news.c src/xml_strategy.c src/xml_team.c src/youth_academy.c src/zip/zip.c src/zip/zip.h src/zip/unzip.c src/zip/unzip.h)

 # Link the target to the GTK+ libraries
target_link_libraries (bygfoot ${GTK2_LIBRARIES} ${GLIB_LIBRARIES})

