#ifndef CONSTRAINTCLASS
#define CONSTRAINTCLASS

#include "column.class.h"
#include <string>
#include <vector>

class constraint
{
	public:
		constraint();
		constraint(std::string constraint_schema, std::string constraint_name);
		virtual ~constraint();
		void addSource(void* columnpntr);

		void* source;
		std::string constraint_schema;
		std::string constraint_name;
};

#endif //CONSTRAINTCLASS