#ifndef OUTPUT_CLI_CLASS
#define OUTPUT_CLI_CLASS

#include "dbanalyze.h"
#include "constraint_fk.class.h"
#include <string>
#include <vector>
#include <iostream>

class output_cli
{
	public:
		void printData(std::vector<table> *tablelist);
		void printTable(table* table);
		void printColumn(column* column);
		void printConstraint(constraint* constr);
		void printStatistic(statistic* stat);
		void printGraph(std::vector<table> *tablelist);
};

#endif //OUTPUT_CLI_CLASS