/*
 * xfs/archiver.c
 */

#include <xfs/xfs.h>

static char * cvt_to_dependent(void * handle, const char * name)
{
	const char sep = __xfs_platform_directory_separator();
	char * buf, * p;

	if(!name)
		return NULL;

	buf = malloc(((handle) ? strlen((char *)handle) : 0) + strlen(name) + 1);
	if(!buf)
		return NULL;

	sprintf(buf, "%s%s", handle ? (char *)handle : "", name);
	if(sep != '/')
	{
		for (p = strchr(buf, '/'); p != NULL; p = strchr(p + 1, '/'))
			*p = sep;
	}

    return buf;
}

static void * direct_open_archive(struct xfs_io_t * io, const char * name, int forWrite)
{
	struct xfs_stat_t st;
	const char sep = __xfs_platform_directory_separator();
	const size_t len = strlen(name);
	char * ret = NULL;

	if(!__xfs_platform_stat(name, &st))
		return NULL;

	if(st.type != XFS_FILETYPE_DIRECTORY)
		return NULL;

	ret = malloc(len + sizeof(sep) + 1);
	if(!ret)
		return NULL;

	strcpy(ret, name);

	if (ret[len - 1] != sep)
	{
		ret[len] = sep;
		ret[len + 1] = '\0';
	}

	return ret;
}

static struct xfs_io_t * direct_open(void * handle, const char * name, const char mode)
{
	struct xfs_stat_t st;
	struct xfs_io_t * io;
	char * f;

    f = cvt_to_dependent(handle, name);
    if(!f)
    	return 0;

	io = __xfs_create_nativeio(f, mode);
	if (io == NULL)
	{
		__xfs_platform_stat(f, &st);
	}

	free(f);
	return io;
}

static struct xfs_io_t * direct_open_read(void * handle, const char * name)
{
	return direct_open(handle, name, 'r');
}

static struct xfs_io_t * direct_open_write(void * handle, const char * name)
{
	return direct_open(handle, name, 'w');
}

static struct xfs_io_t * direct_open_append(void * handle, const char * name)
{
	return direct_open(handle, name, 'a');
}

static bool_t direct_mkdir(void * handle, const char * name)
{
	bool_t ret;
	char * f;

	f = cvt_to_dependent(handle, name);
	if(!f)
		return FALSE;

	ret = __xfs_platform_mkdir(f);
	free(f);

	return ret;
}

static bool_t direct_remove(void * handle, const char * name)
{
	bool_t ret;
	char * f;

	f = cvt_to_dependent(handle, name);
	if(!f)
		return FALSE;

	ret = __xfs_platform_delete(f);
	free(f);

	return ret;
}

static bool_t direct_stat(void * handle, const char * name, struct xfs_stat_t * stat)
{
	bool_t ret;
	char * f;

	f = cvt_to_dependent(handle, name);
	if(!f)
		return FALSE;

	ret = __xfs_platform_stat(f, stat);
	free(f);

	return ret;
}

static void direct_enumerate(void * handle, const char * dname, xfs_enumerate_callback cb, const char * odir, void * cbdata)
{
	char * f;

	f = cvt_to_dependent(handle, dname);
	if(f != NULL)
	{
		__xfs_platform_enumerate(f, cb, odir, cbdata);
		free(f);
	}
}

static void direct_close_archive(void * handle)
{
	free(handle);
}

struct xfs_archiver_t __xfs_archiver_direct = {
	.extension			= "",
	.description		= "Non-archive, direct I/O",

	.open_archive		= direct_open_archive,
	.open_read			= direct_open_read,
	.open_write			= direct_open_write,
	.open_append		= direct_open_append,
	.remove				= direct_remove,
	.mkdir				= direct_mkdir,
	.stat				= direct_stat,
	.enumerate			= direct_enumerate,
	.close_archive		= direct_close_archive,
};

struct xfs_archiver_t ** __xfs_archiver_supported = {
	NULL,
};
