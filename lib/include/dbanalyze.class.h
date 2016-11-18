#ifndef DBA_DBANALYZE_CLASS
#define DBA_DBANALYZE_CLASS
#include "table.class.h"
#include "graph.class.h"
#include <string>
#include <vector>

class database;

class dbanalyze
{
	public:
		dbanalyze();
		dbanalyze(std::string type, std::string host, int port, std::string user, std::string password, std::string dbname);
		~dbanalyze();
		bool connect(std::string type, std::string host, int port, std::string user, std::string password, std::string dbname);
		void loadData(bool withsize);
		void loadTables();
		void loadColumns();
		void loadConstraints();
		void loadTableSize();

		table* getTable(std::string schemaname, std::string tablename);
		column* getColumn(std::string schemaname, std::string tablename, std::string columnname);
		std::vector<table>* getTablelist(){ return &tablelist; }

	private:
		std::vector<table> tablelist;
		database *db = NULL;
};

#endif //DBA_DBANALYZE_CLASS