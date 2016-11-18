#ifndef DBA_COLUMN_CLASS
#define DBA_COLUMN_CLASS

#include <string>

class column
{
	public:
		column(void* tablepntr, std::string columnname, std::string datatype, std::string character_maximum_length,std::string numeric_precision, std::string numeric_scale, std::string is_nullable);
		std::string getColumnType();

		void* tablepntr;
		std::string columnname;
		std::string datatype;
		std::string character_maximum_length;
		std::string numeric_precision;
		std::string numeric_scale;
		std::string is_nullable;
};

#endif //DBA_COLUMN_CLASS