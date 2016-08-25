#ifndef COLUMNCLASS
#define COLUMNCLASS

#include <string>
#include <vector>

class column
{
	public:
		column(std::string columnname, std::string datatype, std::string character_maximum_length,std::string numeric_precision, std::string numeric_scale, std::string is_nullable);
		void output();

	private:
		std::string columnname;
		std::string datatype;
		std::string character_maximum_length;
		std::string numeric_precision;
		std::string numeric_scale;
		std::string is_nullable;
};

#endif //COLUMNCLASS