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
		bool query(std::string sql);
		std::string get(std::string columnname);
		bool nextRow();
		const GValue *getRecord(std::string columnname);

	public:
		GdaConnection *dbconn;
		GdaDataModel *data_model;
		gint row_id;
};

#endif //DATABASECLASS