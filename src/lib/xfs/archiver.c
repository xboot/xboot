/*
 * xfs/archiver.c
 */

#include <xfs/xfs.h>

static char * cvt_to_dependent(void * opaque, const char * name)
{
	const char sep = __xfs_platform_directory_separator();
	char * buf, * p;

	if(!name)
		return NULL;

	buf = malloc(((opaque) ? strlen((char *)opaque) : 0) + strlen(name) + 1);
	if(!buf)
		return NULL;

	sprintf(buf, "%s%s", opaque ? (char *)opaque : "", name);
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
	char * retval = NULL;
	const size_t namelen = strlen(name);
	const size_t seplen = 1;
	int exists = 0;

	if(!__xfs_platform_stat(name, &exists, &st))
		return NULL;

	if(st.filetype != XFS_FILETYPE_DIRECTORY)
		return NULL;

	retval = malloc(namelen + seplen + 1);
	if(!retval)
		return NULL;

	strcpy(retval, name);

	if (retval[namelen - 1] != sep)
	{
		retval[namelen] = sep;
		retval[namelen + 1] = '\0';
	}

	return retval;
}

static struct xfs_io_t * direct_open(void * opaque, const char * name, const char mode, int * fileExists)
{
	struct xfs_stat_t statbuf;
	char * f;
	struct xfs_io_t * io = NULL;
	int existtmp = 0;

    f = cvt_to_dependent(opaque, name);
    if(!f)
    	return 0;

	if (fileExists == NULL)
		fileExists = &existtmp;

	io = __xfs_create_nativeio(f, mode);
	if (io == NULL)
	{
		__xfs_platform_stat(f, fileExists, &statbuf);
	}
	else
	{
		*fileExists = 1;
	}

	free(f);
	return io;
}

static struct xfs_io_t * direct_open_read(void * opaque, const char * fnm, int * exist)
{
	return direct_open(opaque, fnm, 'r', exist);
}

static struct xfs_io_t * direct_open_write(void * opaque, const char * filename)
{
	return direct_open(opaque, filename, 'w', NULL);
}

static struct xfs_io_t * direct_open_append(void * opaque, const char * filename)
{
	return direct_open(opaque, filename, 'a', NULL);
}

static int direct_mkdir(void * opaque, const char * name)
{
    char * f;
    int retval;

    f = cvt_to_dependent(opaque, name);
    if(!f)
    	return 0;

    retval = __xfs_platform_mkdir(f);
    free(f);

    return retval;
}

static int direct_remove(void * opaque, const char * name)
{
	char * f;
    int retval;

    f = cvt_to_dependent(opaque, name);
    if(!f)
    	return 0;

    retval = __xfs_platform_delete(f);
    free(f);

    return retval;
}

static int direct_stat(void *opaque, const char * name, int * exists, struct xfs_stat_t * stat)
{
    char * d;
    int retval = 0;

    d = cvt_to_dependent(opaque, name);
    if(!d)
    	return 0;

    retval = __xfs_platform_stat(d, exists, stat);
    free(d);

    return retval;
}

static void direct_enumerate_files(void * opaque, const char * dname, int omitSymLinks, xfs_enum_files_callback cb, const char * origdir, void * callbackdata)
{
    char * d;

    d = cvt_to_dependent(opaque, dname);
    if (d != NULL)
    {
    	__xfs_platform_enumerate_files(d, omitSymLinks, cb, origdir, callbackdata);
        free(d);
    }
}

static void direct_close_archive(void * opaque)
{
	free(opaque);
}

const struct xfs_archiver_t __xfs_archiver_direct = {
	.extension			= "",
	.description		= "Non-archive, direct filesystem I/O",

	.open_archive		= direct_open_archive,
	.open_read			= direct_open_read,
	.open_write			= direct_open_write,
	.open_append		= direct_open_append,
	.remove				= direct_remove,
	.mkdir				= direct_mkdir,
	.stat				= direct_stat,
	.enumerate_files	= direct_enumerate_files,
	.close_archive		= direct_close_archive,
};

const struct xfs_archiver_t ** __xfs_archiver_supported = {
	NULL,
};
