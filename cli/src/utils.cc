#include <termios.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iomanip>

/**
 * Disables / Enables Output visibility for password input
 */
void SetStdinEcho(bool enable = true)
{
	struct termios tty;
	tcgetattr(STDIN_FILENO, &tty);
	if( !enable )
		tty.c_lflag &= ~ECHO;
	else
		tty.c_lflag |= ECHO;

	(void) tcsetattr(STDIN_FILENO, TCSANOW, &tty);

}

/**
 * Create a Color from a string
 */
std::string StringToColor(std::string value)
{
	int hash = 0;
	for(unsigned int i=0; i < value.length(); i++)
	{
		hash = value[i] + ((hash << 5) - hash);
	}

	int r = (hash & 0xFF0000) >> 16;
	int g = (hash & 0x00FF00) >> 8;
	int b = (hash & 0x0000FF);

	std::stringstream rgb;
	rgb << "#";
	rgb << std::setiosflags(std::ios::uppercase) << std::setfill('0') << std::setw(2) << std::hex;
	rgb << r;
	rgb << g;
	rgb << b;
	return rgb.str();
}