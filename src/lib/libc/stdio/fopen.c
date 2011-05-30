/*
 * libc/stdio/fopen.c
 */

#include <stdio.h>

FILE * fopen(const char * file, const char * mode)
{
	int flags = O_RDONLY;
	int plus = 0;
	FILE * f;

	if ((file == NULL) || (*file == '\0'))
	{
		errno = EINVAL;
		return NULL;
	}

	while(*mode)
	{
		switch(*mode++)
		{
		case 'r':
			flags = O_RDONLY;
			break;
		case 'w':
			flags = O_WRONLY | O_CREAT | O_TRUNC;
			break;
		case 'a':
			flags = O_WRONLY | O_CREAT | O_APPEND;
			break;
		case '+':
			plus = 1;
			break;
		}
	}

	if(plus)
		flags = (flags & ~(O_RDONLY | O_WRONLY)) | O_RDWR;

	f = __create_stdio();
	if (f == NULL)
		return NULL;

	if ((flags & O_RDONLY) != 0)
	{
		f->in.buf = malloc(BUFSIZ);
		if (f->in.buf == NULL)
			return NULL;
	}

	if ((flags & O_WRONLY) != 0)
	{
		f->out.buf = malloc(BUFSIZ);
		if (f->out.buf == NULL)
			return NULL;
	}

	f->fd = open(file, flags, (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH));
	if(f->fd < 0)
	{
		__destroy_stdio(f);
		return NULL;
	}

	f->ofs = lseek(f->fd, 0, VFS_SEEK_CUR);
	if (f->ofs < 0)
	{
		close(f->fd);
		__destroy_stdio(f);
		return NULL;
	}

	return f;
}
