/*
 * libc/stdio/fopen.c
 */

#include <errno.h>
#include <stdio.h>
#include <vfs/vfs.h>
#include <xboot/module.h>

FILE * fopen(const char * path, const char * mode)
{
	FILE * f;
	int flags = O_RDONLY;
	int plus = 0;
	int fd;

	if((path == NULL) || (*path == '\0'))
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

	fd = vfs_open(path, flags, 0644);
	if(fd < 0)
		return NULL;

	f = __file_alloc(fd);
	if(!f)
	{
		vfs_close(fd);
		return NULL;
	}
	f->pos = vfs_lseek(f->fd, 0, VFS_SEEK_CUR);

	return f;
}
EXPORT_SYMBOL(fopen);
