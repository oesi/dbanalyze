#include "statistic.class.h"
#include "constraint_fk.class.h"
#include "constraint_uk.class.h"

void statistic::analyze(std::vector<table> *tablelist)
{
	this->num_tables = tablelist->size();
	this->num_columns = 0;
	this->num_fk = 0;
	this->num_uk = 0;

	for(unsigned int i = 0; i < tablelist->size(); i++)
	{
		// Check if table has a primary key
		if(tablelist->at(i).pk.size()==0)
			this->tables_without_pk.push_back(&tablelist->at(i));

		// count number of columns
		this->num_columns += tablelist->at(i).columnlist.size();

		for(unsigned int j = 0; j < tablelist->at(i).constraintlist.size(); j++)
		{
			constraint_fk* is_a_fk = dynamic_cast< constraint_fk* >( tablelist->at(i).constraintlist[j] );
			constraint_uk* is_a_uk = dynamic_cast< constraint_uk* >( tablelist->at(i).constraintlist[j] );
			if(is_a_fk)
			{
				// count foreign keys
				this->num_fk++;

				// check if foreign key datatype match target datatype
				/*
				if(tablelist->at(i).datatype !== tablelist->at(i).constraintlist[j]->target[0]->datatype)
				{
					this->fk_datatype_missmatch.push_back(tablelist->at(i).constraintlist[j]);
				}*/
			}
			else if(is_a_uk)
			{
				// count unique constraints
				this->num_uk++;
			}
		}

		// collect empty tables
		if(tablelist->at(i).row_count==0)
			this->empty_tables.push_back(&tablelist->at(i));
	}
}

statistic::~statistic()
{
}