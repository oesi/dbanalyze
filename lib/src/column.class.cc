#include "column.class.h"
#include "constraint.class.h"
#include "constraint_fk.class.h"
#include <vector>

column::column(void* tablepntr, std::string columnname, std::string datatype, std::string character_maximum_length,std::string numeric_precision, std::string numeric_scale, std::string is_nullable)
{
	this->tablepntr = tablepntr;
	this->columnname = columnname;
	this->datatype = datatype;
	this->character_maximum_length = character_maximum_length;
	this->numeric_precision = numeric_precision;
	this->numeric_scale = numeric_scale;
	this->is_nullable = is_nullable;
}

std::string column::getColumnType()
{
	std::string retval = this->datatype;

	if(this->character_maximum_length!="NULL")
		retval += "("+this->character_maximum_length+")";
	if(this->numeric_precision!="NULL" && this->numeric_scale!="NULL" && this->numeric_scale!="0")
		retval += "("+this->numeric_precision+","+this->numeric_scale+")";

	return retval;
}