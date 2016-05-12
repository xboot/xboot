#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

int serial_open(const char * device, int baud);
int serial_close(int fd);
ssize_t serial_read_byte(int fd, void * byte);
ssize_t serial_read(int fd, void * buf, size_t len);
ssize_t serial_write(int fd, const void * buf, size_t len);

#endif /* __SERIAL_H__ */
