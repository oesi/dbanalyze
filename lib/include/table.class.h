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
		table(std::string schemaname, std::string tablename, std::string comment);
		std::vector<constraint*> getConstraints(std::string schema,std::string table,std::string column);

		std::string tablename;
		std::string schemaname;
		std::string comment;
		std::vector<column> columnlist;
		std::vector<std::string> pk;
		std::vector<constraint*> constraintlist;
		long row_count;
};

#endif //TABLECLASS