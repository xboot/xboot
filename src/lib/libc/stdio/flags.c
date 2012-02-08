/*
 * libc/stdio/flags.c
 */

#include <fs/fileio.h>
#include <stdio.h>

/*
 * Return the (stdio) flags for a given mode. Store the flags
 * to be passed to an open() syscall through * optr.
 * Return 0 on error.
 */
int __sflags(const char * mode, int * optr)
{
	int ret, m, o;

	switch (*mode++)
	{
	case 'r':	/* open for reading */
		ret = __SRD;
		m = O_RDONLY;
		o = 0;
		break;

	case 'w':	/* open for writing */
		ret = __SWR;
		m = O_WRONLY;
		o = O_CREAT | O_TRUNC;
		break;

	case 'a':	/* open for appending */
		ret = __SWR;
		m = O_WRONLY;
		o = O_CREAT | O_APPEND;
		break;

	default:	/* illegal mode */
		errno = EINVAL;
		return (0);
	}

	/* [rwa]\+ or [rwa]b\+ means read and write */
	if (*mode == '+' || (*mode == 'b' && mode[1] == '+'))
	{
		ret = __SRW;
		m = O_RDWR;
	}

	*optr = m | o;
	return (ret);
}
