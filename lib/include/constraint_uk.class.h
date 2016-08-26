#ifndef CONSTRAINT_uk_CLASS
#define CONSTRAINT_uk_CLASS
#include "constraint.class.h"


class constraint_uk: public constraint
{
	public:
		constraint_uk();
		constraint_uk(std::string constraint_schema, std::string constraint_name);
		~constraint_uk();
};

#endif //CONSTRAINT_uk_CLASS