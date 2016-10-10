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
#include <iostream>
#include <string>
#include <exception>
#include <sstream>
#include "dbanalyze.h"


MainWindow::MainWindow(const Glib::RefPtr<Gtk::Application>& app)
: 	m_VBox(Gtk::ORIENTATION_VERTICAL, 6)
{
	set_title("DBAnalyze");
	set_size_request(800, 600);

	// Button go-home-symbolic
	m_headerbar_button.set_image_from_icon_name("document-open-symbolic", Gtk::ICON_SIZE_BUTTON, true);
	m_headerbar_button.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_headerbar_button_clicked));

	m_header_bar.set_title("DBAnalyze");
	m_header_bar.set_subtitle("Choose a Database");
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

	m_VBox.pack_start(m_Notebook);

	//Add the Notebook pages:
	m_Notebook.append_page(m_BoxNotebookDatabase, "1. Choose Database");

	addDatabaseEntrys();
	show_all_children();

	m_InfoBar.hide();
}

void MainWindow::addDatabaseEntrys()
{
	m_BoxNotebookDatabase.pack_start(m_grid);

	// Type
	m_ComboDbtype.append("PostgreSQL");
	m_ComboDbtype.append("SQLite");
	m_ComboDbtype.append("MySQL");
	m_ComboDbtype.append("MSAccess");
	m_ComboDbtype.set_active(0);
	m_grid.add(m_ComboDbtype);

	// Host
	m_EntryHost.set_max_length(50);
	m_EntryHost.set_text("Host");
	m_grid.add(m_EntryHost);

	// Port
	m_EntryPort.set_max_length(50);
	m_EntryPort.set_text("Port");
	m_grid.add(m_EntryPort);

	// DB
	m_EntryDB.set_max_length(50);
	m_EntryDB.set_text("DB Name");
	m_grid.add(m_EntryDB);

	// User
	m_EntryUser.set_max_length(50);
	m_EntryUser.set_text("User");
	m_grid.add(m_EntryUser);

	// Pass
	m_EntryPass.set_max_length(50);
	m_EntryPass.set_text("Password");
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

void MainWindow::on_button_connect_clicked()
{
	m_Message_Label.set_text("Connecting ...");
	m_InfoBar.set_message_type(Gtk::MESSAGE_WARNING);
	m_InfoBar.show();

	std::string type = m_ComboDbtype.get_active_text();
	std::string host = m_EntryHost.get_text();
	std::string dbname = m_EntryDB.get_text();
	int port = std::stoi(m_EntryPort.get_text());
	std::string user = m_EntryUser.get_text();
	std::string password = m_EntryPass.get_text();

	dbanalyze dba(type, host, port, user, password, dbname);
	m_Message_Label.set_text("Loading Data ...");
	m_InfoBar.show();
	dba.loadData();
	m_Message_Label.set_text("Data loaded successfully!");
	m_InfoBar.show();

}

void MainWindow::on_headerbar_button_clicked()
{
	Gtk::FileChooserDialog dialog("Please choose a file", Gtk::FILE_CHOOSER_ACTION_OPEN);
	dialog.set_transient_for(*this);

	//Add response buttons the the dialog:
	dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
	dialog.add_button("_Open", Gtk::RESPONSE_OK);

}

MainWindow::~MainWindow()
{
}
