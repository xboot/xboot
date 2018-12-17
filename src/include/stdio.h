#ifndef __STDIO_H__
#define __STDIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <stdarg.h>
#include <stddef.h>
#include <sizes.h>
#include <errno.h>
#include <limits.h>
#include <fifo.h>

#ifndef EOF
#define EOF			(-1)
#endif

#ifndef BUFSIZ
#define BUFSIZ		(4096)
#endif

#ifndef L_tmpnam
#define L_tmpnam	(32)
#endif

enum {
	_IONBF			= 0,
	_IOLBF			= 1,
	_IOFBF			= 2,
};

enum {
	SEEK_SET		= 0,		/* set file offset to offset */
	SEEK_CUR		= 1,		/* set file offset to current plus offset */
	SEEK_END		= 2,		/* set file offset to EOF plus offset */
};

/*
 * Stdio file position type
 */
typedef loff_t fpos_t;

/*
 * stdio state variables.
 */
typedef struct __FILE FILE;
struct __FILE {
	int fd;

	ssize_t (*read)(FILE *, unsigned char *, size_t);
	ssize_t (*write)(FILE *, const unsigned char *, size_t);
	fpos_t (*seek)(FILE *, fpos_t, int);
	int (*close)(FILE *);

	struct fifo_t * fifo_read;
	struct fifo_t * fifo_write;

	unsigned char * buf;
	size_t bufsz;
	int (*rwflush)(FILE *);

	fpos_t pos;
	int mode;
	int eof, error;
};

#define stdin		(__stdio_get_stdin())
#define stdout		(__stdio_get_stdout())
#define stderr		(__stdio_get_stderr())

FILE * fopen(const char * path, const char * mode);
FILE * freopen(const char * path, const char * mode, FILE * f);
int fclose(FILE * f);

int remove(const char * path);
int rename(const char * old, const char * new);
int system(const char * cmd);

int feof(FILE * f);
int ferror(FILE * f);
int fflush(FILE * f);
void clearerr(FILE * f);

int fseek(FILE * f, fpos_t off, int whence);
fpos_t ftell(FILE * f);
void rewind(FILE * f);

int fgetpos(FILE * f, fpos_t * pos);
int fsetpos(FILE * f, const fpos_t * pos);

size_t fread(void * buf, size_t size, size_t count, FILE * f);
size_t fwrite(const void * buf, size_t size, size_t count, FILE * f);

int getchar(void);
int getc(FILE * f);
int fgetc(FILE * f);
char * fgets(char * s, int n, FILE * f);
int putchar(int c);
int putc(int c, FILE * f);
int fputc(int c, FILE * f);
int fputs(const char * s, FILE * f);
int ungetc(int c, FILE * f);

int setvbuf(FILE * f, char * buf, int mode, size_t size);
void setbuf(FILE * f, char * buf);

FILE * tmpfile(void);
char * tmpnam(char * buf);

int fprintf(FILE * f, const char * fmt, ...);
int fscanf(FILE * f, const char * fmt, ...);
int printf(const char * fmt, ...);
int scanf(const char * fmt, ...);

int vsnprintf(char * buf, size_t n, const char * fmt, va_list ap);
int vasprintf(char ** s, const char * fmt, va_list ap);
int vsscanf(const char * buf, const char * fmt, va_list ap);
int asprintf(char ** s, const char * fmt, ...);
int sprintf(char * buf, const char * fmt, ...);
int snprintf(char * buf, size_t n, const char * fmt, ...);
int sscanf(const char * buf, const char * fmt, ...);

/*
 * Inner function
 */
int __stdio_no_flush(FILE * f);
int	__stdio_read_flush(FILE * f);
int __stdio_write_flush(FILE * f);

ssize_t __stdio_read(FILE * f, unsigned char * buf, size_t size);
ssize_t __stdio_write(FILE * f, const unsigned char * buf, size_t size);

FILE * __file_alloc(int fd);
FILE * __stdio_get_stdin(void);
FILE * __stdio_get_stdout(void);
FILE * __stdio_get_stderr(void);

#ifdef __cplusplus
}
#endif

#endif /* __STDIO_H__ */
