#ifndef __STDIO_H__
#define __STDIO_H__

#include <types.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <malloc.h>
#include <math.h>
#include <xboot/list.h>
#include <fs/fileio.h>

#ifndef EOF
#define EOF		(-1)
#endif

#ifndef BUFSIZ
#define BUFSIZ	(4096)
#endif

enum {
	SEEK_SET	= 0,
	SEEK_CUR	= 1,
	SEEK_END	= 2,
};

enum {
	_IONBF		= 0,
	_IOLBF		= 1,
	_IOFBF		= 2,
};

struct sbuf
{
	unsigned char * buf;
	size_t limit;
	size_t pos;
	int dirty;
};

typedef struct {
	int fd;
	int eof;
	int error;
	loff_t ofs;
	struct sbuf in;
	struct sbuf out;
	struct list_head node;
} FILE;


extern FILE * stdin;
extern FILE * stdout;
extern FILE * stderr;

int feof(FILE * f);
int ferror(FILE * f);
void clearerr(FILE * fp);

FILE * fopen(const char * file, const char * mode);
FILE * fdopen(int fd, const char * mode);
FILE * freopen(const char * file, const char * mode, FILE * f);
int fclose(FILE * f);
int fileno(FILE * f);
int fflush(FILE * f);

int fgetc(FILE * f);
char * fgets(char * buf, int n, FILE * f);
int fputc(int c, FILE * f);
int fputs(const char * s, FILE * f);
int ungetc(int c, FILE * f);

int fseek(FILE * f, loff_t offset, int whence);
loff_t ftell(FILE * f);

int setvbuf(FILE * f, char * buf, int mode, size_t size);
size_t fread(void * buf, size_t size, size_t count, FILE * f);
size_t fwrite(const void * buf, size_t size, size_t count, FILE * f);
int fprintf(FILE * f, const char * fmt, ...);
int fscanf(FILE * f, const char * fmt, ...);

FILE * tmpfile(void);

int vsnprintf(char * buf, size_t n, const char * fmt, va_list ap);
int vsscanf(const char * buf, const char * fmt, va_list ap);
int sprintf(char * buf, const char * fmt, ...);
int snprintf(char * buf, size_t n, const char * fmt, ...);
int sscanf(const char * buf, const char * fmt, ...);

/*
 * inner function
 */
FILE * __create_stdio(void);
void __destroy_stdio(FILE * f);
int __fill_stdio(FILE * f);
int __flush_stdio(void);

#endif /* __STDIO_H__ */
