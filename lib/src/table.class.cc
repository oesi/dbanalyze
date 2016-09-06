#include "table.class.h"
#include "constraint_fk.class.h"
#include "constraint_uk.class.h"

table::table(std::string schemaname, std::string tablename)
{
	this->schemaname = schemaname;
	this->tablename = tablename;
}

std::vector<constraint*> table::getConstraints(std::string schema, std::string table, std::string column)
{
	std::vector<constraint*> retval;
	for(unsigned int i=0;i < this->constraintlist.size();i++)
	{
		for(unsigned int j=0;j < this->constraintlist[i]->source.size();j++)
		{
			if(this->constraintlist[i]->source[j].schema==schema
				&& this->constraintlist[i]->source[j].table==table
				&& this->constraintlist[i]->source[j].column==column)
			{
				retval.push_back(this->constraintlist[i]);
			}
		}
	}
	return retval;
}