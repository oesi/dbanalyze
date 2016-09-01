#ifndef CONSTRAINTCLASS
#define CONSTRAINTCLASS

#include "column.class.h"
#include <string>
#include <vector>

struct ConstraintData
{
	std::string schema;
	std::string table;
	std::string column;
	ConstraintData(std::string s, std::string t, std::string c){schema = s;table = t;column = c;};
	~ConstraintData(){};
};

class constraint
{
	public:
		constraint();
		constraint(std::string constraint_schema, std::string constraint_name);
		virtual ~constraint();
		void addSource(std::string table_schema, std::string table_name, std::string table_column);

		std::vector<ConstraintData> source;
		std::string constraint_schema;
		std::string constraint_name;
};

#endif //CONSTRAINTCLASS