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
		std::string color;

	public:
		Log(const std::string &funcName, std::string color)
		{
			this->color = color;
			this->endl = false;
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
			if(!this->endl)
			{
				std::cout << std::endl;
			}
		}
};

#define dbg Log(__FUNCTION__,KBLU)
#define green Log(__FUNCTION__,KGRN)
#define red Log(__FUNCTION__,KRED)

#endif //DBANALYZE_LOG