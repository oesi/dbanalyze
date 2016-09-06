#ifndef DBANALYZECLASS
#define DBANALYZECLASS
#include "database.class.h"
#include "table.class.h"
#include <string>
#include <vector>
#include <libgda/libgda.h>
#include <sql-parser/gda-sql-parser.h>

class dbanalyze
{
	public:
		dbanalyze(std::string type, std::string host, int port, std::string user, std::string password, std::string dbname);
		~dbanalyze();
		void loadData();
		void loadTables();
		void loadColumns();
		void loadConstraints();
		void loadTableSize();
		
		table* getTable(std::string schemaname, std::string tablename);
		std::vector<table>* getTablelist(){ return &tablelist; }

	private:
		std::vector<table> tablelist;
		database *db;
};

#endif //DBANALYZECLASS