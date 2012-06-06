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
//    enum xfs_err code;
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

static struct xfs_dir_handle_t * tryOpenDir(struct xfs_io_t * io, const struct xfs_archiver_t * funcs, const char * d, int forWriting)
{
	struct xfs_dir_handle_t * retval = NULL;
    void * opaque = NULL;

    if(io != NULL)
    {
		if(!io->seek(io, 0))
			return NULL;
    }

    opaque = funcs->open_archive(io, d, forWriting);
    if (opaque != NULL)
    {
        retval = (struct xfs_dir_handle_t *)malloc(sizeof(struct xfs_dir_handle_t));
        if (retval == NULL)
            funcs->close_archive(opaque);
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
		retval = tryOpenDir(io, &__xfs_archiver_direct, d, forWriting);
		if (retval != NULL)
			return retval;

		io = __xfs_create_nativeio(d, forWriting ? 'w' : 'r');
		if(!io)
			return NULL;
	}

	ext = find_filename_extension(d);
	if (ext != NULL)
	{
		/* Look for archivers with matching file extensions first... */
		for (i = __xfs_archiver_supported; (*i != NULL) && (retval == NULL); i++)
		{
			if (strcasecmp(ext, (*i)->extension) == 0)
				retval = tryOpenDir(io, *i, d, forWriting);
		} /* for */

		/* failing an exact file extension match, try all the others... */
		for (i = __xfs_archiver_supported; (*i != NULL) && (retval == NULL); i++)
		{
			if (strcasecmp(ext, (*i)->extension) != 0)
				retval = tryOpenDir(io, *i, d, forWriting);
		} /* for */
	} /* if */

	else /* no extension? Try them all. */
	{
		for (i = __xfs_archiver_supported; (*i != NULL) && (retval == NULL); i++)
			retval = tryOpenDir(io, *i, d, forWriting);
	} /* else */

	if(!retval)
		return NULL;
	return retval;
}
/*
 * Make a platform-independent path string sane. Doesn't actually check the
 *  file hierarchy, it just cleans up the string.
 *  (dst) must be a buffer at least as big as (src), as this is where the
 *  cleaned up string is deposited.
 * If there are illegal bits in the path (".." entries, etc) then we
 *  return zero and (dst) is undefined. Non-zero if the path was sanitized.
 */
static int sanitizePlatformIndependentPath(const char *src, char *dst)
{
    char *prev;
    char ch;

    while (*src == '/')  /* skip initial '/' chars... */
        src++;

    prev = dst;
    do
    {
        ch = *(src++);

        if ((ch == ':') || (ch == '\\'))  /* illegal chars in a physfs path. */
            ;//xxxBAIL_MACRO(PHYSFS_ERR_BAD_FILENAME, 0);

        if (ch == '/')   /* path separator. */
        {
            *dst = '\0';  /* "." and ".." are illegal pathnames. */
            if ((strcmp(prev, ".") == 0) || (strcmp(prev, "..") == 0))
                ;//xxx BAIL_MACRO(PHYSFS_ERR_BAD_FILENAME, 0);

            while (*src == '/')   /* chop out doubles... */
                src++;

            if (*src == '\0') /* ends with a pathsep? */
                break;  /* we're done, don't add final pathsep to dst. */

            prev = dst + 1;
        } /* if */

        *(dst++) = ch;
    } while (ch != '\0');

    return 1;
} /* sanitizePlatformIndependentPath */


/*
 * Figure out if (fname) is part of (h)'s mountpoint. (fname) must be an
 *  output from sanitizePlatformIndependentPath(), so that it is in a known
 *  state.
 *
 * This only finds legitimate segments of a mountpoint. If the mountpoint is
 *  "/a/b/c" and (fname) is "/a/b/c", "/", or "/a/b/c/d", then the results are
 *  all zero. "/a/b" will succeed, though.
 */
static int partOfMountPoint(struct xfs_dir_handle_t *h, char *fname)
{
    /* !!! FIXME: This code feels gross. */
    int rc;
    size_t len, mntpntlen;

    if (h->mount_point == NULL)
        return 0;
    else if (*fname == '\0')
        return 1;

    len = strlen(fname);
    mntpntlen = strlen(h->mount_point);
    if (len > mntpntlen)  /* can't be a subset of mountpoint. */
        return 0;

    /* if true, must be not a match or a complete match, but not a subset. */
    if ((len + 1) == mntpntlen)
        return 0;

    rc = strncmp(fname, h->mount_point, len); /* !!! FIXME: case insensitive? */
    if (rc != 0)
        return 0;  /* not a match. */

    /* make sure /a/b matches /a/b/ and not /a/bc ... */
    return h->mount_point[len] == '/';
} /* partOfMountPoint */

