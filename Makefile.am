## Process this file with automake to produce Makefile.in

AM_CFLAGS = -Wall

SUBDIRS = src po b-u

EXTRA_DIST = \
	autogen.sh \
	bygfoot.glade \
	bygfoot.gladep

dist_man6_MANS = man/bygfoot.6

SUPPORT_DIRS = support_files support_files/text_files support_files/pixmaps 

SUPPORT_FILES = support_files/text_files/help support_files/text_files/country_br support_files/text_files/country_de support_files/text_files/country_es support_files/text_files/country_fr support_files/text_files/country_hu support_files/text_files/country_it support_files/text_files/country_mx support_files/text_files/country_pl support_files/text_files/country_ro support_files/text_files/teams support_files/text_files/country_eng support_files/text_files/bygfoot.conf support_files/text_files/player_names support_files/pixmaps/flag_ro.png support_files/pixmaps/bygfoot_team_editor_icon.png support_files/pixmaps/bygfoot_icon.png support_files/pixmaps/flag_eng.png support_files/pixmaps/pay_loan.png support_files/pixmaps/inc_cap.png support_files/pixmaps/new_week.png support_files/pixmaps/preview.png support_files/pixmaps/get_loan.png support_files/pixmaps/transfers.png support_files/pixmaps/fire_player.png support_files/pixmaps/inc_saf.png support_files/pixmaps/football.png support_files/pixmaps/flag_br.png support_files/pixmaps/flag_de.png support_files/pixmaps/fin_stad.png support_files/pixmaps/flag_es.png support_files/pixmaps/flag_fr.png support_files/pixmaps/flag_hu.png support_files/pixmaps/flag_it.png support_files/pixmaps/flag_mx.png support_files/pixmaps/flag_pl.png 

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
