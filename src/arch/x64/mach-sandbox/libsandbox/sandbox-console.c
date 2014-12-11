#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <termios.h>
#include <sandbox.h>

static struct termios __term_config;

void sandbox_console_init(void)
{
	struct termios termvi;

	tcgetattr(0, &__term_config);
	termvi = __term_config;
	termvi.c_lflag &= (~ICANON & ~ECHO & ~ISIG);
	termvi.c_iflag &= (~IXON & ~ICRNL);
	termvi.c_oflag |= (ONLCR);
	termvi.c_cc[VMIN] = 1;
	termvi.c_cc[VTIME] = 0;
	tcsetattr(0, TCSANOW, &termvi);
}

void sandbox_console_exit(void)
{
	fflush(stdout);
	tcsetattr(0, TCSANOW, &__term_config);
}

ssize_t sandbox_console_read(void * buf, size_t count)
{
	return sandbox_read_nonblock(fileno(stdin), buf, count);
}

ssize_t sandbox_console_write(void * buf, size_t count)
{
	return sandbox_write(fileno(stdout), buf, count);
}
