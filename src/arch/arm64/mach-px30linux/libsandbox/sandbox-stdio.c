#include <x.h>
#include <sandbox.h>

ssize_t sandbox_stdio_read(void * buf, size_t count)
{
	return sandbox_file_read_nonblock(fileno(stdin), buf, count);
}

ssize_t sandbox_stdio_write(void * buf, size_t count)
{
	return sandbox_file_write(fileno(stdout), buf, count);
}
