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
			constraint_fk* fk = dynamic_cast< constraint_fk* >( tablelist->at(i).constraintlist[j] );
			constraint_uk* uk = dynamic_cast< constraint_uk* >( tablelist->at(i).constraintlist[j] );
			if(fk)
			{
				// count foreign keys
				this->num_fk++;

				// check if foreign key datatype match target datatype
				column *sourcecol = static_cast<column*>(fk->source);
				if(fk->target!=NULL)
				{
					if(sourcecol->datatype != fk->target->datatype)
					{
						this->fk_datatype_missmatch.push_back(tablelist->at(i).constraintlist[j]);
					}
				}
			}
			else if(uk)
			{
				// count unique constraints
				this->num_uk++;
			}
		}

		// collect empty tables
		if(tablelist->at(i).row_count==0)
			this->empty_tables.push_back(&tablelist->at(i));
	}

	this->calculateScore();
}

void statistic::calculateScore()
{
	this->score=0;
	this->maximum_score=0;

	// positive
	this->score += this->num_tables * STATISTIC_TABLE_SCORE_FACTOR;
	this->score += this->num_columns * STATISTIC_COLUMN_SCORE_FACTOR;
	this->score += this->num_fk * STATISTIC_FK_SCORE_FACTOR;
	this->score += this->num_uk * STATISTIC_UK_SCORE_FACTOR;
	this->maximum_score = this->score;

	// negative
	this->score += this->tables_without_pk.size() * STATISTIC_TABLE_NO_PK_SCORE_FACTOR;
	this->score += this->fk_datatype_missmatch.size() * STATISTIC_FK_MISSMATCH_SCORE_FACTOR;
	this->score += this->empty_tables.size() * STATISTIC_EMPTY_TABLES_SCORE_FACTOR;
}

statistic::~statistic()
{
}