static struct xfs_dir_handle_t *createDirHandle(struct xfs_io_t *io, const char *newDir,
                                  const char *mountPoint, int forWriting)
{
	struct xfs_dir_handle_t *dirHandle = NULL;
    char *tmpmntpnt = NULL;

    if (mountPoint != NULL)
    {
        const size_t len = strlen(mountPoint) + 1;
        tmpmntpnt = (char *) malloc(len);
        if(!tmpmntpnt)
        	goto badDirHandle;

        if (!sanitizePlatformIndependentPath(mountPoint, tmpmntpnt))
            goto badDirHandle;
        mountPoint = tmpmntpnt;  /* sanitized version. */
    } /* if */

    dirHandle = openDirectory(io, newDir, forWriting);
    if(!dirHandle)
    	goto badDirHandle;

    if (newDir == NULL)
        dirHandle->dir_name = NULL;
    else
    {
        dirHandle->dir_name = (char *) malloc(strlen(newDir) + 1);
        if (!dirHandle->dir_name)
           	goto badDirHandle;
        strcpy(dirHandle->dir_name, newDir);
    } /* else */

    if ((mountPoint != NULL) && (*mountPoint != '\0'))
    {
        dirHandle->mount_point = (char *)malloc(strlen(mountPoint)+2);
        if (!dirHandle->mount_point)
        	goto badDirHandle;
        strcpy(dirHandle->mount_point, mountPoint);
        strcat(dirHandle->mount_point, "/");
    } /* if */

    free(tmpmntpnt);
    return dirHandle;

badDirHandle:
    if (dirHandle != NULL)
    {
        dirHandle->funcs->close_archive(dirHandle->pirv);
        free(dirHandle->dir_name);
        free(dirHandle->mount_point);
        free(dirHandle);
    } /* if */

    free(tmpmntpnt);
    return NULL;
} /* createDirHandle */


/* MAKE SURE you've got the stateLock held before calling this! */
static int freeDirHandle(struct xfs_dir_handle_t *dh, struct xfs_file_handle_t *openList)
{
	struct xfs_file_handle_t *i;

    if (dh == NULL)
        return 1;

    for (i = openList; i != NULL; i = i->next)
    {
    	if(i->dir_handle == dh)
    		return 0;
    }

    dh->funcs->close_archive(dh->pirv);
    free(dh->dir_name);
    free(dh->mount_point);
    free(dh);
    return 1;
} /* freeDirHandle */


static char *calculateBaseDir(const char *argv0)
{
    const char dirsep = __xfs_platform_directory_separator();
    char *retval = NULL;
    char *ptr = NULL;

#if 0
    /* Give the platform layer first shot at this. */
    retval = __PHYSFS_platformCalcBaseDir(argv0);
    if (retval != NULL)
        return retval;

    /* We need argv0 to go on. */
    BAIL_IF_MACRO(argv0 == NULL, PHYSFS_ERR_ARGV0_IS_NULL, NULL);

    ptr = strrchr(argv0, dirsep);
    if (ptr != NULL)
    {
        const size_t size = ((size_t) (ptr - argv0)) + 1;
        retval = (char *) allocator.Malloc(size + 1);
        BAIL_IF_MACRO(!retval, PHYSFS_ERR_OUT_OF_MEMORY, NULL);
        memcpy(retval, argv0, size);
        retval[size] = '\0';
        return retval;
    } /* if */

    /* argv0 wasn't helpful. */
    BAIL_MACRO(PHYSFS_ERR_INVALID_ARGUMENT, NULL);
#endif
} /* calculateBaseDir */

//xxx
static struct xfs_dir_handle_t *searchPath = NULL;
static int doMount(struct xfs_io_t *io, const char *fname,
                   const char *mountPoint, int appendToPath)
{
	struct xfs_dir_handle_t *dh;
	struct xfs_dir_handle_t *prev = NULL;
	struct xfs_dir_handle_t *i;

    if (mountPoint == NULL)
        mountPoint = "/";

    //xxx __PHYSFS_platformGrabMutex(stateLock);

    if (fname != NULL)
    {
        for (i = searchPath; i != NULL; i = i->next)
        {
            /* already in search path? */
            if ((i->dir_name != NULL) && (strcmp(fname, i->dir_name) == 0))
                ;//xxx BAIL_MACRO_MUTEX(ERRPASS, stateLock, 1);
            prev = i;
        } /* for */
    } /* if */

    dh = createDirHandle(io, fname, mountPoint, 0);
    //xxxBAIL_IF_MACRO_MUTEX(!dh, ERRPASS, stateLock, 0);

    if (appendToPath)
    {
        if (prev == NULL)
            searchPath = dh;
        else
            prev->next = dh;
    } /* if */
    else
    {
        dh->next = searchPath;
        searchPath = dh;
    } /* else */

    //xxx __PHYSFS_platformReleaseMutex(stateLock);
    return 1;
} /* doMount */

int PHYSFS_mount(const char *newDir, const char *mountPoint, int appendToPath)
{
    if(!newDir)
    	return 0;

    return doMount(NULL, newDir, mountPoint, appendToPath);
} /* PHYSFS_mount */

int PHYSFS_addToSearchPath(const char *newDir, int appendToPath)
{
    return doMount(NULL, newDir, NULL, appendToPath);
} /* PHYSFS_addToSearchPath */


//xxx
void tt(void)
{
	PHYSFS_addToSearchPath("/", 1);
	printk("init\r\n");

}
