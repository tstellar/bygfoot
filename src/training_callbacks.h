/*
   bet_struct.h

   Bygfoot Football Manager -- a small and simple GTK2-based
   football management game.

   http://bygfoot.sourceforge.net

   Copyright (C) 2005  Gyözö Both (gyboth@bygfoot.com)

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <gtk/gtk.h>


void
on_b_cancel_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_b_ok_clicked                        (GtkButton       *button,
                                        gpointer         user_data);

void
on_rb_camp3_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_rb_camp2_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_rb_camp1_clicked                    (GtkButton       *button,
										gpointer         user_data);

void
on_b_dec_recreation_clicked            (GtkButton       *button,
                                        gpointer         user_data);

void
on_b_inc_recreation_clicked            (GtkButton       *button,
                                        gpointer         user_data);

void
on_b_dec_training_clicked              (GtkButton       *button,
                                        gpointer         user_data);

void
on_b_inc_training_clicked              (GtkButton       *button,
                                        gpointer         user_data);
