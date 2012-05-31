/*
 * xfs/xfs.c
 */

#include <xfs/xfs.h>

struct xfs_dir_handle_t
{
    void * pirv;
    char * dir_name;
    char * mount_point;
    const struct xfs_archiver_t * funcs;
    struct xfs_dir_handle_t * next;
};

struct xfs_file_handle_t
{
	struct xfs_io_t * io;
    u8_t forReading;
    const struct xfs_dir_handle_t * dir_handle;
    u8_t * buffer;
    u32_t bufsize;
    u32_t buffill;
    u32_t bufpos;
    struct xfs_file_handle_t * next;
};

struct xfs_err_state_t
{
    void * tid;
    enum xfs_err code;
    struct xfs_err_state_t * next;
};

//-----------------------
struct xfs {
	struct xfs_err_state_t * errorStates;
	struct xfs_dir_handle_t * searchPath;
	struct xfs_dir_handle_t * writeDir;
	struct xfs_file_handle_t * openWriteList;
	struct xfs_file_handle_t * openReadList;
	char * baseDir;
	char * userDir;
	char * prefDir;
	int allowSymLinks;
	const struct xfs_archiver_t ** archivers;
	const struct xfs_archive_info_t ** archiveInfo;

	void * errorLock;
	void * stateLock;
};

//=========================================================================





static const char * find_filename_extension(const char * name)
{
	const char * retval = NULL;

	if(name != NULL)
	{
		const char * p = strchr(name, '.');
		retval = p;

		while(p != NULL)
		{
			p = strchr(p + 1, '.');
			if(p != NULL)
				retval = p;
		}

		if(retval != NULL)
			retval++;
	}

	return retval;
}

#if 0
static struct xfs_dir_handle_t * tryOpenDir(struct xfs_io_t * io, const struct xfs_archiver_t * funcs, const char * d, int forWriting)
{
	struct xfs_dir_handle_t * retval = NULL;
    void * opaque = NULL;

    if(io != NULL)
    {
		if(!io->seek(io, 0))
			return NULL;
    }

    opaque = funcs->openArchive(io, d, forWriting);
    if (opaque != NULL)
    {
        retval = (struct xfs_dir_handle_t *)malloc(sizeof(struct xfs_dir_handle_t));
        if (retval == NULL)
            funcs->closeArchive(opaque);
        else
        {
            memset(retval, 0, sizeof(struct xfs_dir_handle_t));
            retval->mount_point = NULL;
            retval->funcs = funcs;
            retval->pirv = opaque;
        }
    }

    return retval;
}

static struct xfs_dir_handle_t * openDirectory(struct xfs_io_t * io, const char * d, int forWriting)
{
	struct xfs_dir_handle_t * retval = NULL;
	const struct xfs_archiver_t ** i;
	const char * ext;

	if(io == NULL)
	{
		retval = tryOpenDir(io, &__PHYSFS_Archiver_DIR, d, forWriting);
		if (retval != NULL)
			return retval;

		io = __PHYSFS_createNativeIo(d, forWriting ? 'w' : 'r');
		BAIL_IF_MACRO(!io, ERRPASS, 0);
	}

	ext = find_filename_extension(d);
	if (ext != NULL)
	{
		/* Look for archivers with matching file extensions first... */
		for (i = archivers; (*i != NULL) && (retval == NULL); i++)
		{
			if (__PHYSFS_stricmpASCII(ext, (*i)->info.extension) == 0)
				retval = tryOpenDir(io, *i, d, forWriting);
		} /* for */

		/* failing an exact file extension match, try all the others... */
		for (i = archivers; (*i != NULL) && (retval == NULL); i++)
		{
			if (__PHYSFS_stricmpASCII(ext, (*i)->info.extension) != 0)
				retval = tryOpenDir(io, *i, d, forWriting);
		} /* for */
	} /* if */

	else /* no extension? Try them all. */
	{
		for (i = archivers; (*i != NULL) && (retval == NULL); i++)
			retval = tryOpenDir(io, *i, d, forWriting);
	} /* else */

	BAIL_IF_MACRO(!retval, PHYSFS_ERR_UNSUPPORTED, NULL);
	return retval;
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
