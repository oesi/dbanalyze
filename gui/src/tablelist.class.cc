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
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "tablelist.class.h"
#include "mainwindow.class.h"
#define DBANALYZE_CONFIG_MAX_DEFAULT_SELECTED_ITEMS 5
using std::sprintf;
using std::strtol;

tablelist::tablelist(void *mw)
: m_VBox(Gtk::ORIENTATION_VERTICAL)
{
	this->mw = mw;
	add(m_VBox);
	//Add the TreeView
	m_VBox.pack_start(m_TreeView);

	//Only show the scrollbars when they are necessary:
	set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

	//Create the Tree model:
	m_refTreeModel = Gtk::ListStore::create(m_Columns);
	m_TreeView.set_model(m_refTreeModel);

	int cols_count =m_TreeView.append_column_editable("Select", m_Columns.m_col_selected);

	Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count-1);
	if(pColumn)
	{
		Gtk::CellRendererToggle* pRenderer =
			static_cast<Gtk::CellRendererToggle*>(pColumn->get_first_cell());
		pRenderer->signal_toggled().connect(sigc::mem_fun(*this, &tablelist::on_select_toggled));
	}

	m_TreeView.append_column("TableName", m_Columns.m_col_tablename);
	m_TreeView.append_column("Schema", m_Columns.m_col_schemaname);
}

void tablelist::on_select_toggled(Glib::ustring)
{
	MainWindow *mw = static_cast<MainWindow*>(this->mw);
	mw->drawGraph();
}

void tablelist::fillTable(std::vector<table>* tables)
{
	// Remove old Items from Table
	m_refTreeModel->clear();

	for(unsigned int i=0; i < tables->size(); i++)
	{
		//Fill the TreeView's model
		Gtk::TreeModel::Row row = *(m_refTreeModel->append());
		row[m_Columns.m_col_tablename] = tables->at(i).tablename;
		row[m_Columns.m_col_schemaname] = tables->at(i).schemaname;

		if(i<DBANALYZE_CONFIG_MAX_DEFAULT_SELECTED_ITEMS)
			row[m_Columns.m_col_selected] = true;
		else
			row[m_Columns.m_col_selected] = false;
	}

}

std::map<std::string, std::map<std::string, std::string>> tablelist::getSelected()
{
	std::map<std::string, std::map<std::string, std::string>> selecteditems;

	typedef Gtk::TreeModel::Children type_children; //minimise code length.
	type_children children = m_refTreeModel->children();
	for(type_children::iterator iter = children.begin();iter != children.end(); ++iter)
	{
		Gtk::TreeModel::Row row = *iter;
		if(row[m_Columns.m_col_selected])
		{
			Glib::ustring schema = row[m_Columns.m_col_schemaname];
			Glib::ustring table = row[m_Columns.m_col_tablename];
			std::string key = schema+"_"+table;

			selecteditems[key]["tablename"]=table;
			selecteditems[key]["schemaname"]=schema;
		}
	}
	return selecteditems;
}

tablelist::~tablelist()
{
}
