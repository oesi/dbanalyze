#ifndef DBA_DATABASE_CLASS
#define DBA_DATABASE_CLASS

#include "table.class.h"
#include <string>
#include <vector>
#include <libgda/libgda.h>
#include <sql-parser/gda-sql-parser.h>

class database
{
	public:
		database();
		~database();
		bool connect(std::string type, std::string host, int port, std::string user, std::string password, std::string dbname);
		bool query(std::string sql);
		bool queryMeta(std::string sql);
		bool nextRow();
		std::string get(std::string columnname);
		double getNumber(std::string columnname);
		void loadTables();
		void loadColumns();
		void loadConstraints();
		void getTableSize(std::string schemaname, std::string tablename);

	protected:
		const GValue *getRecord(std::string columnname);

		std::string dbname;
		std::string type;
		GdaConnection *dbconn;
		GdaDataModel *data_model;
		long row_id;
};

#endif //DBA_DATABASE_CLASS