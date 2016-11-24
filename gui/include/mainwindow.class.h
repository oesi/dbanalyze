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
#include "worker.class.h"
#include "tablelist.class.h"
#include "dbaclutter.class.h"
#include <gtkmm.h>
#include <iostream>

class MainWindow : public Gtk::Window
{
public:
	MainWindow(const Glib::RefPtr<Gtk::Application>& app);
	virtual ~MainWindow();
	void drawGraph();
	void notify();
	void addDatabaseEntrys();
	void on_worker_notification();
	void on_button_connect_clicked();
	void on_infobar_response(int response);
	void on_headerbarconnect_button_clicked();
	void on_headerbarexport_button_clicked();
	void setStatistic();

	dbanalyze m_db;
	tablelist m_tablelist;
	dbaclutter m_clutterstage;
	Glib::Dispatcher m_Dispatcher;
	std::thread* m_WorkerThread;
	Worker m_Worker;

	Gtk::SearchBar m_SearchBarConnection;
	Gtk::Box m_BoxConnection;
	Gtk::Box m_VBoxMain;
	Gtk::Box m_BoxTable;
	Gtk::Box m_BoxNotebookDatabase;
	Gtk::Box m_BoxChart;
	Gtk::Paned m_PanedMain;
	Gtk::Button m_ButtonHeaderbarConnect;
	Gtk::Button m_ButtonHeaderbarExport;
	Gtk::Button m_ButtonConnect;
	Gtk::HeaderBar m_HeaderBar;
	Gtk::Entry m_EntryUser;
	Gtk::Entry m_EntryPass;
	Gtk::Entry m_EntryPort;
	Gtk::Entry m_EntryHost;
	Gtk::Entry m_EntryDB;
	Gtk::ComboBoxText m_ComboBoxTextDbtype;
	Gtk::Grid m_GridConnect;
	Gtk::InfoBar m_InfoBar;
	Gtk::Label m_LabelInfobar;
	Gtk::Image m_Image;
	Gtk::ScrolledWindow m_ScrollWindowGraph;
	Gtk::ScrolledWindow m_ScrolledWindowStatistic;
	Gtk::TextView m_TextViewStatistic;
	Glib::RefPtr<Gtk::TextBuffer> m_refTextBufferStatistic;
	Gtk::Notebook m_Notebook;
};

#endif
