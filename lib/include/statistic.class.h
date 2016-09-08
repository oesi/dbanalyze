#ifndef STATISTICCLASS
#define STATISTICCLASS

#include "table.class.h"
#include "constraint_fk.class.h"
#include <string>
#include <vector>

class statistic
{
	public:
		~statistic();
		void analyze(std::vector<table> *tablelist);

		std::vector<table*> tables_without_pk;
		std::vector<table*> empty_tables;
		int num_tables;
		int num_columns;
		int num_fk;
		int num_uk;
		std::vector<constraint*> fk_datatype_missmatch;
};

#endif //STATISTICCLASS