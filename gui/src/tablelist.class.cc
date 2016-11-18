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
#include <exception>
#define DBANALYZE_CONFIG_MAX_DEFAULT_SELECTED_ITEMS 5

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
	m_refTreeModel = Gtk::TreeStore::create(m_Columns);
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

void tablelist::on_select_toggled(Glib::ustring path)
{
	// if clicked on schema -> select/unselect tables
	typedef Gtk::TreeModel::Children type_children;
	type_children children_schema = m_refTreeModel->children();
	for(type_children::iterator iter = children_schema.begin();iter != children_schema.end(); ++iter)
	{
		Gtk::TreeModel::Row row = *iter;

		Gtk::TreePath treepath;
		treepath = m_refTreeModel->get_path(row);
		if(path == treepath.to_string())
		{
			if(row[m_Columns.m_col_selected])
			{
				type_children children_tables = row->children();
				for(type_children::iterator itertable = children_tables.begin();itertable != children_tables.end(); ++itertable)
				{
					Gtk::TreeModel::Row row_table = *itertable;
					row_table[m_Columns.m_col_selected]=true;
				}
			}
			else
			{
				type_children children_tables = row->children();
				for(type_children::iterator itertable = children_tables.begin();itertable != children_tables.end(); ++itertable)
				{
					Gtk::TreeModel::Row row_table = *itertable;
					row_table[m_Columns.m_col_selected]=false;
				}
			}
		}
	}
	MainWindow *mw = static_cast<MainWindow*>(this->mw);
	mw->drawGraph();
}

void tablelist::fillTable(std::vector<table>* tables)
{
	// Remove old Items from Table
	m_refTreeModel->clear();
	std::map<std::string, Gtk::TreeModel::Row> schema;

	for(unsigned int i=0; i < tables->size(); i++)
	{
		Gtk::TreeModel::Row row_schema;
		if(schema[tables->at(i).schemaname]==NULL)
		{
			// Create Schema Entry
			row_schema = *(m_refTreeModel->append());
			row_schema[m_Columns.m_col_tablename] = tables->at(i).schemaname;
			row_schema[m_Columns.m_col_schemaname] = "";
			row_schema[m_Columns.m_col_selected] = false;
			schema[tables->at(i).schemaname] = row_schema;
		}

		//Fill the TreeView's model
		Gtk::TreeModel::Row row = *(m_refTreeModel->append(schema[tables->at(i).schemaname].children()));
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

	typedef Gtk::TreeModel::Children type_children;
	type_children children_schema = m_refTreeModel->children();
	for(type_children::iterator iter = children_schema.begin();iter != children_schema.end(); ++iter)
	{
		Gtk::TreeModel::Row row_schema = *iter;
		type_children children_tables = row_schema->children();

		for(type_children::iterator itertable = children_tables.begin();itertable != children_tables.end(); ++itertable)
		{
			Gtk::TreeModel::Row row_table = *itertable;
			if(row_table[m_Columns.m_col_selected])
			{
				Glib::ustring schema = row_table[m_Columns.m_col_schemaname];
				Glib::ustring table = row_table[m_Columns.m_col_tablename];
				std::string key = schema+"_"+table;

				selecteditems[key]["tablename"]=table;
				selecteditems[key]["schemaname"]=schema;
			}
		}
	}
	return selecteditems;
}

tablelist::~tablelist()
{
}
