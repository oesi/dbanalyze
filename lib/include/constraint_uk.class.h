#ifndef DBA_CONSTRAINT_UK_CLASS
#define DBA_CONSTRAINT_UK_CLASS
#include "constraint.class.h"

class constraint_uk: public constraint
{
	public:
		constraint_uk();
		constraint_uk(std::string constraint_schema, std::string constraint_name);
		~constraint_uk();
};

#endif //DBA_CONSTRAINT_UK_CLASS