#include "constraint_uk.class.h"
#include "log.h"

constraint_uk::constraint_uk()
{
}

constraint_uk::constraint_uk(std::string constraint_schema, std::string constraint_name): constraint(constraint_schema, constraint_name)
{
	this->constraint_schema = constraint_schema;
	this->constraint_name = constraint_name;
}

constraint_uk::~constraint_uk()
{

}

void constraint_uk::output()
{
	out_uk << "⚷UK:";
	out_uk << this->constraint_name;
}