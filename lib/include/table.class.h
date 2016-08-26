#ifndef TABLECLASS
#define TABLECLASS

#include "column.class.h"
#include "constraint.class.h"

#include <string>
#include <vector>
#include <libgda/libgda.h>
#include <sql-parser/gda-sql-parser.h>

class table
{
	public:
		table(std::string schemaname, std::string tablename);
		void output();
		bool loadColumns(GdaConnection *dbconn);
		bool loadConstraints(GdaConnection *dbconn);

	private:
		std::string tablename;
		std::string schemaname;
		std::vector<column> columnlist;
		std::vector<std::string> pk;
		std::vector<constraint> constraintlist;
};

#endif //TABLECLASS