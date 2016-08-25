#include <termios.h>
#include <unistd.h>

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