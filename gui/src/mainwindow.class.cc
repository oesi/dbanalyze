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
#include "mainwindow.class.h"
#include "tablelist.class.h"
#include "graph.class.h"
#include <iostream>
#include <string>
#include <exception>
#include <sstream>
#include <thread>
#include <mutex>

MainWindow::MainWindow(const Glib::RefPtr<Gtk::Application>& app)
: 	tl(this),
	m_VBox(Gtk::ORIENTATION_VERTICAL, 6),
	m_Tablebox(Gtk::ORIENTATION_VERTICAL),
	m_HBoxTable(Gtk::ORIENTATION_HORIZONTAL),
	clutterstage(this)
{
	// Cast app to void to remove compiler warning
	(void) app;

	m_WorkerThread=NULL;
	set_title("DBAnalyze");
	set_size_request(800, 600);

	// Button go-home-symbolic
	m_headerbar_button.set_image_from_icon_name("document-open-symbolic", Gtk::ICON_SIZE_BUTTON, true);
	m_headerbarexport_button.set_image_from_icon_name("document-save-symbolic", Gtk::ICON_SIZE_BUTTON, true);
	m_statistic_button.set_image_from_icon_name("dialog-information-symbolic", Gtk::ICON_SIZE_BUTTON, true);
	m_headerbar_button.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_headerbar_button_clicked));
	m_drawgraph_button.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_drawgraph_button_clicked));
	m_headerbarexport_button.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_headerbarexport_button_clicked));
	m_statistic_button.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_statistic_button_clicked));
	m_Dispatcher.connect(sigc::mem_fun(*this, &MainWindow::on_worker_notification));

	m_header_bar.set_title("DBAnalyze");
	m_header_bar.set_subtitle("Connect to a Database");
	m_header_bar.set_show_close_button();
	m_header_bar.pack_start(m_headerbar_button);
	m_header_bar.pack_start(m_headerbarexport_button);
	m_header_bar.pack_start(m_statistic_button);

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

	m_Tablebox.pack_start(tl);

	m_drawgraph_button.set_label("Redraw Graph");
	m_Tablebox.pack_start(m_drawgraph_button,Gtk::PACK_SHRINK);

	m_VBox.pack_start(m_HBoxTable);
	m_HBoxTable.pack1(m_Tablebox);
	m_scrollwindowgraph.add(m_image);
	m_image.set("dbanalyze.png");
	//Only show the scrollbars when they are necessary:
	m_scrollwindowgraph.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

	m_Notebook.append_page(m_scrollwindowgraph, "Graph");
	m_Notebook.append_page(clutterstage, "ClutterStage");
	m_Notebook.set_scrollable();
	m_HBoxTable.pack2(m_Notebook);
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

	m_EntryPass.signal_activate().connect( sigc::mem_fun(*this,
	&MainWindow::on_button_connect_clicked) );

	m_EntryPass.grab_focus();

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

