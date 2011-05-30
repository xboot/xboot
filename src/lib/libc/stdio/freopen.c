/*
 * libc/stdio/freopen.c
 */

#include <stdio.h>

FILE * freopen(const char * file, const char * mode, FILE * f)
{
	int flags = O_RDONLY;
	int plus = 0;

	if(f == NULL)
		return NULL;

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

	fflush(f);
	close(f->fd);

	f->fd = -1;
	f->eof = f->error = 0;
	f->ofs = 0;
	f->in.limit = f->in.pos = 0;
	f->in.dirty = 0;
	f->out.limit = f->out.pos = 0;
	f->out.dirty = 0;

	if (f->in.buf != NULL)
		free(f->in.buf);
	if (f->out.buf != NULL)
		free(f->out.buf);

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
