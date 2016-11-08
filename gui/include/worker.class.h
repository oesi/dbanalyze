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

#ifndef WORKER_H
#define WORKER_H

#include <gtkmm.h>
#include <thread>
#include <mutex>

class Worker
{
public:
	Worker();

	// Thread function.
	void do_work(void* caller);
	void get_data(std::string* msg);
	bool has_stopped() const;

private:
	// Synchronizes access to member data.
	mutable std::mutex m_Mutex;

	// Data used by both GUI thread and worker thread.
	bool m_has_stopped;
	std::string m_message;
};

#endif // WORKER_H
