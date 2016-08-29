#include "table.class.h"
#include "log.h"
#include "constraint_fk.class.h"
#include "constraint_uk.class.h"
#include <iostream>
#include <sstream>

table::table(std::string schemaname, std::string tablename)
{
	this->schemaname = schemaname;
	this->tablename = tablename;
}

void table::output()
{
	std::cout << this->schemaname << "." << this->tablename;

	std::cout << " ( PK:";
	if(this->pk.size()==0)
	{
		std::cout << " missing";
	}
	else
	{
		for(unsigned int i=0;i < this->pk.size();i++)
		{
			std::cout << " " << this->pk[i];
		}
	}
	std::cout << " ) " << std::endl;

	for(unsigned int i=0;i < this->columnlist.size();i++)
	{
		this->columnlist[i].output();
		std::vector<constraint*> colconstr = this->getConstraints(this->schemaname, this->tablename, this->columnlist[i].getColumnname());
		for(unsigned int j=0;j < colconstr.size();j++)
		{
			colconstr[j]->output();
		}
		std::cout << std::endl;
	}
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