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
	ssize_t (*read)(struct console_t * con, unsigned char * buf, size_t count);

	/* Write console */
	ssize_t (*write)(struct console_t * con, const unsigned char * buf, size_t count);

	/* Private data */
	void * priv;
};

struct console_t * search_console(const char * name);
struct console_t * search_first_console(void);
struct device_t * register_console(struct console_t * con, struct driver_t * drv);
void unregister_console(struct console_t * con);

struct console_t * console_get(void);
bool_t console_set(const char * name);
ssize_t console_stdin_read(unsigned char * buf, size_t count);
ssize_t console_stdout_write(const unsigned char * buf, size_t count);
ssize_t console_stderr_write(const unsigned char * buf, size_t count);

#ifdef __cplusplus
}
#endif

#endif /* __CONSOLE_H__ */
