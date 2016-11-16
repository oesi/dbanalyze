/* Copyright (C) 2016 Andreas Österreicher
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */
#ifndef DBA_CLUTTER_H
#define DBA_CLUTTER_H

#include <gtkmm.h>
#include <clutter/clutter.h>
#include <clutter-gtk/clutter-gtk.h>
#include "table.class.h"
#include "constraint_fk.class.h"

class dbaclutter : public Gtk::ScrolledWindow
{
public:
	dbaclutter(void *mw);
	virtual ~dbaclutter();
	void draw(std::vector<table> *tablelist);

	void drawtable(table *tbl);
	void export_graph(std::string filename, std::string format);

	void drawLine(std::string name, ClutterActor *actor1, ClutterActor *actor2);
	void redrawLine(ClutterActor *line, ClutterActor *actor1, ClutterActor *actor2);
	void get_line_position(ClutterActor *actor1, ClutterActor *actor2, float *x, float *y, float *len, float *angle);
	void helper_clutter_actor_get_center_position(ClutterActor *actor, float *x, float *y);

	std::map<std::string,constraint_fk*> constraintlist;


protected:
	void *mw;
	GtkWidget *clutter0;
	ClutterActor *stage;
	Glib::RefPtr<Gdk::Pixbuf> pixbuf;
	gfloat last_x_position, last_y_position, next_x_position;

//private:
	//static void on_actor_dragmotion (ClutterDragAction *action,ClutterActor *actor,gfloat delta_x,gfloat delta_y,gpointer user_data);
};

#endif //DBA_CLUTTER_H