void MainWindow::on_statistic_button_clicked()
{
	Gtk::MessageDialog dialog(*this, "statistic");

	statistic stat;
	stat.analyze(this->db.getTablelist());

	std::stringstream outmsg;
	outmsg << "Tables without PK:" << stat.tables_without_pk.size() << std::endl;
	for(unsigned int j = 0; j < stat.tables_without_pk.size(); j++)
	{
		outmsg << "\t" << stat.tables_without_pk[j]->schemaname + "." + stat.tables_without_pk[j]->tablename << std::endl;
	}
	outmsg << "Empty Tables:" << stat.empty_tables.size() << std::endl;
	for(unsigned int j = 0; j < stat.empty_tables.size(); j++)
	{
		outmsg << "\t" << stat.empty_tables[j]->schemaname + "." + stat.empty_tables[j]->tablename << std::endl;
	}

	outmsg << "FK datatype missmatch:" << stat.fk_datatype_missmatch.size() << std::endl;
	for(unsigned int j = 0; j < stat.fk_datatype_missmatch.size(); j++)
	{
		constraint_fk* fk = dynamic_cast< constraint_fk* >( stat.fk_datatype_missmatch[j] );
		column *sourcecol = static_cast<column*>(fk->source);

		outmsg << "\t" << stat.fk_datatype_missmatch[j]->constraint_schema + "." + stat.fk_datatype_missmatch[j]->constraint_name + "." + sourcecol->columnname << std::endl;
	}

	outmsg << "Number of Tables:" << stat.num_tables << std::endl;
	outmsg << "Number of Columns: " << stat.num_columns << std::endl;
	outmsg << "Number of FK: " << stat.num_fk << std::endl;
	outmsg << "Number of UK: " << stat.num_uk << std::endl;

	std::cout << std::endl;

	outmsg << "Score: " << std::endl;
	outmsg << "Maximum Score: " << stat.maximum_score << std::endl;
	outmsg << "Score: " << stat.score << std::endl;
	outmsg << "Percent: " << 100/stat.maximum_score*stat.score << std::endl;

	dialog.set_secondary_text(outmsg.str());

	dialog.run();
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

void MainWindow::on_headerbarexport_button_clicked()
{

	int page = m_Notebook.get_current_page();

	Gtk::FileChooserDialog dialog("Please choose a file",
		Gtk::FILE_CHOOSER_ACTION_SAVE);
	dialog.set_transient_for(*this);

	//Add response buttons the the dialog:
	dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
	dialog.add_button("_Save", Gtk::RESPONSE_OK);

	auto filter_pdf = Gtk::FileFilter::create();
	if(page==0)
	{
		filter_pdf->set_name("PDF files");
		filter_pdf->add_mime_type("application/pdf");
		dialog.add_filter(filter_pdf);
	}

	auto filter_png = Gtk::FileFilter::create();
	filter_png->set_name("PNG files");
	filter_png->add_mime_type("image/png");
	dialog.add_filter(filter_png);

	//Show the dialog and wait for a user response:
	int result = dialog.run();

	//Handle the response:
	switch(result)
	{
		case(Gtk::RESPONSE_OK):
		{

			std::string filename = dialog.get_filename();
			auto filter = dialog.get_filter();
			std::string format = "pdf";
			std::map<std::string, std::map<std::string, std::string>> selecteditems;


			if(filter == filter_pdf)
				format = "pdf";
			else if(filter == filter_png)
				format = "png";

			if(page==0)
			{
				// Create a vector with all selected tables
				selecteditems = tl.getSelected();
				std::vector<table> tablelist;
				for(auto & i:selecteditems)
				{
					table *tbl = this->db.getTable(i.second["schemaname"],i.second["tablename"]);
					if(tbl)
						tablelist.push_back(*tbl);
				}

				// Create a graph with the selected tables
				graph g(&tablelist);
				g.write(filename,format);
				break;
			}
			else
				clutterstage.export_graph(filename, format);

		}
		case(Gtk::RESPONSE_CANCEL):
		{
			std::cout << "Cancel clicked." << std::endl;
			break;
		}
		default:
		{
			std::cout << "Unexpected button clicked." << std::endl;
			break;
		}
	}
}

void MainWindow::drawGraph()
{
	std::map<std::string, std::map<std::string, std::string>> selecteditems;

	// Create a vector with all selected tables
	selecteditems = tl.getSelected();
	std::vector<table> tablelist;
	for(auto & i:selecteditems)
	{
		table *tbl = this->db.getTable(i.second["schemaname"],i.second["tablename"]);
		if(tbl)
			tablelist.push_back(*tbl);
	}

	// Create a graph with the selected tables
	graph g(&tablelist);

	// Convert the Graph to a Pixbuf
	image_type buf = g.getImage();

	Glib::RefPtr<Gdk::Pixbuf> pixbuf;
	Glib::RefPtr<Gdk::PixbufLoader> loader;
	loader = Gdk::PixbufLoader::create();

	try
	{
		loader->write((const guint8*)buf.image_buffer, buf.image_size);
	}
	catch(Gdk::PixbufError e)
	{
		std::cout << e.what() << std::endl;
	}

	loader->close();
	pixbuf = loader->get_pixbuf();

	// Scale Image if it is bigger than the viewport
	int height = pixbuf->get_height();
	int width = pixbuf->get_width();

	float maxwidth = m_image.get_allocated_width() + 0.0;
	float maxheight = m_image.get_allocated_height() + 0.0;

	if(height>maxheight || width>maxwidth)
	{
		float scalefactor = height/maxheight;
		if(scalefactor<width/maxwidth)
			scalefactor = width/maxwidth;
		pixbuf = pixbuf->scale_simple(width/scalefactor,height/scalefactor,Gdk::INTERP_BILINEAR);
	}

	if(selecteditems.size() == 0)
		m_image.set("dbanalyze.png");
	else
		m_image.set(pixbuf);

	// Draw clutter Stage
	clutterstage.draw(&tablelist);
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