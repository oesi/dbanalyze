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
#ifndef DBA_TABLELIST_H
#define DBA_TABLELIST_H

#include <gtkmm.h>
#include "table.class.h"


class tablelist : public Gtk::ScrolledWindow
{
public:
	tablelist(void *mw);
	virtual ~tablelist();
	void fillTable(std::vector<table>* tables);
	std::map<std::string, std::map<std::string, std::string>> getSelected();

protected:
	void *mw;
	//Signal handlers:
	void on_button_quit();
	void on_select_toggled(Glib::ustring);

	void treeviewcolumn_validated_on_cell_data(Gtk::CellRenderer* renderer, const Gtk::TreeModel::iterator& iter);
	void cellrenderer_validated_on_editing_started(Gtk::CellEditable* cell_editable, const Glib::ustring& path);
	void cellrenderer_validated_on_edited(const Glib::ustring& path_string, const Glib::ustring& new_text);

	//Tree model columns:
	class ModelColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:

		ModelColumns()
		{ add(m_col_tablename); add(m_col_schemaname); add(m_col_selected); }

		Gtk::TreeModelColumn<Glib::ustring> m_col_tablename;
		Gtk::TreeModelColumn<Glib::ustring> m_col_schemaname;
		Gtk::TreeModelColumn<bool> m_col_selected;

	};

	ModelColumns m_Columns;

	Gtk::Box m_VBox;
	Gtk::ScrolledWindow m_ScrolledWindow;
	Gtk::TreeView m_TreeView;
	Glib::RefPtr<Gtk::TreeStore> m_refTreeModel;
};

#endif //DBA_TABLELIST_H