#ifndef DATABASECLASS
#define DATABASECLASS

#include "table.class.h"
#include <string>
#include <vector>
#include <libgda/libgda.h>
#include <sql-parser/gda-sql-parser.h>


struct statistic
{
	int num_tables;
	std::vector<table*> tables_without_pk;
	int num_columns;
	int num_fk;
	int num_uk;
};

class database
{
	public:
		database(std::string type, std::string host, int port, std::string user, std::string password, std::string dbname);
		~database();
		bool loadTables();
		void output();
		void loadColumns();
		void analyze();
		void loadConstraints();
		table* getTable(std::string schemaname, std::string tablename);

	private:
		std::vector<table> tablelist;
		GdaConnection *dbconn;
		statistic stat;
};

#endif //DATABASECLASS