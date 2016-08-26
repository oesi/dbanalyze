#include "constraint.class.h"
#include "log.h"

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

void constraint::addSource(std::string table_schema, std::string table_name, std::string table_column)
{
	this->source.push_back(ConstraintData(table_schema, table_name, table_column));
}

void constraint::output()
{
	green << "⚷";
	for(unsigned int i=0;i < this->source.size();i++)
	{
		green << this->source[i].column;
	}
}