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
	//xxx
/*    BAIL_IF_MACRO(remove(path) == -1, errcodeFromErrno(), 0);
    return 1;*/
	return 0;
}

bool_t __xfs_platform_stat(const char * path, int * exists, struct xfs_stat_t * st)
{
	//xxx
 /*   struct stat statbuf;

    if (lstat(filename, &statbuf) == -1)
    {
        *exists = (errno == ENOENT);
        BAIL_MACRO(errcodeFromErrno(), 0);
    }

    *exists = 1;

    if (S_ISREG(statbuf.st_mode))
    {
        st->filetype = PHYSFS_FILETYPE_REGULAR;
        st->filesize = statbuf.st_size;
    }

    else if(S_ISDIR(statbuf.st_mode))
    {
        st->filetype = PHYSFS_FILETYPE_DIRECTORY;
        st->filesize = 0;
    }

    else
    {
        st->filetype = PHYSFS_FILETYPE_OTHER;
        st->filesize = statbuf.st_size;
    }

    st->modtime = statbuf.st_mtime;
    st->createtime = statbuf.st_ctime;
    st->accesstime = statbuf.st_atime;

     !!! FIXME: maybe we should just report full permissions?
    st->readonly = access(filename, W_OK);
    return 1;*/

	return 0;
}

void __xfs_platform_enumerate_files(const char * dirname, int omitSymLinks, xfs_enum_files_callback callback, const char *origdir, void *callbackdata)
{
	//xxx

}
