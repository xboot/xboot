/*
 * xfs/archiver.c
 */

#include <xfs/archiver.h>

#if 0
static char *cvtToDependent(const char *prepend, const char *path, char *buf)
{
    BAIL_IF_MACRO(buf == NULL, PHYSFS_ERR_OUT_OF_MEMORY, NULL);
    sprintf(buf, "%s%s", prepend ? prepend : "", path);

    if (__PHYSFS_platformDirSeparator != '/')
    {
        char *p;
        for (p = strchr(buf, '/'); p != NULL; p = strchr(p + 1, '/'))
            *p = __PHYSFS_platformDirSeparator;
    } /* if */

    return buf;
} /* cvtToDependent */


#define CVT_TO_DEPENDENT(buf, pre, dir) { \
    const size_t len = ((pre) ? strlen((char *) pre) : 0) + strlen(dir) + 1; \
    buf = cvtToDependent((char*)pre,dir,(char*)__PHYSFS_smallAlloc(len)); \
}




#define BAIL_MACRO(e, r) do { if (e) __PHYSFS_setError(e); return r; } while (0)
#define BAIL_IF_MACRO(c, e, r) do { if (c) { if (e) __PHYSFS_setError(e); return r; } } while (0)
#define BAIL_MACRO_MUTEX(e, m, r) do { if (e) __PHYSFS_setError(e); __PHYSFS_platformReleaseMutex(m); return r; } while (0)
#define BAIL_IF_MACRO_MUTEX(c, e, m, r) do { if (c) { if (e) __PHYSFS_setError(e); __PHYSFS_platformReleaseMutex(m); return r; } } while (0)
#define GOTO_MACRO(e, g) do { if (e) __PHYSFS_setError(e); goto g; } while (0)
#define GOTO_IF_MACRO(c, e, g) do { if (c) { if (e) __PHYSFS_setError(e); goto g; } } while (0)
#define GOTO_MACRO_MUTEX(e, m, g) do { if (e) __PHYSFS_setError(e); __PHYSFS_platformReleaseMutex(m); goto g; } while (0)
#define GOTO_IF_MACRO_MUTEX(c, e, m, g) do { if (c) { if (e) __PHYSFS_setError(e); __PHYSFS_platformReleaseMutex(m); goto g; } } while (0)
#endif

static void * direct_open_archive(struct xfs_io_t * io, const char * name, int forWrite)
{
	struct xfs_stat_t st;
	const char dirsep = __xfs_platform_directory_separator();
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

	if (retval[namelen - 1] != dirsep)
	{
		retval[namelen] = dirsep;
		retval[namelen + 1] = '\0';
	}

	return retval;
}

static struct xfs_io_t * direct_open(void * opaque, const char * name, const int mode, int * fileExists)
{
#if 0
	char * f;
	struct xfs_io_t * io = NULL;
	int existtmp = 0;

	CVT_TO_DEPENDENT(f, opaque, name);
	BAIL_IF_MACRO(!f, ERRPASS, NULL);

	if (fileExists == NULL)
		fileExists = &existtmp;

	io = __PHYSFS_createNativeIo(f, mode);
	if (io == NULL)
	{
		const PHYSFS_ErrorCode err = PHYSFS_getLastErrorCode();
		PHYSFS_Stat statbuf;
		__PHYSFS_platformStat(f, fileExists, &statbuf);
		__PHYSFS_setError(err);
	} /* if */
	else
	{
		*fileExists = 1;
	} /* else */

	__PHYSFS_smallFree(f);

	return io;
#endif
	return NULL;
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
#if 0
    int retval;
    char * f;

    CVT_TO_DEPENDENT(f, opaque, name);
    BAIL_IF_MACRO(!f, ERRPASS, 0);
    retval = __PHYSFS_platformMkDir(f);
    __PHYSFS_smallFree(f);
    return retval;
#endif
    return 0;
}

static int direct_remove(void * opaque, const char * name)
{
#if 0
    int retval;
    char *f;

    CVT_TO_DEPENDENT(f, opaque, name);
    BAIL_IF_MACRO(!f, ERRPASS, 0);
    retval = __PHYSFS_platformDelete(f);
    __PHYSFS_smallFree(f);
    return retval;
#endif
    return 0;
}

static int direct_stat(void *opaque, const char * name, int * exists, struct xfs_stat_t * stat)
{
#if 0
    int retval = 0;
    char *d;

    CVT_TO_DEPENDENT(d, opaque, name);
    BAIL_IF_MACRO(!d, ERRPASS, 0);
    retval = __PHYSFS_platformStat(d, exists, stat);
    __PHYSFS_smallFree(d);
    return retval;
#endif
    return 0;
}

static void direct_enumerate_files(void * opaque, const char * dname, int omitSymLinks, xfs_enum_files_callback cb, const char * origdir, void * callbackdata)
{
#if 0
    char * d;

    CVT_TO_DEPENDENT(d, opaque, dname);
    if (d != NULL)
    {
        __PHYSFS_platformEnumerateFiles(d, omitSymLinks, cb,
                                        origdir, callbackdata);
        __PHYSFS_smallFree(d);
    }
#endif
}

static void direct_close_archive(void * opaque)
{
	free(opaque);
}

const struct xfs_archiver_t __xfs_archiver_direct = {
	.info = {
		.extension		= "",
		.description	= "Non-archive, direct filesystem I/O",
	},

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
