#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/time.h>

struct interface_t {
	ssize_t (*read)(struct interface_t * iface, void * buf, size_t len);
	ssize_t (*write)(struct interface_t * iface, void * buf, size_t len);
	void * priv;
};

ssize_t interface_read(struct interface_t * iface, void * buf, size_t len);
ssize_t interface_write(struct interface_t * iface, void * buf, size_t len);

struct interface_t * interface_serial_alloc(const char * device, int baud);
void interface_serial_free(struct interface_t * iface);

#endif /* __INTERFACE_H__ */
