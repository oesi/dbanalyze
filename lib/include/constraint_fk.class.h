#ifndef CONSTRAINT_FK_CLASS
#define CONSTRAINT_FK_CLASS
#include "constraint.class.h"

class constraint_fk: public constraint
{
	public:
		constraint_fk();
		constraint_fk(std::string constraint_schema, std::string constraint_name);
		~constraint_fk();
		void addTarget(column* target);

		column* target;
};

#endif //CONSTRAINT_FK_CLASS