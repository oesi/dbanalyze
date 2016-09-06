#include "constraint_fk.class.h"

constraint_fk::constraint_fk()
{
}

constraint_fk::constraint_fk(std::string constraint_schema, std::string constraint_name): constraint(constraint_schema, constraint_name)
{
	this->constraint_schema = constraint_schema;
	this->constraint_name = constraint_name;
}

void constraint_fk::addTarget(std::string table_schema, std::string table_name, std::string table_column)
{
	this->target.push_back(ConstraintData(table_schema, table_name, table_column));
}

constraint_fk::~constraint_fk()
{

}