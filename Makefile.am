## Process this file with automake to produce Makefile.in

AM_CFLAGS = -Wall

SUBDIRS = src po

bin_SCRIPTS = bygfoot-update

EXTRA_DIST = \
	autogen.sh \
	bygfoot.glade \
	bygfoot.gladep

dist_man6_MANS = man/bygfoot.6

SUPPORT_DIRS = support_files support_files/names support_files/pixmaps support_files/pixmaps/live_game support_files/definitions 

SUPPORT_FILES = support_files/names/player_names.xml support_files/bygfoot.conf support_files/pixmaps/flag_ro.png support_files/pixmaps/bygfoot_team_editor_icon.png support_files/pixmaps/bygfoot_icon.png support_files/pixmaps/pay_loan.png support_files/pixmaps/inc_cap.png support_files/pixmaps/new_week.png support_files/pixmaps/preview.png support_files/pixmaps/get_loan.png support_files/pixmaps/transfers.png support_files/pixmaps/fire_player.png support_files/pixmaps/inc_saf.png support_files/pixmaps/football.png support_files/pixmaps/flag_br.png support_files/pixmaps/flag_de.png support_files/pixmaps/fin_stad.png support_files/pixmaps/flag_en.png support_files/pixmaps/flag_es.png support_files/pixmaps/flag_fr.png support_files/pixmaps/possession_ball.png support_files/pixmaps/flag_hu.png support_files/pixmaps/live_game/yellow.png support_files/pixmaps/live_game/red.png support_files/pixmaps/live_game/save.png support_files/pixmaps/live_game/goal.png support_files/pixmaps/flag_it.png support_files/pixmaps/flag_mx.png support_files/pixmaps/flag_pl.png support_files/definitions/league_turkey1_short.xml support_files/definitions/league_italy1_short.xml support_files/definitions/league_germany1_short.xml support_files/definitions/country_england.xml support_files/definitions/league_russia1_short.xml support_files/definitions/league_scotland1_short.xml support_files/definitions/league_england1.xml support_files/definitions/league_england2.xml support_files/definitions/league_england3.xml support_files/definitions/league_england4.xml support_files/definitions/league_england5.xml support_files/definitions/league_greece1_short.xml support_files/definitions/league_denmark1_short.xml support_files/definitions/cup_england_fa.xml support_files/definitions/cup_champ_league.xml support_files/definitions/league_ukraine1_short.xml support_files/definitions/league_switzerland1_short.xml support_files/definitions/cup_cwc.xml support_files/definitions/league_netherlands1_short.xml support_files/definitions/league_europe_div.xml support_files/definitions/league_czech1_short.xml support_files/definitions/cup_england_prom_games.xml support_files/definitions/league_portugal1_short.xml support_files/definitions/league_spain1_short.xml support_files/definitions/cup_uefa.xml support_files/definitions/cup_england_league.xml support_files/definitions/league_belgium1_short.xml support_files/definitions/league_france1_short.xml 

install-data-local:
	@$(NORMAL_INSTALL)
	for dir in $(SUPPORT_DIRS); do $(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/$$dir; done
	for file in $(SUPPORT_FILES); do $(INSTALL_DATA) $$file $(DESTDIR)$(pkgdatadir)/$$file; done

dist-hook:
	for dir in $(SUPPORT_DIRS); do mkdir $(distdir)/$$dir; done
	for file in $(SUPPORT_FILES); do cp -p $$file $(distdir)/$$file; done

uninstall:
	rm -rf $(DESTDIR)$(pkgdatadir)
	rm -rf $(prefix)/bin/bygfoot*
	rm -rf $(prefix)/man/man6/bygfoot.6
	for dir in $(prefix)/share/locale/*; do rm -rf $$dir/LC_MESSAGES/bygfoot*; done

doc:
	doxygen Doxyfile 1> /dev/null

doc-verbose:
	doxygen Doxyfile

doc-clean:
	rm -rf docs/*
