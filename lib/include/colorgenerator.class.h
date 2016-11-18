#ifndef DBA_COLORGENERATOR_CLASS
#define DBA_COLORGENERATOR_CLASS

#include <string>

class colorgenerator
{
public:
	colorgenerator(std::string value);

	int r;
	int g;
	int b;
	std::string hex;
};

#endif //DBA_COLORGENERATOR