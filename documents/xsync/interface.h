#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>

struct interface_t {
	uint64_t (*time)(void);
	ssize_t (*read)(void * buf, size_t len);
	ssize_t (*write)(void * buf, size_t len);
	void * pirv;
};

uint64_t interface_time(struct interface_t * iface);
ssize_t interface_read(struct interface_t * iface, void * buf, size_t len);
ssize_t interface_write(struct interface_t * iface, void * buf, size_t len);

#endif /* __INTERFACE_H__ */
