## Process this file with automake to produce Makefile.in
AM_CFLAGS = -Wall

INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	@PACKAGE_CFLAGS@

bin_PROGRAMS = bygfoot

bygfoot_SOURCES = \
	callback_func.c callback_func.h callbacks.h cup.h finance.h fixture.h game_gui.h league.h live_game.h misc.h option.h player.h start_end.h team.h transfer.h treeview.h user.h window.h \
	callbacks.c callbacks.h callback_func.h game_gui.h main.h option.h team.h transfer.h treeview.h user.h window.h \
	cup.c cup.h free.h main.h maths.h misc.h team.h variables.h xml_league.h \
	file.c file.h free.h main.h misc.h option.h support.h variables.h \
	finance.c callbacks.h finance.h game_gui.h maths.h option.h player.h team.h user.h \
	fixture.c cup.h fixture.h free.h league.h main.h maths.h misc.h table.h team.h variables.h \
	free.c free.h user.h variables.h \
	game.c cup.h finance.h fixture.h game.h game_gui.h league.h live_game.h maths.h misc.h option.h player.h table.h team.h treeview.h user.h variables.h \
	game_gui.c file.h game_gui.h gui.h league.h live_game.h maths.h misc.h option.h treeview.h support.h team.h user.h variables.h window.h \
	gui.c gui.h misc.h support.h variables.h window.h \
	interface.c callbacks.h interface.h support.h \
	league.c cup.h league.h team.h variables.h \
	live_game.c fixture.h free.h game.h game_gui.h live_game.h maths.h misc_callback_func.h option.h player.h support.h team.h treeview.h user.h variables.h window.h \
	load_save.c load_save.h  \
	main.c misc_callbacks.h file.h free.h main.h transfer_struct.h variables.h window.h \
	maths.c maths.h misc.h \
	misc.c main.h maths.h misc.h \
	misc_callbacks.c free.h misc_callback_func.h misc_callbacks.h \
	misc_callback_func.c misc_callback_func.h start_end.h support.h team.h treeview.h user.h variables.h xml_country.h \
	misc_interface.c  misc_interface.h misc_callbacks.h support.h \
	misc2_callbacks.c finance.h game_gui.h main.h misc2_callbacks.h misc2_callback_func.h misc2_interface.h support.h transfer.h user.h window.h \
	misc2_callback_func.c finance.h game_gui.h maths.h misc2_callback_func.h option.h player.h team.h treeview.h transfer.h user.h \
	misc2_interface.c  misc2_interface.h misc2_callbacks.h support.h \
	option.c option.h variables.h \
	option_gui.c file.h option_gui.h option.h support.h user.h variables.h \
	options_callbacks.c options_callbacks.h option_gui.h \
	options_interface.c  options_interface.h options_callbacks.h support.h \
	player.c cup.h free.h game_gui.h league.h maths.h misc.h option.h player.h team.h user.h \
	start_end.c cup.h file.h finance.h fixture.h game_gui.h gui.h league.h live_game.h main.h maths.h start_end.h table.h team.h transfer.h user.h variables.h xml_name.h \
	support.c support.h \
	table.c cup.h league.h table.h variables.h \
	team.c cup.h fixture.h game.h game_gui.h league.h maths.h misc.h option.h player.h team.h transfer.h user.h \
	transfer.c finance.h free.h maths.h option.h player.h team.h transfer.h user.h \
	treeview.c cup.h file.h finance.h fixture.h free.h league.h live_game.h maths.h misc.h player.h option.h support.h team.h transfer.h treeview.h treeview_cell.h user.h \
	treeview_cell.c misc.h option.h player.h team.h treeview.h treeview_cell.h user.h variables.h \
	user.c fixture.h free.h game_gui.h maths.h misc.h option.h player.h transfer.h user.h window.h \
	window.c file.h finance.h free.h game_gui.h gui.h interface.h misc_interface.h misc2_interface.h option.h support.h user.h window.h \
	xml_cup.c cup.h file.h misc.h xml_cup.h \
	xml_country.c file.h free.h misc.h variables.h xml_cup.h xml_country.h xml_league.h \
	xml_league.c cup_struct.h file.h free.h league.h misc.h team.h table.h variables.h xml_league.h xml_cup.h \
	xml_name.c file.h free.h maths.h misc.h variables.h xml_name.h

bygfoot_LDADD = @PACKAGE_LIBS@ $(INTLLIBS)

