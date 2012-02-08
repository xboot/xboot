#ifndef __STDIO_H__
#define __STDIO_H__

#include <types.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <malloc.h>

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

enum {
	__SLBF		= 0x0001, 	/* line buffered */
	__SNBF		= 0x0002, 	/* unbuffered */
	__SRD 		= 0x0004, 	/* OK to read */
	__SWR 		= 0x0008, 	/* OK to write */
	__SRW 		= 0x0010, 	/* open for reading & writing */
	__SEOF		= 0x0020, 	/* found EOF */
	__SERR 		= 0x0040, 	/* found error */
	__SMBF 		= 0x0080, 	/* _buf is from malloc */
	__SAPP 		= 0x0100, 	/* fdopen()ed in append mode */
	__SSTR 		= 0x0200, 	/* this is an sprintf/snprintf string */
	__SOPT 		= 0x0400, 	/* do fseek() optimisation */
	__SNPT 		= 0x0800, 	/* do not do fseek() optimisation */
	__SOFF 		= 0x1000, 	/* set iff _offset is in fact correct */
	__SMOD 		= 0x2000, 	/* true => fgetln modified _p text */
	__SALC 		= 0x4000, 	/* allocate string space dynamically */
	__SIGN 		= 0x8000,	/* ignore this file in _fwalk */
};

/*
 * Stdio file position type
 */
typedef loff_t fpos_t;

/*
 * Stdio buffers.
 */
struct __sbuf {
	unsigned char * _base;
	int	_size;
};

/*
 * stdio state variables.
 *
 * The following always hold:
 *
 *	if (_flags&(__SLBF|__SWR)) == (__SLBF|__SWR),
 *		_lbfsize is -_bf._size, else _lbfsize is 0
 *	if _flags&__SRD, _w is 0
 *	if _flags&__SWR, _r is 0
 *
 * This ensures that the getc and putc macros (or inline functions) never
 * try to write or read from a file that is in `read' or `write' mode.
 * (Moreover, they can, and do, automatically switch from read mode to
 * write mode, and back, on "r+" and "w+" files.)
 *
 * _lbfsize is used only to make the inline line-buffered output stream
 * code as compact as possible.
 *
 * _ub, _up, and _ur are used when ungetc() pushes back more characters
 * than fit in the current _bf, or when ungetc() pushes back a character
 * that does not match the previous one in _bf.  When this happens,
 * _ub._base becomes non-nil (i.e., a stream has ungetc() data iff
 * _ub._base!=NULL) and _up and _ur save the current values of _p and _r.
 *
 * NOTE: if you change this structure, you also need to update the
 * std() initializer in findfp.c.
 */
typedef struct {
	unsigned char * _p;		/* current position in (some) buffer */
	int	_r;					/* read space left for getc() */
	int	_w;					/* write space left for putc() */
	short _flags;			/* flags, below; this FILE is free if 0 */
	short _file;			/* fileno, if Unix descriptor, else -1 */
	struct __sbuf _bf;		/* the buffer (at least 1 byte, if !NULL) */
	int _lbfsize;			/* 0 or -_bf._size, for inline putc */

	/* operations */
	void * _cookie;			/* cookie passed to io functions */
	int	(*_read)(void *, char *, int);
	int	(*_write)(void *, const char *, int);
	fpos_t (*_seek)(void *, fpos_t, int);
	int	(*_close)(void *);

	/* extension data, to avoid further ABI breakage */
	struct __sbuf _ext;

	/* data for long sequences of ungetc() */
	unsigned char *_up;		/* saved _p when _p is doing ungetc data */
	int	_ur;				/* saved _r when _r is counting ungetc data */

	/* tricks to meet minimum requirements even when malloc() fails */
	unsigned char _ubuf[3];	/* guarantee an ungetc() buffer */
	unsigned char _nbuf[1];	/* guarantee a getc() buffer */

	/* separate buffer for fgetln() when line crosses buffer boundary */
	struct	__sbuf _lb;		/* buffer for fgetln() */

	/* Unix stdio files get aligned to block boundaries on fseek() */
	int	_blksize;			/* stat.st_blksize (may be != _bf._size) */
	fpos_t _offset;			/* current lseek offset */
} FILE;


#define	__sfeof(p)			(((p)->_flags & __SEOF) != 0)
#define	__sferror(p)		(((p)->_flags & __SERR) != 0)
#define	__sclearerr(p)		((void)((p)->_flags &= ~(__SERR|__SEOF)))
#define	__sfileno(p)		((p)->_file)

extern FILE * stdin;
extern FILE * stdout;
extern FILE * stderr;

int feof(FILE * fp);
int ferror(FILE * fp);
void clearerr(FILE * fp);

FILE * fopen(const char * file, const char * mode);
FILE * fdopen(int fd, const char * mode);
FILE * freopen(const char * file, const char * mode, FILE * fp);
int fclose(FILE * fp);
int fileno(FILE * fp);
int fflush(FILE * fp);

int fgetc(FILE * fp);
char * fgets(char * buf, int n, FILE * fp);
int fputc(int c, FILE * fp);
int fputs(const char * s, FILE * fp);
int ungetc(int c, FILE * fp);

int fseek(FILE * fp, fpos_t offset, int whence);
fpos_t ftell(FILE * fp);

int setvbuf(FILE * fp, char * buf, int mode, size_t size);
size_t fread(void * buf, size_t size, size_t count, FILE * fp);
size_t fwrite(const void * buf, size_t size, size_t count, FILE * fp);
int fprintf(FILE * fp, const char * fmt, ...);
int fscanf(FILE * fp, const char * fmt, ...);

FILE * tmpfile(void);

int vsnprintf(char * buf, size_t n, const char * fmt, va_list ap);
int vsscanf(const char * buf, const char * fmt, va_list ap);
int sprintf(char * buf, const char * fmt, ...);
int snprintf(char * buf, size_t n, const char * fmt, ...);
int sscanf(const char * buf, const char * fmt, ...);

/*
 * Inner function
 */
void flockfile(FILE * fp);
int ftrylockfile(FILE * fp);
void funlockfile(FILE * fp);

#endif /* __STDIO_H__ */
