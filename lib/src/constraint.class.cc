#include "constraint.class.h"

constraint::constraint()
{
}

constraint::constraint(std::string constraint_schema, std::string constraint_name)
{
	this->constraint_schema = constraint_schema;
	this->constraint_name = constraint_name;
}

constraint::~constraint()
{

}

void constraint::addSource(void* columnpntr)
{
	this->source = columnpntr;
}