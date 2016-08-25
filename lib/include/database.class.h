#ifndef DATABASECLASS
#define DATABASECLASS

#include "table.class.h"
#include <string>
#include <vector>
#include <libgda/libgda.h>
#include <sql-parser/gda-sql-parser.h>


class database
{
	public:
		database(std::string type, std::string host, int port, std::string user, std::string password, std::string dbname);
		~database();
		bool loadTables();
		void output();
		bool loadColumns();

	private:
		std::vector<table> tablelist;
		GdaConnection *dbconn;
};

#endif //DATABASECLASS