#include <x.h>
#include <sandbox.h>

int sandbox_file_open(const char * path, const char * mode)
{
	int flags = O_RDONLY;
	int plus = 0;

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

	return open(path, flags, (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH));
}

int sandbox_file_close(int fd)
{
	int ret = close(fd);
	return ret < 0 ? 0 : 1;
}

int sandbox_file_isdir(const char * path)
{
	struct stat st;
	int ret = 0;

	if((stat(path, &st) == 0) && S_ISDIR(st.st_mode))
		ret = 1;
	return ret;
}

int sandbox_file_isfile(const char * path)
{
	struct stat st;
	int ret = 0;

	if((stat(path, &st) == 0) && S_ISREG(st.st_mode))
		ret = 1;
	return ret;
}

int sandbox_file_mkdir(const char * path)
{
	int ret = 0;

	if(mkdir(path, (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)) == 0)
		ret = 1;
	return ret;
}

int sandbox_file_remove(const char * path)
{
	struct stat st;
	int ret = 0;

	if(stat(path, &st) == 0)
	{
		if(S_ISDIR(st.st_mode))
			ret = (rmdir(path) == 0) ? 1 : 0;
		else if(S_ISREG(st.st_mode))
			ret = (unlink(path) == 0) ? 1 : 0;
	}
	return ret;
}

int sandbox_file_access(const char * path, const char * mode)
{
	int m = F_OK;

	while(*mode)
	{
		switch(*mode++)
		{
		case 'r':
			m |= R_OK;
			break;
		case 'w':
			m |= W_OK;
			break;
		case 'x':
			m |= X_OK;
			break;
		default:
			break;
		}
	}
	if(access(path, m) == 0)
		return 1;
	return 0;
}

void sandbox_file_walk(const char * path, void (*cb)(const char * dir, const char * name, void * data), const char * dir, void * data)
{
	struct dirent * entry;
	void * d;

	if((d = opendir(path)) == NULL)
		return;
	while((entry = readdir(d)) != NULL)
	{
		if(strcmp(entry->d_name, ".") == 0)
			continue;
		else if(strcmp(entry->d_name, "..") == 0)
			continue;
		cb(dir, entry->d_name, data);
	}
	closedir(d);
}

ssize_t sandbox_file_read(int fd, void * buf, size_t count)
{
	ssize_t ret = read(fd, buf, count);
	return (ret > 0) ? ret: 0;
}

ssize_t sandbox_file_read_nonblock(int fd, void * buf, size_t count)
{
	ssize_t ret;
	int flags;

	flags = fcntl(fd, F_GETFL);
	if(flags == -1)
		return 0;

	if(fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
		return 0;

	ret = sandbox_file_read(fd, buf, count);

	if(fcntl(fd, F_SETFL, flags) == -1)
		return 0;

	return ret;
}

ssize_t sandbox_file_write(int fd, const void * buf, size_t count)
{
	ssize_t ret = write(fd, buf, count);
	return (ret > 0) ? ret: 0;
}

int64_t sandbox_file_seek(int fd, int64_t offset)
{
	int64_t len = (int64_t)lseek(fd, 0, SEEK_END);
	if(offset < 0)
		offset = 0;
	else if(offset > len)
		offset = len;
	return (int64_t)lseek(fd, offset, SEEK_SET);
}

int64_t sandbox_file_tell(int fd)
{
	return (int64_t)lseek(fd, 0, SEEK_CUR);
}

int64_t sandbox_file_length(int fd)
{
	off_t off, ret;
	off = lseek(fd, 0, SEEK_CUR);
	ret = lseek(fd, 0, SEEK_END);
	lseek(fd, off, SEEK_SET);
	return (ret > 0) ? (int64_t)ret: 0;
}
