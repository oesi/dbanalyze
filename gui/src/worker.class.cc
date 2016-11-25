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
#include "worker.class.h"
#include "mainwindow.class.h"
#include <sstream>
#include <chrono>

Worker::Worker() :
	m_Mutex(),
	m_has_stopped(false),
	m_message()
{
}

void Worker::get_data(std::string* msg)
{
	std::lock_guard<std::mutex> lock(m_Mutex);

	if (msg)
		*msg = m_message;
}

bool Worker::has_stopped() const
{
	std::lock_guard<std::mutex> lock(m_Mutex);
	return m_has_stopped;
}

void Worker::do_work(void* caller)
{
	m_has_stopped = false;
	MainWindow *mw = static_cast<MainWindow*>(caller);

	m_message = "Connecting...";
	mw->notify();

	std::string type = mw->m_ComboBoxTextDbtype.get_active_text();
	std::string host = mw->m_EntryHost.get_text();
	std::string dbname = mw->m_EntryDB.get_text();
	int port = std::stoi(mw->m_EntryPort.get_text());
	std::string user = mw->m_EntryUser.get_text();
	std::string password = mw->m_EntryPass.get_text();

	if(mw->m_db.connect(type, host, port, user, password, dbname))
	{
		m_message = "Loading Data...";
		mw->notify();

		mw->m_db.loadData(true);
		m_message = "Data loaded!";

	}
	else
	{
		m_message = "Failed!";
	}

	m_has_stopped = true;
	mw->notify();
}
