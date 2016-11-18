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
	clutterstage(this),
	m_VBoxMain(Gtk::ORIENTATION_VERTICAL, 6),
	m_BoxTable(Gtk::ORIENTATION_VERTICAL),
	m_PanedMain(Gtk::ORIENTATION_HORIZONTAL)
{
	// Cast app to void to remove compiler warning
	(void) app;

	m_WorkerThread=NULL;
	set_title("DBAnalyze");
	set_size_request(800, 600);

	m_ButtonHeaderbarConnect.set_image_from_icon_name("document-open-symbolic", Gtk::ICON_SIZE_BUTTON, true);
	m_ButtonHeaderbarExport.set_image_from_icon_name("document-save-symbolic", Gtk::ICON_SIZE_BUTTON, true);
	m_ButtonHeaderbarStatistic.set_image_from_icon_name("dialog-information-symbolic", Gtk::ICON_SIZE_BUTTON, true);
	m_ButtonHeaderbarConnect.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_headerbarconnect_button_clicked));
	m_ButtonHeaderbarExport.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_headerbarexport_button_clicked));
	m_ButtonHeaderbarStatistic.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_statistic_button_clicked));
	m_ButtonRedraw.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_redraw_button_clicked));
	m_Dispatcher.connect(sigc::mem_fun(*this, &MainWindow::on_worker_notification));

	m_HeaderBar.set_title("DBAnalyze");
	m_HeaderBar.set_subtitle("Connect to a Database");
	m_HeaderBar.set_show_close_button();
	m_HeaderBar.pack_start(m_ButtonHeaderbarConnect);
	m_HeaderBar.pack_start(m_ButtonHeaderbarExport);
	m_HeaderBar.pack_start(m_ButtonHeaderbarStatistic);

	// Set headerbar as titlebar
	set_titlebar(m_HeaderBar);

	add(m_VBoxMain);

	// Add the message label to the InfoBar:
	auto infoBarContainer = dynamic_cast<Gtk::Container*>(m_InfoBar.get_content_area());
	if (infoBarContainer)
		infoBarContainer->add(m_LabelInfobar);

	// Connect signals:
	m_InfoBar.signal_response().connect(sigc::mem_fun(*this, &MainWindow::on_infobar_response));

	// Add an ok button to the InfoBar:
	m_InfoBar.add_button("_OK", 0);

	m_VBoxMain.pack_start(m_InfoBar, Gtk::PACK_SHRINK);
	m_VBoxMain.pack_start(m_SearchBarConnection, Gtk::PACK_SHRINK);

	addDatabaseEntrys();

	m_BoxTable.pack_start(tl);

	m_ButtonRedraw.set_label("Redraw Graph");
	m_BoxTable.pack_start(m_ButtonRedraw,Gtk::PACK_SHRINK);

	m_VBoxMain.pack_start(m_PanedMain);
	m_PanedMain.pack1(m_BoxTable);
	m_ScrollWindowGraph.add(m_Image);
	m_Image.set("dbanalyze.png");
	//Only show the scrollbars when they are necessary:
	m_ScrollWindowGraph.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

	m_Notebook.append_page(m_ScrollWindowGraph, "Graph");
	m_Notebook.append_page(clutterstage, "ClutterStage");
	m_Notebook.set_scrollable();
	m_PanedMain.pack2(m_Notebook);
	m_PanedMain.set_position(250);

	show_all_children();
	m_InfoBar.hide();
}

void MainWindow::addDatabaseEntrys()
{
	m_SearchBarConnection.add(m_BoxConnection);
	m_BoxConnection.pack_start(m_GridConnect,Gtk::PACK_SHRINK);
	m_SearchBarConnection.set_search_mode(true);

	// Type
	m_ComboBoxTextDbtype.append("PostgreSQL");
	m_ComboBoxTextDbtype.append("SQLite");
	m_ComboBoxTextDbtype.append("MySQL");
	m_ComboBoxTextDbtype.append("MSAccess");
	m_ComboBoxTextDbtype.set_active(0);
	m_GridConnect.add(m_ComboBoxTextDbtype);

	// Host
	m_EntryHost.set_max_length(50);
	m_EntryHost.set_text("localhost");
	m_GridConnect.add(m_EntryHost);

	// Port
	m_EntryPort.set_max_length(50);
	m_EntryPort.set_text("5433");
	m_GridConnect.add(m_EntryPort);

	// DB
	m_EntryDB.set_max_length(50);
	m_EntryDB.set_text("analyzetest");
	m_GridConnect.add(m_EntryDB);

	// User
	m_EntryUser.set_max_length(50);
	m_EntryUser.set_text("dbanalyze");
	m_GridConnect.add(m_EntryUser);

	// Pass
	m_EntryPass.set_max_length(50);
	m_EntryPass.set_text("dbanalyze");
	m_EntryPass.set_visibility(false);
	m_GridConnect.add(m_EntryPass);

	m_EntryPass.signal_activate().connect( sigc::mem_fun(*this, &MainWindow::on_button_connect_clicked) );

	m_EntryPass.grab_focus();

	// Connect Button
	m_ButtonConnect.set_label("Connect");
	m_GridConnect.add(m_ButtonConnect);

	m_ButtonConnect.signal_clicked().connect( sigc::mem_fun(*this, &MainWindow::on_button_connect_clicked) );
}

void MainWindow::on_infobar_response(int)
{
	m_LabelInfobar.set_text("");
	m_InfoBar.hide();
}

void MainWindow::on_headerbarconnect_button_clicked()
{
	m_SearchBarConnection.set_search_mode(!m_SearchBarConnection.get_search_mode());
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

void MainWindow::on_redraw_button_clicked()
{
	this->drawGraph();
}

void MainWindow::on_headerbarexport_button_clicked()
{
	int page = m_Notebook.get_current_page();

	Gtk::FileChooserDialog dialog("Please choose a file", Gtk::FILE_CHOOSER_ACTION_SAVE);
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

	float maxwidth = m_Image.get_allocated_width() + 0.0;
	float maxheight = m_Image.get_allocated_height() + 0.0;

	if(height>maxheight || width>maxwidth)
	{
		float scalefactor = height/maxheight;
		if(scalefactor<width/maxwidth)
			scalefactor = width/maxwidth;
		pixbuf = pixbuf->scale_simple(width/scalefactor,height/scalefactor,Gdk::INTERP_BILINEAR);
	}

	if(selecteditems.size() == 0)
		m_Image.set("dbanalyze.png");
	else
		m_Image.set(pixbuf);

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
	m_HeaderBar.set_subtitle(msg);

	if (m_WorkerThread && m_Worker.has_stopped())
	{
		// Work is done.
		if (m_WorkerThread->joinable())
			m_WorkerThread->join();
		delete m_WorkerThread;
		m_WorkerThread = nullptr;
		tl.fillTable(this->db.getTablelist());

		on_redraw_button_clicked();
		m_SearchBarConnection.set_search_mode(false);
	}
}