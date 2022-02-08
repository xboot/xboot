#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct console_t
{
	char * name;
	ssize_t (*read)(struct console_t * con, unsigned char * buf, size_t count);
	ssize_t (*write)(struct console_t * con, const unsigned char * buf, size_t count);
	void * priv;
};

struct console_t * search_console(const char * name);
struct console_t * search_first_console(void);
struct device_t * register_console(struct console_t * con, struct driver_t * drv);
void unregister_console(struct console_t * con);

#ifdef __cplusplus
}
#endif

#endif /* __CONSOLE_H__ */
