cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
set(CMAKE_LEGACY_CYGWIN_WIN32 0)

project("bygfoot")

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
add_definitions("-DGETTEXT_PACKAGE=\"${PROJECT_NAME}\"")
find_package(ZLIB)
include(GNUInstallDirs)
add_definitions("-DPACKAGE_DATA_DIR=\"${CMAKE_INSTALL_FULL_DATADIR}\"")
add_definitions("-DPACKAGE=\"${PROJECT_NAME}\"")

add_executable(bygfoot WIN32
    src/bet.c src/bet.h src/bet_struct.h
    src/bygfoot.c src/bygfoot.h
    src/callback_func.c src/callback_func.h
    src/callbacks.c src/callbacks.h
    src/cup.c src/cup.h src/cup_struct.h
    src/debug.c src/debug.h
    src/enums.h
    src/file.c src/file.h
    src/finance.c src/finance.h
    src/fixture.c src/fixture.h
    src/free.c src/free.h
    src/game.c src/game.h
    src/game_gui.c src/game_gui.h
    src/gui.c src/gui.h
    src/interface.c src/interface.h
    src/job.c src/job.h src/job_struct.h
    src/language.c src/language.h
    src/league.c src/league.h src/league_struct.h
    src/lg_commentary.c src/lg_commentary.h src/lg_commentary_struct.h
    src/live_game.c src/live_game.h
    src/load_save.c src/load_save.h
    src/main.c src/main.h
    src/maths.c src/maths.h
    src/misc.c src/misc.h
    src/misc2_callback_func.c src/misc2_callback_func.h
    src/misc2_callbacks.c src/misc2_callbacks.h
    src/misc2_interface.c src/misc2_interface.h
    src/misc3_callbacks.c src/misc3_callbacks.h
    src/misc3_interface.c src/misc3_interface.h
    src/misc_callback_func.c src/misc_callback_func.h
    src/misc_callbacks.c src/misc_callbacks.h
    src/misc_interface.c src/misc_interface.h
    src/name.c src/name.h src/name_struct.h
    src/news.c src/news.h
    src/option.c src/option.h
    src/option_gui.c src/option_gui.h
    src/options_callbacks.c src/options_callbacks.h
    src/options_interface.c src/options_interface.h
    src/player.c src/player.h
    src/start_end.c src/start_end.h
    src/stat.c src/stat.h src/stat_struct.h
    src/strategy.c src/strategy.h src/strategy_struct.h
    src/support.c src/support.h
    src/table.c src/table.h src/table_struct.h
    src/team.c src/team.h
    src/training.c src/training.h
    src/training_callbacks.c src/training_callbacks.h
    src/training_interface.c src/training_interface.h
    src/training_struct.h
    src/transfer.c src/transfer.h src/transfer_struct.h
    src/treeview.c src/treeview.h
    src/treeview2.c src/treeview2.h
    src/treeview_helper.c src/treeview_helper.h
    src/user.c src/user.h
    src/variables.h
    src/window.c src/window.h
    src/xml.c src/xml.h
    src/xml_country.c src/xml_country.h
    src/xml_cup.c src/xml_cup.h
    src/xml_league.c src/xml_league.h
    src/xml_lg_commentary.c src/xml_lg_commentary.h
    src/xml_loadsave_cup.c src/xml_loadsave_cup.h
    src/xml_loadsave_fixtures.c src/xml_loadsave_fixtures.h
    src/xml_loadsave_jobs.c src/xml_loadsave_jobs.h
    src/xml_loadsave_league.c src/xml_loadsave_league.h
    src/xml_loadsave_league_stat.c src/xml_loadsave_league_stat.h
    src/xml_loadsave_leagues_cups.c src/xml_loadsave_leagues_cups.h
    src/xml_loadsave_live_game.c src/xml_loadsave_live_game.h
    src/xml_loadsave_misc.c src/xml_loadsave_misc.h
    src/xml_loadsave_newspaper.c src/xml_loadsave_newspaper.h
    src/xml_loadsave_players.c src/xml_loadsave_players.h
    src/xml_loadsave_season_stats.c src/xml_loadsave_season_stats.h
    src/xml_loadsave_table.c src/xml_loadsave_table.h
    src/xml_loadsave_teams.c src/xml_loadsave_teams.h
    src/xml_loadsave_transfers.c src/xml_loadsave_transfers.h
    src/xml_loadsave_users.c src/xml_loadsave_users.h
    src/xml_mmatches.c src/xml_mmatches.h
    src/xml_name.c src/xml_name.h
    src/xml_news.c src/xml_news.h
    src/xml_strategy.c src/xml_strategy.h
    src/xml_team.c src/xml_team.h
    src/youth_academy.c src/youth_academy.h
    src/zip/unzip.c src/zip/unzip.h
    src/zip/zip.c src/zip/zip.h
)
# Some gtk headers use deprecated glib features, so disable this warning
# since we can't do anything about it.
target_compile_options(bygfoot PRIVATE -Wno-deprecated-declarations)
# Link the target to the GTK+ libraries
target_link_libraries (bygfoot ${GTK2_LIBRARIES} ${GLIB_LIBRARIES} m
                       ${ZLIB_LIBRARIES})

install(TARGETS bygfoot)

install(DIRECTORY support_files DESTINATION
	${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME})

find_package(Gettext)
if (GETTEXT_FOUND)
    set (languages bg da de el es fr it nl pl pt_BR pt_PT ro sv tr zh)
    foreach (lang ${languages})
        GETTEXT_PROCESS_PO_FILES(${lang} ALL PO_FILES po/${lang}.po)
	install(FILES ${CMAKE_BINARY_DIR}/${lang}.gmo DESTINATION
		${CMAKE_INSTALL_LOCALEDIR}/${lang}/LC_MESSAGES/ RENAME
		bygfoot.mo)
    endforeach()
endif()
