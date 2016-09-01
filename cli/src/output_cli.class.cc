#include "output_cli.class.h"


void output_cli::printConstraint(constraint* constr)
{
	std::cout << "⚷";
	for(unsigned int i=0;i < constr->source.size();i++)
	{
		std::cout << constr->constraint_name;
	}
}

void output_cli::printColumn(column* column)
{
	std::cout << "\t" << column->columnname << " " << column->datatype;
}

void output_cli::printTable(table* table)
{
	std::cout << table->schemaname << "." << table->tablename;

	std::cout << " ( PK:";
	if(table->pk.size()==0)
	{
		std::cout << " missing";
	}
	else
	{
		for(unsigned int i=0;i < table->pk.size();i++)
		{
			std::cout << " " << table->pk[i];
		}
	}
	std::cout << " ) " << std::endl;

	for(unsigned int i=0;i < table->columnlist.size();i++)
	{
		printColumn(&table->columnlist[i]);
		std::vector<constraint*> colconstr = table->getConstraints(table->schemaname, table->tablename, table->columnlist[i].columnname);
		for(unsigned int j=0;j < colconstr.size();j++)
		{
			printConstraint(colconstr[j]);
		}
		std::cout << std::endl;
	}
}

void output_cli::printData(std::vector<table> *tablelist)
{
	unsigned int i;
	std::cout << std::endl << "== Tables ==" << std::endl;
	for(i = 0; i < tablelist->size(); i++)
	{
		printTable(&tablelist->at(i));
	}
	std::cout << "(" << i << " rows)" << std::endl;
}