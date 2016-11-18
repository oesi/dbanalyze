#ifndef DBA_TABLE_CLASS
#define DBA_TABLE_CLASS

#include "column.class.h"
#include "constraint.class.h"

#include <string>
#include <vector>

class table
{
	public:
		table(std::string schemaname, std::string tablename, std::string comment);
		std::vector<constraint*> getConstraints(std::string column);

		std::string tablename;
		std::string schemaname;
		std::string comment;
		std::vector<column> columnlist;
		std::vector<std::string> pk;
		std::vector<constraint*> constraintlist;
		long row_count;
};

#endif //DBA_TABLE_CLASS