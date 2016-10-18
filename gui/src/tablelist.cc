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
#include "tablelist.h"

using std::sprintf;
using std::strtol;

tablelist::tablelist()
: m_VBox(Gtk::ORIENTATION_VERTICAL)
{

	add(m_VBox);
	//Add the TreeView
	m_VBox.pack_start(m_TreeView);

	//Only show the scrollbars when they are necessary:
	set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

	//Create the Tree model:
	m_refTreeModel = Gtk::ListStore::create(m_Columns);
	m_TreeView.set_model(m_refTreeModel);

	//Add the TreeView's view columns:
	m_TreeView.append_column_editable("Select", m_Columns.m_col_selected);
	m_TreeView.append_column("TableName", m_Columns.m_col_tablename);
	m_TreeView.append_column("Schema", m_Columns.m_col_schemaname);
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
		row[m_Columns.m_col_selected] = true;
	}

}

tablelist::~tablelist()
{
}
