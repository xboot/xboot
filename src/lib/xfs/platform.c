/*
 * xfs/platform.c
 */

#include <xfs/xfs.h>

bool_t __xfs_platform_init(void)
{
	return TRUE;
}

bool_t __xfs_platform_exit(void)
{
	return TRUE;
}


void * __xfs_platform_create_mutex(void)
{
	return 0;
}

void __xfs_platform_destory_mutex(void * mutex)
{
}

void __xfs_platform_lock_mutex(void * mutex)
{
}

void __xfs_platform_unlock_mutex(void * mutex)
{
}


char __xfs_platform_directory_separator(void)
{
	return ('/');
}

static void * __do_open(const char * path, const char * mode)
{
	FILE *f, *ret;

	f = fopen(path, mode);
	if(!f)
		return NULL;

	ret = (FILE *)malloc(sizeof(FILE *));
	if(!ret)
	{
		fclose(f);
		return NULL;
	}

	ret = f;
	return ((void *)ret);
}

void * __xfs_platform_open_read(const char * path)
{
	return __do_open(path, "r");
}

void * __xfs_platform_open_write(const char * path)
{
	return __do_open(path, "w");
}

void * __xfs_platform_open_append(const char * path)
{
	return __do_open(path, "a");
}

u64_t __xfs_platform_read(void * handle, void * buf, u64_t len)
{
	FILE * f = handle;
	u8_t * p = buf;
	size_t sz;
	u64_t ret = 0;

	while(len > 0)
	{
		sz = (len > SZ_2G) ? SZ_2G : (size_t)len;

		if(fread(p, sz, 1, f) != 1)
			break;

		len -= sz;
		ret += sz;
		p += sz;
	}

	return ret;
}

u64_t __xfs_platform_write(void * handle, const void * buf, u64_t len)
{
	FILE * f = handle;
	const u8_t * p = buf;
	size_t sz;
	u64_t ret = 0;

	while(len > 0)
	{
		sz = (len > SZ_2G) ? SZ_2G : (size_t)len;

		if(fwrite(p, sz, 1, f) != 1)
			break;

		len -= sz;
		ret += sz;
		p += sz;
	}

	return ret;
}

u64_t __xfs_platform_tell(void * handle)
{
	FILE * f = handle;

	return ((u64_t)ftell(f));
}

u64_t __xfs_platform_length(void * handle)
{
	FILE * f = handle;
	fpos_t pos = ftell(f);
	u64_t len = 0;

	if(fseek(f, 0, SEEK_END) == 0)
	{
		len = ftell(f);
		fseek(f, pos, SEEK_SET);
	}

	return len;
}

bool_t __xfs_platform_seek(void * handle, u64_t pos)
{
	FILE * f = handle;

	if(fseek(f, pos, SEEK_SET) == 0)
		return TRUE;

	return FALSE;
}

bool_t __xfs_platform_flush(void * handle)
{
	FILE * f = handle;

	if(fflush(f) == 0)
		return TRUE;

	return FALSE;
}

bool_t __xfs_platform_close(void * handle)
{
	FILE * f = handle;

	fclose(f);
	free(f);

	return TRUE;
}

bool_t __xfs_platform_mkdir(const char * path)
{
	if(mkdir(path, (S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)) == 0)
		return TRUE;
	return FALSE;
}

bool_t __xfs_platform_delete(const char * path)
{
	struct stat _st;
	int ret;

	if(stat(path, &_st) != 0)
		return FALSE;

    if(S_ISDIR(_st.st_mode))
        ret = rmdir(path);
    else
        ret = unlink(path);

    if(ret == 0)
    	return TRUE;
    return FALSE;
}

bool_t __xfs_platform_stat(const char * path, struct xfs_stat_t * st)
{
	struct stat _st;

	if(stat(path, &_st) != 0)
		return FALSE;

	if(S_ISREG(_st.st_mode))
	{
		st->size = _st.st_size;
		st->type = XFS_FILETYPE_REGULAR;
	}
	else if(S_ISDIR(_st.st_mode))
	{
		st->size = 0;
		st->type = XFS_FILETYPE_DIRECTORY;
	}
	else
	{
		st->size = _st.st_size;
		st->type = XFS_FILETYPE_OTHER;
	}

    st->ctime = _st.st_ctime;
    st->atime = _st.st_atime;
    st->mtime = _st.st_mtime;

    return TRUE;
}

void __xfs_platform_enumerate(const char * path, xfs_enumerate_callback cb, const char * odir, void * cbdata)
{
	void * dir;
	struct dirent * entry;

	if( (dir = opendir(path)) == NULL )
    	return;

	while ((entry = readdir(dir)) != NULL)
	{
		if (strcmp(entry->d_name, ".") == 0)
			continue;

		if (strcmp(entry->d_name, "..") == 0)
			continue;

		cb(cbdata, odir, entry->d_name);
	}

	closedir(dir);
}
