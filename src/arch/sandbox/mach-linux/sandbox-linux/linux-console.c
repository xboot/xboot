#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sandbox-linux.h>

ssize_t sandbox_linux_console_read(void * buf, size_t count)
{
	return sandbox_linux_read_nonblock(fileno(stdin), buf, count);
}

ssize_t sandbox_linux_console_write(void * buf, size_t count)
{
	return sandbox_linux_write(fileno(stdout), buf, count);
}
