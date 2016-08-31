#ifndef STATISTICCLASS
#define STATISTICCLASS

#include "table.class.h"
#include <string>
#include <vector>

class statistic
{
	public:
		~statistic();
		void analyze(std::vector<table> *tablelist);
		void output();

		int num_tables;
		std::vector<table*> tables_without_pk;
		int num_columns;
		int num_fk;
		int num_uk;
};

#endif //STATISTICCLASS