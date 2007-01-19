## Process this file with automake to produce Makefile.in
AM_CFLAGS = -Wall #-fprofile-arcs -ftest-coverage -pg

INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	@PACKAGE_CFLAGS@ @GST_CFLAGS@

bin_PROGRAMS = bygfoot

bygfoot_SOURCES = \
	bet.c bet.h finance.h fixture.h game_gui.h league.h main.h maths.h misc.h option.h player.h support.h treeview2.h user.h \
	callback_func.c bet.h callback_func.h callbacks.h cup.h finance.h fixture.h game_gui.h gui.h league.h live_game.h maths.h misc.h option.h player.h start_end.h team.h transfer.h treeview.h treeview_helper.h user.h window.h \
	callbacks.c callback_func.h callbacks.h debug.h free.h game_gui.h game.h gui.h job.h league.h load_save.h main.h misc2_callback_func.h option.h player.h table.h team.h transfer.h treeview2.h treeview.h treeview_helper.h user.h window.h \
	cup.c cup.h fixture.h free.h league.h main.h maths.h misc.h option.h table.h team.h variables.h xml_league.h \
	debug.c callbacks.h debug.h game_gui.h league.h live_game.h option.h strategy.h support.h team.h user.h variables.h \
	file.c file.h free.h language.h main.h misc.h option.h support.h variables.h \
	finance.c callbacks.h finance.h game_gui.h league.h maths.h option.h player.h team.h user.h \
	fixture.c cup.h fixture.h free.h league.h main.h maths.h misc.h option.h table.h team.h user.h variables.h \
	free.c bet_struct.h free.h lg_commentary_struct.h strategy_struct.h transfer.h user.h variables.h \
	game.c cup.h finance.h fixture.h game_gui.h game.h league.h live_game.h main.h maths.h misc.h option.h player.h table.h team.h treeview.h user.h variables.h \
	game_gui.c callbacks.h file.h game_gui.h gui.h job.h league.h live_game.h maths.h misc.h option.h support.h team.h treeview.h user.h variables.h window.h \
	gui.c gui.h misc.h option.h support.h variables.h window.h \
	interface.c callbacks.h interface.h support.h \
	job.c bet.h cup.h file.h free.h job.h league.h live_game.h main.h maths.h option.h start_end.h team.h transfer.h user.h variables.h xml_country.h \
	language.c callbacks.h free.h language.h lg_commentary.h misc.h option.h variables.h window.h \
	lg_commentary.c cup.h file.h fixture.h free.h language.h league.h lg_commentary.h live_game.h main.h maths.h misc.h option.h player.h team.h variables.h xml_lg_commentary.h \
	league.c cup.h free.h league.h main.h maths.h misc.h option.h player.h stat.h table.h team.h user.h variables.h \
	live_game.c callbacks.h fixture.h free.h game_gui.h game.h league.h lg_commentary.h live_game.h main.h maths.h misc_callback_func.h misc.h option.h player.h strategy.h support.h team.h treeview.h user.h variables.h window.h \
	load_save.c callbacks.h file.h game_gui.h gui.h language.h load_save.h misc.h option.h support.h user.h variables.h window.h xml.h xml_loadsave_cup.h xml_loadsave_jobs.h xml_loadsave_league.h xml_loadsave_leagues_cups.h xml_loadsave_misc.h xml_loadsave_season_stats.h xml_loadsave_transfers.h xml_loadsave_users.h \
	main.c bet_struct.h debug.h file.h free.h job_struct.h language.h lg_commentary.h live_game.h load_save.h main.h misc_callbacks.h misc.h name_struct.h option.h start_end.h stat_struct.h strategy_struct.h transfer_struct.h variables.h window.h xml_strategy.h \
	maths.c maths.h misc.h variables.h \
	misc.c main.h maths.h misc.h option.h variables.h \
	misc_callbacks.c callback_func.h callbacks.h game_gui.h game.h gui.h live_game.h load_save.h main.h maths.h misc_callback_func.h misc_callbacks.h option.h user.h variables.h window.h xml_country.h \
	misc_callback_func.c callbacks.h debug.h finance.h free.h game_gui.h game.h gui.h load_save.h main.h maths.h misc_callback_func.h misc.h option.h start_end.h support.h treeview.h treeview_helper.h user.h variables.h window.h xml_country.h xml_name.h \
	misc2_callbacks.c bet.h callback_func.h callbacks.h debug.h finance.h free.h game_gui.h job.h league.h load_save.h main.h misc2_callback_func.h misc2_callbacks.h misc2_interface.h player.h support.h team.h transfer.h treeview2.h treeview.h treeview_helper.h user.h window.h \
	misc3_callbacks.c bet.h fixture.h main.h misc3_callbacks.h misc3_interface.h misc_callback_func.h option.h support.h treeview2.h treeview_helper.h user.h variables.h window.h \
	misc2_callback_func.c callback_func.h callbacks.h file.h finance.h free.h game_gui.h job.h league.h maths.h misc2_callback_func.h misc.h option.h player.h support.h team.h transfer.h treeview2.h treeview.h treeview_helper.h user.h window.h \
	misc_interface.c misc_callbacks.h misc_interface.h support.h \
	misc2_interface.c misc2_callbacks.h misc2_interface.h support.h \
	misc3_interface.c misc3_callbacks.h misc3_interface.h support.h \
	name.c main.h name.h option.h variables.h xml_name.h \
	option.c main.h misc.h option.h variables.h \
	option_gui.c callbacks.h file.h game_gui.h language.h misc.h option_gui.h option.h support.h treeview.h user.h variables.h \
	options_callbacks.c file.h option_gui.h options_callbacks.h options_interface.h support.h user.h variables.h window.h \
	options_interface.c options_callbacks.h options_interface.h support.h \
	player.c cup.h fixture.h free.h game_gui.h league.h main.h maths.h misc.h name.h option.h player.h team.h transfer.h user.h \
	start_end.c bet.h cup.h file.h finance.h fixture.h free.h game_gui.h gui.h job.h league.h live_game.h load_save.h main.h maths.h misc.h name.h option.h player.h start_end.h stat.h table.h team.h transfer.h user.h variables.h xml_name.h youth_academy.h \
	stat.c cup.h free.h league.h option.h player.h stat.h table_struct.h team.h variables.h \
	strategy.c fixture.h league.h live_game.h main.h misc.h option.h player.h strategy.h team.h \
	support.c support.h \
	table.c cup.h league.h misc.h table.h variables.h \
	team.c cup.h file.h finance.h fixture.h game_gui.h game.h league.h main.h maths.h misc.h option.h player.h strategy.h team.h transfer.h user.h xml_team.h \
	training.c training.h \
	training_callbacks.c training_callbacks.h training_interface.h variables.h window.h training_struct.h \
	training_interface.c training_callbacks.h training_interface.h support.h \
	transfer.c callbacks.h cup.h finance.h free.h game_gui.h main.h maths.h misc.h option.h player.h support.h team.h transfer.h treeview.h user.h \
	treeview.c cup.h file.h finance.h fixture.h free.h game_gui.h game.h language.h league.h live_game.h maths.h misc.h option.h player.h support.h team.h transfer.h treeview.h treeview_helper.h user.h \
	treeview2.c bet.h fixture.h job.h league.h live_game.h misc.h option.h support.h team.h treeview2.h treeview_helper.h user.h \
	treeview_helper.c bet.h cup.h file.h fixture.h free.h job.h league.h misc.h option.h player.h support.h team.h transfer.h treeview2.h treeview.h treeview_helper.h user.h variables.h \
	user.c bet.h cup.h file.h finance.h fixture.h free.h game_gui.h league.h live_game.h main.h maths.h misc.h name.h option.h player.h support.h team.h transfer.h treeview.h user.h window.h xml_mmatches.h youth_academy.h \
	window.c callbacks.h debug.h file.h finance.h free.h game_gui.h gui.h interface.h language.h league.h live_game.h load_save.h main.h maths.h misc2_interface.h misc3_interface.h misc_callback_func.h misc_interface.h option_gui.h option.h options_interface.h support.h training_interface.h transfer.h treeview2.h treeview.h treeview_helper.h user.h window.h \
	xml.c cup.h file.h free.h gui.h league.h misc.h option.h support.h table.h transfer_struct.h user.h variables.h xml.h xml_loadsave_cup.h xml_loadsave_fixtures.h xml_loadsave_league.h xml_loadsave_league_stat.h xml_loadsave_live_game.h xml_loadsave_table.h xml_loadsave_teams.h xml_loadsave_transfers.h xml_loadsave_users.h \
	xml_loadsave_cup.c cup.h file.h misc.h table.h team.h xml.h xml_loadsave_cup.h xml_loadsave_fixtures.h xml_loadsave_table.h xml_loadsave_teams.h \
	xml_loadsave_fixtures.c file.h fixture.h misc.h team.h xml.h xml_loadsave_fixtures.h \
	xml_loadsave_jobs.c file.h free.h job.h misc.h variables.h xml.h xml_loadsave_jobs.h xml_loadsave_teams.h \
	xml_loadsave_league.c file.h league.h misc.h xml.h xml_loadsave_cup.h xml_loadsave_fixtures.h xml_loadsave_league.h xml_loadsave_league_stat.h xml_loadsave_table.h xml_loadsave_teams.h \
	xml_loadsave_leagues_cups.c cup.h file.h free.h league_struct.h misc.h xml.h xml_loadsave_cup.h xml_loadsave_league.h xml_loadsave_leagues_cups.h \
	xml_loadsave_league_stat.c file.h misc.h stat.h team.h xml.h xml_loadsave_league_stat.h \
	xml_loadsave_live_game.c cup.h file.h fixture.h league.h live_game.h misc.h option.h variables.h xml.h xml_loadsave_live_game.h \
	xml_loadsave_misc.c bet_struct.h cup.h file.h fixture.h free.h misc.h variables.h xml.h xml_loadsave_cup.h xml_loadsave_league.h xml_loadsave_misc.h \
	xml_loadsave_players.c misc.h player.h team.h xml.h xml_loadsave_players.h \
	xml_loadsave_season_stats.c file.h free.h misc.h stat.h variables.h xml.h xml_loadsave_league_stat.h xml_loadsave_season_stats.h \
	xml_loadsave_table.c file.h misc.h team.h xml.h xml_loadsave_table.h \
	xml_loadsave_teams.c file.h misc.h player.h team.h xml.h xml_loadsave_players.h xml_loadsave_teams.h \
	xml_loadsave_transfers.c file.h misc.h team.h transfer.h xml.h xml_loadsave_transfers.h \
	xml_loadsave_users.c bet_struct.h file.h fixture.h misc.h team.h user.h variables.h xml.h xml_loadsave_live_game.h xml_loadsave_players.h xml_loadsave_users.h \
	xml_cup.c cup.h file.h main.h misc.h option.h variables.h xml_cup.h \
	xml_country.c file.h free.h misc.h option.h variables.h xml_country.h xml_cup.h xml_league.h \
	xml_league.c cup_struct.h file.h free.h league.h misc.h option.h table.h team.h variables.h xml_cup.h xml_league.h \
	xml_lg_commentary.c free.h lg_commentary.h live_game.h misc.h variables.h xml_lg_commentary.h \
	xml_mmatches.c file.h free.h live_game.h misc.h user.h xml.h xml_loadsave_live_game.h xml_mmatches.h \
	xml_name.c file.h free.h misc.h name.h variables.h xml_name.h \
	xml_strategy.c file.h free.h main.h misc.h strategy_struct.h xml_strategy.h \
	xml_team.c file.h free.h main.h misc.h option.h player.h team.h variables.h xml_team.h \
	youth_academy.c free.h maths.h name.h option.h player.h team.h user.h youth_academy.h 

bygfoot_LDADD = @PACKAGE_LIBS@ @GST_LIBS@ $(INTLLIBS) 

