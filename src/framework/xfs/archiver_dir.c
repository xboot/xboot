/*
 * framework/xfs/archiver_dir.c
 */

#include <framework/xfs/platform.h>
#include <framework/xfs/archiver.h>

static s64_t direct_read(void * handle, void * buf, u32_t size, u32_t count)
{
	s64_t ret;

	ret = __xfs_platform_read(handle, buf, size, count);
	return ret;
}

static s64_t direct_write(void *handle, const void * buf, u32_t size, u32_t count)
{
	s64_t ret;

	ret = __xfs_platform_write(handle, buf, size, count);
	return ret;
}

static bool_t direct_eof(void * handle)
{
	return (__xfs_platform_eof(handle));
}

static s64_t direct_tell(void * handle)
{
	return (__xfs_platform_tell(handle));
}

static bool_t direct_seek(void * handle, u64_t pos)
{
	return (__xfs_platform_seek(handle, pos));
}

static s64_t direct_length(void * handle)
{
	return (__xfs_platform_length(handle));
}

static bool_t direct_file_close(void * handle)
{
	if(!__xfs_platform_flush(handle))
		return FALSE;

	if(!__xfs_platform_close(handle))
		return FALSE;

	return TRUE;
}

static bool_t direct_is_archive(const char * name, int forWriting)
{
	return (__xfs_platform_is_directory(name));
}

static void * direct_open_archive(const char * name, int forWriting)
{
	const char * dirsep = __xfs_platform_directory_separator();
	char * ret = NULL;
	size_t namelen = strlen(name);
	size_t seplen = strlen(dirsep);

	if(!direct_is_archive(name, forWriting))
		return NULL;

	ret = malloc(namelen + seplen + 1);
	if(!ret)
		return NULL;

	strcpy(ret, name);
	if (strcmp((name + namelen) - seplen, dirsep) != 0)
		strcat(ret, dirsep);

	return (ret);
}

static void direct_enumerate(void * handle, const char * dname, xfs_enumerate_callback cb, const char * odir, void * cbdata)
{
	char * d = __xfs_platform_cvt_to_dependent((char *)handle, dname, NULL);

	if(d != NULL )
	{
		__xfs_platform_enumerate(d, cb, odir, cbdata);
		free(d);
	}
}

static bool_t direct_exists(void * handle, const char * name)
{
	char * f = __xfs_platform_cvt_to_dependent((char *)handle, name, NULL);
	bool_t ret;

	if(!f)
		return FALSE;

	ret = __xfs_platform_exists(f);
	free(f);

	return ret;
}

static bool_t direct_is_directory(void * handle, const char * name, int * fileExists)
{
	char * d = __xfs_platform_cvt_to_dependent((char *)handle, name, NULL);
	bool_t ret = FALSE;

	if(!d)
		return FALSE;

	*fileExists = __xfs_platform_exists(d);
	if (*fileExists)
		ret = __xfs_platform_is_directory(d);
	free(d);

	return ret;
}

static bool_t direct_is_symlink(void * handle, const char * name, int * fileExists)
{
	char * f = __xfs_platform_cvt_to_dependent((char *)handle, name, NULL);
	bool_t ret = FALSE;

	if(!f)
		return FALSE;

	*fileExists = __xfs_platform_exists(f);
	if (*fileExists)
		ret = __xfs_platform_is_link(f);
	free(f);

	return ret;
}

static s64_t direct_get_last_modtime(void * handle, const char * name, int * fileExists)
{
	char * d = __xfs_platform_cvt_to_dependent((char *)handle, name, NULL);
	s64_t ret = -1;

	if(!d)
		return ret;

	*fileExists = __xfs_platform_exists(d);
	if (*fileExists)
		ret = __xfs_platform_get_last_modtime(d);
	free(d);

	return ret;
}

static void * direct_open(void * handle, const char * name, void *(*func)(const char * name), int * fileExists)
{
	char * f = __xfs_platform_cvt_to_dependent((char *)handle, name, NULL);
	void * rc = NULL;

	if(!f)
		return NULL;

	if (fileExists != NULL )
	{
		*fileExists = __xfs_platform_exists(f);
		if (!(*fileExists))
		{
			free(f);
			return (NULL );
		}
	}

	rc = func(f);
	free(f);

	return ((void *) rc);
}

static void * direct_open_read(void * handle, const char * name, int * fileExists)
{
	return (direct_open(handle, name, __xfs_platform_open_read, fileExists));
}

static void * direct_open_write(void * handle, const char * name)
{
	return (direct_open(handle, name, __xfs_platform_open_write, NULL));
}

static void * direct_open_append(void * handle, const char * name)
{
	return (direct_open(handle, name, __xfs_platform_open_append, NULL));
}

static bool_t direct_remove(void * handle, const char * name)
{
	char * f = __xfs_platform_cvt_to_dependent((char *)handle, name, NULL);
	bool_t ret;

	if(!f)
		return FALSE;

	ret = __xfs_platform_delete(f);
	free(f);

	return ret;
}

static bool_t direct_mkdir(void * handle, const char * name)
{
	char * f = __xfs_platform_cvt_to_dependent((char *)handle, name, NULL);
	bool_t ret;

	if(!f)
		return FALSE;

	ret = __xfs_platform_mkdir(f);
	free(f);

	return ret;
}

static void direct_dir_close(void * handle)
{
	free(handle);
}

const struct xfs_archiver_t __xfs_archiver_direct = {
	.extension			= "",
	.description		= "Non-archive, direct I/O",

	.is_archive			= direct_is_archive,
	.open_archive		= direct_open_archive,
	.enumerate			= direct_enumerate,
	.exists				= direct_exists,
	.is_directory		= direct_is_directory,
	.is_symlink			= direct_is_symlink,
	.get_last_modtime	= direct_get_last_modtime,
	.open_read			= direct_open_read,
	.open_write			= direct_open_write,
	.open_append		= direct_open_append,
	.remove				= direct_remove,
	.mkdir				= direct_mkdir,
	.dir_close			= direct_dir_close,
	.read				= direct_read,
	.write				= direct_write,
	.eof				= direct_eof,
	.tell				= direct_tell,
	.seek				= direct_seek,
	.length				= direct_length,
	.file_close			= direct_file_close,
};
