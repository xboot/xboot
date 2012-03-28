#ifndef __STDIO_H__
#define __STDIO_H__

#include <types.h>
#include <ctype.h>
#include <errno.h>
#include <fifo.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#ifndef EOF
#define EOF		(-1)
#endif

#ifndef BUFSIZ
#define BUFSIZ	(4096)
#endif

enum {
	_IONBF		= 0,
	_IOLBF		= 1,
	_IOFBF		= 2,
};

enum {
	SEEK_SET	= 0,		/* set file offset to offset */
	SEEK_CUR	= 1,		/* set file offset to current plus offset */
	SEEK_END	= 2,		/* set file offset to EOF plus offset */
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
	//void * handle;
	int fd;

	ssize_t (*read)(FILE *, unsigned char *, size_t);
	ssize_t (*write)(FILE *, const unsigned char *, size_t);
	fpos_t (*seek)(FILE *, fpos_t, int);
	int (*close)(FILE *);

	int (*rwflush)(FILE *);

	struct fifo * fifo_read;
	struct fifo * fifo_write;

	fpos_t pos;
	int mode;
	int eof, error;
};

#define stdin			(__get_runtime()->__stdin)
#define stdout			(__get_runtime()->__stdout)
#define stderr			(__get_runtime()->__stderr)

#define putc(ch, fp)	fputc(ch, fp)
#define getc(fp)		fgetc(fp)

FILE * fopen(const char * filename, const char * mode);
FILE * freopen(const char * filename, const char * mode, FILE * f);
int fclose(FILE * f);

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

int fgetc(FILE * f);
char * fgets(char * s, int n, FILE * f);
int fputc(int c, FILE * f);
int fputs(const char * s, FILE * f);
int ungetc(int c, FILE * f);

int setvbuf(FILE * f, char * buf, int mode, size_t size);
void setbuf(FILE * f, char * buf);

FILE * tmpfile(void);

int fprintf(FILE * f, const char * fmt, ...);
int fscanf(FILE * f, const char * fmt, ...);

int vsnprintf(char * buf, size_t n, const char * fmt, va_list ap);
int vsscanf(const char * buf, const char * fmt, va_list ap);
int sprintf(char * buf, const char * fmt, ...);
int snprintf(char * buf, size_t n, const char * fmt, ...);
int sscanf(const char * buf, const char * fmt, ...);

/*
 * Inner function
 */
ssize_t __file_read(FILE * f, unsigned char * buf, size_t size);
ssize_t __file_write(FILE * f, const unsigned char * buf, size_t size);
fpos_t __file_seek(FILE * f, fpos_t off, int whence);
int __file_close(FILE * f);

int __stdio_no_flush(FILE * f);
int	__stdio_read_flush(FILE * f);
int __stdio_write_flush(FILE * f);

ssize_t __stdio_read(FILE * f, unsigned char * buf, size_t size);
ssize_t __stdio_write(FILE * f, const unsigned char * buf, size_t size);

#if 0
#ifndef UNGET
#define UNGET	(8)
#endif

enum {
	F_PERM 		= 0x0001,
	F_NORD 		= 0x0004,
	F_NOWR 		= 0x0008,
	F_EOF 		= 0x0010,
	F_ERR 		= 0x0020,
	F_SVB 		= 0x0040,
};

/*
 * stdio state variables.
 */
typedef struct __FILE FILE;
struct __FILE {
	unsigned flags;
	unsigned char * rpos, * rend;
	int (*close)(FILE *);
	unsigned char * wend, * wpos;
	unsigned char * mustbezero_1;
	unsigned char * wbase;
	size_t (*read)(FILE *, unsigned char *, size_t);
	size_t (*write)(FILE *, const unsigned char *, size_t);
	off_t (*seek)(FILE *, off_t, int);
	unsigned char * buf;
	size_t buf_size;
	FILE * prev, * next;
	int fd;
	int pipe_pid;
	long lockcount;
	short dummy3;
	signed char mode;
	signed char lbf;
	int lock;
	int waiters;
	void * cookie;
	off_t off;
	int (*flush)(FILE *);
	void * mustbezero_2;
};

#define FLOCK(f)		int __need_unlock = ((f)->lock >= 0 ? __lockfile((f)) : 0)
#define FUNLOCK(f)		if (__need_unlock) __unlockfile((f)); else

#define OFLLOCK()		do{ } while(0)
#define OFLUNLOCK() 	do{ } while(0)

#define getc_unlocked(f) \
	( ((f)->rpos < (f)->rend) ? *(f)->rpos++ : __underflow((f)) )

#define putc_unlocked(c, f)	\
	( ((c)!=(f)->lbf && (f)->wpos<(f)->wend) ? *(f)->wpos++ = (c) : __overflow((f),(c)) )

#define stdin			(__get_runtime()->__stdin)
#define stdout			(__get_runtime()->__stdout)
#define stderr			(__get_runtime()->__stderr)

#define putc(ch, fp)	fputc(ch, fp)
#define getc(fp)		fgetc(fp)




/*
 * Inner function
 */
int __lockfile(FILE * f);
void __unlockfile(FILE * f);

size_t __stdio_read(FILE * f, unsigned char * buf, size_t len);
size_t __stdio_write(FILE * f, const unsigned char * buf, size_t len);
off_t __stdio_seek(FILE * f, off_t off, int whence);
int __stdio_close(FILE * f);

int __toread(FILE * f);
int __towrite(FILE * f);

int __overflow(FILE * f, int _c);
int __underflow(FILE * f);

int __fseeko(FILE * f, off_t off, int whence);
int __fseeko_unlocked(FILE * f, off_t off, int whence);
off_t __ftello(FILE * f);
off_t __ftello_unlocked(FILE * f);

FILE * __fdopen(int fd, const char * mode);
#endif

#endif /* __STDIO_H__ */
