## Process this file with automake to produce Makefile.in
AM_CFLAGS = -Wall

INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	@PACKAGE_CFLAGS@

bin_PROGRAMS = bygfoot-update-gui
bin_SCRIPTS = bygfoot-update

bygfoot_update_gui_SOURCES = \
	bu_main.c \
	bu_support.c bu_support.h \
	bu_interface.c bu_interface.h \
	bu_callbacks.c bu_callbacks.h bu_functions.h \
	bu_functions.c bu_functions.h bu_interface.h bu_support.h bu_callbacks.h

bygfoot_update_gui_LDADD = @PACKAGE_LIBS@ $(INTLLIBS)
