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
#include "mainwindow.h"
#include "tablelist.h"
#include "graph.h"
#include <iostream>
#include <string>
#include <exception>
#include <sstream>
#include <thread>
#include <mutex>

MainWindow::MainWindow(const Glib::RefPtr<Gtk::Application>& app)
: 	m_VBox(Gtk::ORIENTATION_VERTICAL, 6),
	m_Tablebox(Gtk::ORIENTATION_VERTICAL),
	m_HBoxTable(Gtk::ORIENTATION_HORIZONTAL),
	tl(this)
{
	// Cast app to void to remove compiler warning
	(void) app;

	m_WorkerThread=NULL;
	set_title("DBAnalyze");
	set_size_request(800, 600);

	// Button go-home-symbolic
	m_headerbar_button.set_image_from_icon_name("document-open-symbolic", Gtk::ICON_SIZE_BUTTON, true);
	m_headerbar_button.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_headerbar_button_clicked));
	m_drawgraph_button.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_drawgraph_button_clicked));
	m_Dispatcher.connect(sigc::mem_fun(*this, &MainWindow::on_worker_notification));

	m_header_bar.set_title("DBAnalyze");
	m_header_bar.set_subtitle("Connect to a Database");
	m_header_bar.set_show_close_button();
	m_header_bar.pack_start(m_headerbar_button);

	// Set headerbar as titlebar
	set_titlebar(m_header_bar);

	add(m_VBox);

	// Add the message label to the InfoBar:
	auto infoBarContainer =
		dynamic_cast<Gtk::Container*>(m_InfoBar.get_content_area());
	if (infoBarContainer)
		infoBarContainer->add(m_Message_Label);

	// Connect signals:
	m_InfoBar.signal_response().connect(sigc::mem_fun(*this,
		&MainWindow::on_infobar_response) );

	// Add an ok button to the InfoBar:
	m_InfoBar.add_button("_OK", 0);

	// Add the InfoBar to the vbox:
	m_VBox.pack_start(m_InfoBar, Gtk::PACK_SHRINK);
	m_VBox.pack_start(m_search_bar, Gtk::PACK_SHRINK);

	addDatabaseEntrys();

	m_drawgraph_button.set_label("Redraw Graph");
	m_Tablebox.pack_start(tl);
	m_Tablebox.pack_start(m_drawgraph_button,Gtk::PACK_SHRINK);

	m_VBox.pack_start(m_HBoxTable);
	m_HBoxTable.pack1(m_Tablebox);
	m_scrollwindowgraph.add(m_image);
	//Only show the scrollbars when they are necessary:
	m_scrollwindowgraph.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

	m_HBoxTable.pack2(m_scrollwindowgraph);
	m_HBoxTable.set_position(250);

	show_all_children();
	m_InfoBar.hide();

}

void MainWindow::addDatabaseEntrys()
{
	m_search_bar.add(m_searchbarbox);
	m_searchbarbox.pack_start(m_grid,Gtk::PACK_SHRINK);
	m_search_bar.set_search_mode(true);

	// Type
	m_ComboDbtype.append("PostgreSQL");
	m_ComboDbtype.append("SQLite");
	m_ComboDbtype.append("MySQL");
	m_ComboDbtype.append("MSAccess");
	m_ComboDbtype.set_active(0);
	m_grid.add(m_ComboDbtype);

	// Host
	m_EntryHost.set_max_length(50);
	m_EntryHost.set_text("localhost");
	m_grid.add(m_EntryHost);

	// Port
	m_EntryPort.set_max_length(50);
	m_EntryPort.set_text("5433");
	m_grid.add(m_EntryPort);

	// DB
	m_EntryDB.set_max_length(50);
	m_EntryDB.set_text("analyzetest");
	m_grid.add(m_EntryDB);

	// User
	m_EntryUser.set_max_length(50);
	m_EntryUser.set_text("dbanalyze");
	m_grid.add(m_EntryUser);

	// Pass
	m_EntryPass.set_max_length(50);
	m_EntryPass.set_text("dbanalyze");
	m_EntryPass.set_visibility(false);
	m_grid.add(m_EntryPass);

	// Connect Button
	m_ButtonConnect.set_label("Connect");
	m_grid.add(m_ButtonConnect);

	m_ButtonConnect.signal_clicked().connect( sigc::mem_fun(*this,
		&MainWindow::on_button_connect_clicked) );
}

void MainWindow::on_infobar_response(int)
{
	// Clear the message and hide the info bar:
	m_Message_Label.set_text("");
	m_InfoBar.hide();
}

void MainWindow::on_headerbar_button_clicked()
{
	m_search_bar.set_search_mode(!m_search_bar.get_search_mode());
}
void MainWindow::on_button_connect_clicked()
{

	if (m_WorkerThread)
	{
		std::cout << "Can't start a worker thread while another one is running." << std::endl;
	}
	else
	{
		// Start a new worker thread.
		m_WorkerThread = new std::thread(
		[this]
		{
			m_Worker.do_work(this);
		});
	}
}

void MainWindow::on_drawgraph_button_clicked()
{
	this->drawGraph();
}

void MainWindow::drawGraph()
{
	std::map<std::string, std::map<std::string, std::string>> selecteditems;
	selecteditems = tl.getSelected();

	std::vector<table> tablelist;
	for(auto & i:selecteditems)
	{
		table *tbl = this->db.getTable(i.second["schemaname"],i.second["tablename"]);
		if(tbl)
			tablelist.push_back(*tbl);
	}
	printGraph(&tablelist, "current");
	m_image.set("data/current.png");
}

MainWindow::~MainWindow()
{
}

void MainWindow::notify()
{
	m_Dispatcher.emit();
}

void MainWindow::on_worker_notification()
{
	std::string msg;
	m_Worker.get_data(&msg);
	m_header_bar.set_subtitle(msg);

	if (m_WorkerThread && m_Worker.has_stopped())
	{
		// Work is done.
		if (m_WorkerThread->joinable())
			m_WorkerThread->join();
		delete m_WorkerThread;
		m_WorkerThread = nullptr;
		tl.fillTable(this->db.getTablelist());

		on_drawgraph_button_clicked();
		m_search_bar.set_search_mode(false);
	}
}
