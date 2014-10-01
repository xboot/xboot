#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct console_t
{
	/* The console name */
	char * name;

	/* Read console */
	ssize_t (*read)(struct console_t * console, unsigned char * buf, size_t count);

	/* Write console */
	ssize_t (*write)(struct console_t * console, const unsigned char * buf, size_t count);

	/* Suspend console */
	void (*suspend)(struct console_t * led);

	/* Resume console */
	void (*resume)(struct console_t * led);

	/* Private data */
	void * priv;
};

struct console_stdio_data_t
{
	const char * in;
	const char * out;
	const char * err;
};

ssize_t console_stdin_read(unsigned char * buf, size_t count);
ssize_t console_stdout_write(const unsigned char * buf, size_t count);
ssize_t console_stderr_write(const unsigned char * buf, size_t count);

struct console_t * search_console(const char * name);
bool_t register_console(struct console_t * console);
bool_t unregister_console(struct console_t * console);

#ifdef __cplusplus
}
#endif

#endif /* __CONSOLE_H__ */
