## Process this file with automake to produce Makefile.in
AM_CFLAGS = -Wall

INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	@PACKAGE_CFLAGS@

bin_PROGRAMS = bygfoot

bygfoot_SOURCES = \
	callback_func.c callback_func.h \
	callbacks.c callbacks.h \
	cup.c cup.h free.h maths.h misc.h team.h variables.h xml_league.h \
	file.c file.h free.h misc.h support.h \
	finance.c finance.h \
	fixture.c cup.h fixture.h league_struct.h main.h maths.h misc.h team.h variables.h \
	free.c cup.h fixture.h free.h league.h player.h variables.h \
	game.c game.h \
	game_gui.c game_gui.h \
	generation.c generation.h \
	graph.c graph.h \
	gui.c gui.h \
	history.c history.h \
	interface.c callbacks.h interface.h support.h \
	league.c league.h team.h variables.h \
	live_game.c game.h live_game.h variables.h \
	load_save.c load_save.h  \
	main.c file.h free.h main.h variables.h window.h \
	maths.c maths.h misc.h variables.h \
	misc.c free.h misc.h \
	misc_callbacks.c free.h misc_callback_func.h misc_callbacks.h \
	misc_callback_func.c misc_callback_func.h start_end.h support.h team.h treeview.h user.h variables.h xml_country.h \
	misc_interface.c  misc_interface.h misc_callbacks.h support.h \
	misc2_callbacks.c misc2_callbacks.h \
	misc2_interface.c  misc2_interface.h misc2_callbacks.h support.h \
	option.c option.h \
	options_callbacks.c options_callbacks.h \
	options_interface.c  options_interface.h options_callbacks.h support.h \
	player.c free.h league.h maths.h player.h team.h variables.h \
	start_end.c cup.h fixture.h league.h maths.h player.h start_end.h team.h transfer.h variables.h xml_name.h \
	support.c support.h \
	table.c table.h \
	team.c cup.h free.h league.h maths.h player.h team.h variables.h \
	transfer.c transfer.h \
	treeview.c gui.h league.h support.h team.h treeview.h variables.h \
	treeview_cell.c treeview_cell.h \
	user.c team.h user.h user_struct.h \
	window.c misc_interface.h file.h free.h support.h window.h \
	xml_cup.c cup.h file.h misc.h variables.h xml_cup.h \
	xml_country.c file.h free.h league.h misc.h variables.h xml_cup.h xml_country.h xml_league.h \
	xml_general.c xml_general.h \
	xml_league.c file.h league.h misc.h team.h variables.h xml_league.h \
	xml_name.c file.h free.h maths.h variables.h xml_name.h \
	xml_teams.c xml_teams.h \
	xml.c xml.h

bygfoot_LDADD = @PACKAGE_LIBS@ $(INTLLIBS)

