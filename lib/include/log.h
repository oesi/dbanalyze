#ifndef DBANALYZE_LOG
#define DBANALYZE_LOG

#include <iostream>
#include <vector>
#include <sstream>

//#define DBANALYZE_DEBUG


#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

class Log
{
	private:
		bool endl;
		bool addendl;
		std::string color;

	public:
		Log(const std::string &funcName, std::string color, bool endl)
		{
			this->color = color;
			this->endl = false;
			this->addendl = endl;
			#ifdef DBANALYZE_DEBUG
				std::cout << KBLU << funcName << ": " << KNRM;
			#endif
		}

		template <class T>
		Log &operator<<(const T &value)
		{
			std::cout << this->color << value << KNRM;

			std::stringstream ss;
			ss << value;

			if(ss.str()[ss.str().size()-1] == '\n')
				this->endl = true;
			else
				this->endl = false;

			return *this;
		}

		~Log()
		{
			if(!this->endl && this->addendl)
			{
				std::cout << std::endl;
			}
		}
};

#define dbg Log(__FUNCTION__,KBLU, true)
#define green Log(__FUNCTION__,KGRN, true)
#define red Log(__FUNCTION__,KRED, true)
#define out_fk Log(__FUNCTION__,KGRN, false)
#define out_uk Log(__FUNCTION__,KGRN, false)

#endif //DBANALYZE_LOG