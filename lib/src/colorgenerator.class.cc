#include <termios.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include "colorgenerator.class.h"
/**
 * Create a Color from a string
 */
colorgenerator::colorgenerator(std::string value)
{
	int hash = 0;
	for(unsigned int i=0; i < value.length(); i++)
	{
		hash = value[i] + ((hash << 5) - hash);
	}

	r = (hash & 0xFF0000) >> 16;
	g = (hash & 0x00FF00) >> 8;
	b = (hash & 0x0000FF);

	std::stringstream rgb;
	rgb << "#";
	rgb << std::setiosflags(std::ios::uppercase) << std::setfill('0') << std::setw(2) << std::hex;
	rgb << r;
	rgb << g;
	rgb << b;

	this->hex = rgb.str();
}