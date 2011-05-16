#ifndef __STDIO_H__
#define __STDIO_H__

#include <types.h>
#include <stdarg.h>

typedef struct {
	int fd;
} FILE;

int fileno(FILE * fp);
FILE * fopen(const char * file, const char * mode);
int fclose(FILE * fp);
int fseek(FILE * fp, loff_t offset, int whence);
loff_t ftell(FILE * fp);


int vsnprintf(char * buf, size_t n, const char * fmt, va_list ap);
int vsscanf(const char * buf, const char * fmt, va_list ap);
int sprintf(char * buf, const char * fmt, ...);
int snprintf(char * buf, size_t n, const char * fmt, ...);
int sscanf(const char * buf, const char * fmt, ...);

// xxx for test
int ssize(char * buf, u64_t size);

#endif /* __STDIO_H__ */
