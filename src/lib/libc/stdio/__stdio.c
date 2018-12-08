/*
 * libc/stdio/__stdio.c
 */

#include <xboot/module.h>
#include <console/console.h>
#include <stdio.h>

static FILE * __stdin = NULL;
static FILE * __stdout = NULL;
static FILE * __stderr = NULL;

static ssize_t __tty_stdin_read(FILE * f, unsigned char * buf, size_t size)
{
	return console_stdin_read(buf, size);
}

static ssize_t __tty_stdout_write(FILE * f, const unsigned char * buf, size_t size)
{
	return console_stdout_write(buf, size);
}

static ssize_t __tty_stderr_write(FILE * f, const unsigned char * buf, size_t size)
{
	return console_stderr_write(buf, size);
}

static ssize_t __tty_null_read(FILE * f, unsigned char * buf, size_t size)
{
	return 0;
}

static ssize_t __tty_null_write(FILE * f, const unsigned char * buf, size_t size)
{
	return 0;
}

static fpos_t __tty_null_seek(FILE * f, fpos_t off, int whence)
{
	return 0;
}

static int __tty_null_close(FILE * f)
{
	return 0;
}

static ssize_t __file_read(FILE * f, unsigned char * buf, size_t size)
{
	return vfs_read(f->fd, (void *)buf, size);
}

static ssize_t __file_write(FILE * f, const unsigned char * buf, size_t size)
{
	return vfs_write(f->fd, (void *)buf, size);
}

static fpos_t __file_seek(FILE * f, fpos_t off, int whence)
{
	return vfs_lseek(f->fd, off, whence);
}

static int __file_close(FILE * f)
{
	return vfs_close(f->fd);
}

/*
 * file alloc
 */
FILE * __file_alloc(int fd)
{
	FILE * f;

	f = malloc(sizeof(FILE));
	if(!f)
		return 0;

	if(fd == 0)
	{
		f->fd = fd;

		f->read = __tty_stdin_read;
		f->write = __tty_null_write;
		f->seek = __tty_null_seek;
		f->close = __tty_null_close;

		f->fifo_read = fifo_alloc(BUFSIZ);
		f->fifo_write = fifo_alloc(BUFSIZ);

		f->buf = malloc(BUFSIZ);
		f->bufsz = BUFSIZ;
		f->rwflush = &__stdio_no_flush;

		f->pos = 0;
		f->mode = _IOLBF;
		f->error = 0;
		f->eof = 0;
	}
	else if(fd == 1)
	{
		f->fd = fd;

		f->read = __tty_null_read;
		f->write = __tty_stdout_write;
		f->seek = __tty_null_seek;
		f->close = __tty_null_close;

		f->fifo_read = fifo_alloc(BUFSIZ);
		f->fifo_write = fifo_alloc(BUFSIZ);

		f->buf = malloc(BUFSIZ);
		f->bufsz = BUFSIZ;
		f->rwflush = &__stdio_no_flush;

		f->pos = 0;
		f->mode = _IOLBF;
		f->error = 0;
		f->eof = 0;
	}
	else if(fd == 2)
	{
		f->fd = fd;

		f->read = __tty_null_read;
		f->write = __tty_stderr_write;
		f->seek = __tty_null_seek;
		f->close = __tty_null_close;

		f->fifo_read = fifo_alloc(BUFSIZ);
		f->fifo_write = fifo_alloc(BUFSIZ);

		f->buf = malloc(BUFSIZ);
		f->bufsz = BUFSIZ;
		f->rwflush = &__stdio_no_flush;

		f->pos = 0;
		f->mode = _IONBF;
		f->error = 0;
		f->eof = 0;
	}
	else
	{
		f->fd = fd;

		f->read = __file_read;
		f->write = __file_write;
		f->seek = __file_seek;
		f->close = __file_close;

		f->fifo_read = fifo_alloc(BUFSIZ);
		f->fifo_write = fifo_alloc(BUFSIZ);

		f->buf = malloc(BUFSIZ);
		f->bufsz = BUFSIZ;
		f->rwflush = &__stdio_no_flush;

		f->pos = 0;
		f->mode = _IOFBF;
		f->error = 0;
		f->eof = 0;
	}

	return f;
}

FILE * __stdio_get_stdin(void)
{
	if(!__stdin)
		__stdin = __file_alloc(0);
	return __stdin;
}
EXPORT_SYMBOL(__stdio_get_stdin);

FILE * __stdio_get_stdout(void)
{
	if(!__stdout)
		__stdout = __file_alloc(1);
	return __stdout;
}
EXPORT_SYMBOL(__stdio_get_stdout);

FILE * __stdio_get_stderr(void)
{
	if(!__stderr)
		__stderr = __file_alloc(2);
	return __stderr;
}
EXPORT_SYMBOL(__stdio_get_stderr);
