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
	editor_callback_func.c editor_callback_func.h \
	editor_callbacks.c editor_callbacks.h \
	editor_interface.c  editor_interface.h \
	file.c file.h \
	finance.c finance.h \
	fixture.c fixture.h \
	free.c free.h \
	game.c game.h \
	game_gui.c game_gui.h \
	generation.c generation.h \
	graph.c graph.h \
	gui.c gui.h \
	history.c history.h \
	interface.c callbacks.h interface.h support.h \
	league.c league.h \
	load_save.c load_save.h  \
	main.c main.h \
	maths.c maths.h \
	misc.c misc.h \
	misc_callbacks.c misc_callbacks.h \
	misc_callback_func.c misc_callback_func.h \
	misc_interface.c  misc_interface.h misc_callbacks.h support.h \
	misc2_callbacks.c misc2_callbacks.h \
	misc2_interface.c  misc2_interface.h misc2_callbacks.h support.h \
	option.c option.h \
	options_callbacks.c options_callbacks.h \
	options_interface.c  options_interface.h options_callbacks.h support.h \
	player.c player.h \
	startup.c startup.h \
	support.c support.h \
	team.c team.h \
	transfer.c transfer.h \
	treeview.c treeview.h \
	treeview_cell.c treeview_cell.h \
	window.c window.h \
	xml_fixtures.c xml_fixtures.h \
	xml_country.c xml_country.h \
	xml_cup.c xml_cup.h \
	xml_general.c xml_general.h \
	xml_league.c xml_league.h \
	xml_teams.c xml_teams.h \
	xml.c xml.h

bygfoot_LDADD = @PACKAGE_LIBS@ $(INTLLIBS)

