#ifndef DBA_CONSTRAINT_CLASS
#define DBA_CONSTRAINT_CLASS

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

#endif //DBA_CONSTRAINT_CLASS