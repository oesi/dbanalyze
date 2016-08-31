#include "statistic.class.h"
#include "constraint_fk.class.h"
#include "constraint_uk.class.h"
#include "log.h"

void statistic::analyze(std::vector<table> *tablelist)
{
	this->num_tables = tablelist->size();
	this->num_columns = 0;
	this->num_fk = 0;
	this->num_uk = 0;

	for(unsigned int i = 0; i < tablelist->size(); i++)
	{
		if(tablelist->at(i).pk.size()==0)
			this->tables_without_pk.push_back(&tablelist->at(i));

		this->num_columns += tablelist->at(i).columnlist.size();

		for(unsigned int j = 0; j < tablelist->at(i).constraintlist.size(); j++)
		{
			constraint_fk* is_a_fk = dynamic_cast< constraint_fk* >( tablelist->at(i).constraintlist[j] );
			constraint_uk* is_a_uk = dynamic_cast< constraint_uk* >( tablelist->at(i).constraintlist[j] );
			if(is_a_fk)
			{
				this->num_fk++;
			}
			else if(is_a_uk)
			{
				this->num_uk++;
			}
		}
	}
}

void statistic::output()
{
	green << "Number of Tables:" << this->num_tables;
	green << "Number of Tables without PK:" << this->tables_without_pk.size();
	for(unsigned int j = 0; j < this->tables_without_pk.size(); j++)
	{
		red << "\t" << this->tables_without_pk[j]->schemaname << "." << this->tables_without_pk[j]->tablename;
	}
	green << "Number of Columns: " << this->num_columns;
	green << "Number of FK: " << this->num_fk;
	green << "Number of UK: " << this->num_uk;
}

statistic::~statistic()
{
}