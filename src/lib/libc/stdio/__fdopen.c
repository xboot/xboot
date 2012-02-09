/*
 * libc/stdio/__fdopen.c
 */

#include <malloc.h>
#include <stdio.h>

FILE * __fdopen(int fd, const char * mode)
{
	FILE *f, *head;
//xxx	struct termios tio;
	int plus = !!strchr(mode, '+');

	/*
	 * Check for valid initial mode character
	 */
	if (!strchr("rwa", *mode))
		return 0;

	/*
	 * Allocate FILE+buffer or fail
	 */
	if (!(f = malloc(sizeof(*f) + UNGET + BUFSIZ)))
		return 0;

	/*
	 * Zero-fill only the struct, not the buffer
	 */
	memset(f, 0, sizeof(*f));

	/*
	 * Impose mode restrictions
	 */
	if (!plus)
		f->flags = (*mode == 'r') ? F_NOWR : F_NORD;

	/*
	 * Set append mode on fd if opened for append
	 */
	if (*mode == 'a')
	{
		//xxx int flags = __syscall(SYS_fcntl, fd, F_GETFL);
		//xxx __syscall(SYS_fcntl, fd, F_SETFL, flags | O_APPEND);
	}

	f->fd = fd;
	f->buf = (unsigned char *)f + sizeof(*f) + UNGET;
	f->buf_size = BUFSIZ;

	/*
	 * Activate line buffered mode for terminals
	 */
	f->lbf = EOF;
	//xxx if (!(f->flags & F_NOWR) && !__syscall(SYS_ioctl, fd, TCGETS, &tio))
	//xxx	f->lbf = '\n';

	/*
	 * Initialize op ptrs. No problem if some are unneeded.
	 */
	f->read = __stdio_read;
	f->write = __stdio_write;
	f->seek = __stdio_seek;
	f->close = __stdio_close;

	//xxx if (!libc.threaded)
		f->lock = -1;

	/*
	 * Add new FILE to open file list
	 */
	OFLLOCK();
	head = __get_runtime()->ofl_head;
	f->next = head;
	if (head)
		head->prev = f;
	head = f;
	OFLUNLOCK();

	return f;
}
