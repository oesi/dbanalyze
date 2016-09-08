#include "table.class.h"
#include "constraint_fk.class.h"
#include "constraint_uk.class.h"
#include "column.class.h"

table::table(std::string schemaname, std::string tablename)
{
	this->schemaname = schemaname;
	this->tablename = tablename;
}

std::vector<constraint*> table::getConstraints(std::string schema, std::string table, std::string column_name)
{
	std::vector<constraint*> retval;
	for(unsigned int i=0;i < this->constraintlist.size();i++)
	{
		column* col = static_cast<column*>(this->constraintlist[i]->source);
		if(col->columnname==column_name)
		{
			retval.push_back(this->constraintlist[i]);
		}
	}
	return retval;
}