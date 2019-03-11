#include <x.h>
#include <sandbox.h>

static struct termios __term_config;

void sandbox_stdio_init(void)
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

void sandbox_stdio_exit(void)
{
	fflush(stdout);
	tcsetattr(0, TCSANOW, &__term_config);
}

ssize_t sandbox_stdio_read(void * buf, size_t count)
{
	return sandbox_file_read_nonblock(fileno(stdin), buf, count);
}

ssize_t sandbox_stdio_write(void * buf, size_t count)
{
	return sandbox_file_write(fileno(stdout), buf, count);
}
