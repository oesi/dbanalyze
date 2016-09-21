#ifndef DBANALYZE_FORMATTER
#define DBANALYZE_FORMATTER

#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define BOLD  "\033[1m"
#define UNBOLD "\033[0m"

class formatter
{
	private:
		std::string color;

	public:
		formatter()
		{
		}
		formatter(std::string color)
		{
			this->color = color;
		}

		template <class T>
		formatter &operator<<(const T &value)
		{
			std::cout << this->color << value << KNRM;
			return *this;
		}

		~formatter()
		{
			std::cout << std::endl;
		}
};

struct TableFormat
{
	unsigned int width;
	char fill;
	std::string color;
	std::string colorend;

	TableFormat(std::string color, int width, char fill)
	{
		this->color = color;
		if(this->color == BOLD)
			this->colorend = UNBOLD;
		else
			this->colorend = KNRM;
		this->width = width;
		this->fill = fill;
	}
	template<typename T>
	TableFormat& operator<<(const T& data)
	{
		std::cout << color;
		std::stringstream outstr;
		std::stringstream outstr2;
		outstr << data;

		if(outstr.str().length()>width)
			outstr2 << outstr.str().substr(0,width-3) << "...";
		else
			outstr2 << data;

		std::cout << std::left << std::setw(width) << std::setfill(fill) << outstr2.str();
		std::cout << colorend;
		std::cout << "|";
		return *this;
	}
	TableFormat& operator<<(std::ostream&(*out)(std::ostream&))
	{
		std::cout << out;
		return *this;
	}
};

#define green formatter(KGRN)
#define red formatter(KRED)
#define tr TableFormat(KGRN,20,' ')
#define th TableFormat(BOLD,20,' ')
#define tf TableFormat(KNRM,20,'-')

#endif //DBANALYZE_FORMATTER