#ifndef STATISTICCLASS
#define STATISTICCLASS

#include "table.class.h"
#include "constraint_fk.class.h"
#include <string>
#include <vector>

#define STATISTIC_TABLE_SCORE_FACTOR 1.0;
#define STATISTIC_COLUMN_SCORE_FACTOR 0.1;
#define STATISTIC_FK_SCORE_FACTOR 0.2;
#define STATISTIC_UK_SCORE_FACTOR 0.1;
#define STATISTIC_TABLE_NO_PK_SCORE_FACTOR -2.0;
#define STATISTIC_FK_MISSMATCH_SCORE_FACTOR -0.7;
#define STATISTIC_EMPTY_TABLES_SCORE_FACTOR -0.4;

class statistic
{
	public:
		~statistic();
		void analyze(std::vector<table> *tablelist);
		void calculateScore();

		std::vector<table*> tables_without_pk;
		std::vector<table*> empty_tables;
		int num_tables;
		int num_columns;
		int num_fk;
		int num_uk;
		std::vector<constraint*> fk_datatype_missmatch;
		float score;
		float maximum_score;
};

#endif //STATISTICCLASS