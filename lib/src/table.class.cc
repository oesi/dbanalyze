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
	std::cout << this->schemaname << "." << this->tablename << std::endl;

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
	std::cout << " ) ";

	for(unsigned int i=0;i < this->constraintlist.size();i++)
	{
		this->constraintlist[i]->output();
		std::cout << " ";
	}
	std::cout << std::endl;

	for(unsigned int i=0;i < this->columnlist.size();i++)
	{
		this->columnlist[i].output();
	}
}