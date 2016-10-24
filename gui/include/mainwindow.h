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
#ifndef DBA_MAINWINDOW_H
#define DBA_MAINWINDOW_H

#include "dbanalyze.h"
#include "worker.h"
#include "tablelist.h"
#include <gtkmm.h>
#include <iostream>

class MainWindow : public Gtk::Window
{
public:
	MainWindow(const Glib::RefPtr<Gtk::Application>& app);
	virtual ~MainWindow();
	void drawGraph();

	void notify();
	void on_worker_notification();
	Glib::Dispatcher m_Dispatcher;
	std::thread* m_WorkerThread;
	Worker m_Worker;

	void addDatabaseEntrys();
	void on_button_connect_clicked();
	void on_infobar_response(int response);
	void on_headerbar_button_clicked();
	void on_drawgraph_button_clicked();

	dbanalyze db;

	tablelist tl;

	//Child widgets:
	Gtk::SearchBar m_search_bar;
	Gtk::Box m_searchbarbox;
	Gtk::Box m_VBox;
	Gtk::Box m_Tablebox;
	Gtk::Paned m_HBoxTable;
	Gtk::Box m_BoxNotebookDatabase, m_BoxChart;
	Gtk::Button m_headerbar_button,m_ButtonConnect;
	Gtk::HeaderBar m_header_bar;
	Gtk::Entry m_EntryUser, m_EntryPass, m_EntryPort, m_EntryHost, m_EntryDB;
	Gtk::Label m_Label1, m_Label2;
	Gtk::ComboBoxText m_ComboDbtype;
	Gtk::Grid m_grid;
	Glib::RefPtr<Gtk::Builder> m_refBuilder;
	Gtk::InfoBar m_InfoBar;
	Gtk::Label m_Message_Label;
	Gtk::Image m_image;

	Gtk::ButtonBox m_ButtonBox;
	Gtk::Button m_drawgraph_button;
	Gtk::ScrolledWindow m_scrollwindowgraph;

};

#endif
