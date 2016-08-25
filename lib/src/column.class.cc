#include "column.class.h"
#include <iostream>
column::column(std::string columnname, std::string datatype, std::string character_maximum_length,std::string numeric_precision, std::string numeric_scale, std::string is_nullable)
{
	this->columnname = columnname;
	this->datatype = datatype;
	this->character_maximum_length = character_maximum_length;
	this->numeric_precision = numeric_precision;
	this->numeric_scale = numeric_scale;
	this->is_nullable = is_nullable;
}

void column::output()
{
	std::cout << "\t" << this->columnname << " " << this->datatype << std::endl;
}