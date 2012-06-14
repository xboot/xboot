/*
 * xfs/xfs.c
 */

#include <xfs/xfs.h>

static char * get_file_extension(const char * name)
{
	char * ret = NULL;
	char * p;

	if(name != NULL)
	{
		p = strchr(name, '.');
		ret = p;

		while(p != NULL)
		{
			p = strchr(p + 1, '.');
			if(p != NULL)
				ret = p;
		}

		if(ret != NULL)
			ret++;
	}

	return ret;
}

static struct xfs_dir_handle_t * try_open_directory(struct xfs_io_t * io, struct xfs_archiver_t * archiver, const char * dir, int forWriting)
{
	struct xfs_dir_handle_t * ret = NULL;
	void * handle;

	if (io != NULL)
	{
		if (!io->seek(io, 0))
			return NULL;
	}

	handle = archiver->open_archive(io, dir, forWriting);
	if (handle != NULL)
	{
		ret = (struct xfs_dir_handle_t *) malloc(sizeof(struct xfs_dir_handle_t));
		if (ret == NULL)
			archiver->close_archive(handle);
		else
		{
			memset(ret, 0, sizeof(struct xfs_dir_handle_t));
			ret->mpoint = NULL;
			ret->archiver = archiver;
			ret->handle = handle;
		}
	}

	return ret;
}

static struct xfs_dir_handle_t * open_directory(struct xfs_io_t * io, const char * dir, int forWriting)
{
	struct xfs_dir_handle_t * ret = NULL;
	struct xfs_archiver_t ** i;
	char * ext;

	if (io == NULL)
	{
		ret = try_open_directory(io, &__xfs_archiver_direct, dir, forWriting);
		if (ret != NULL)
			return ret;

		io = __xfs_create_nativeio(dir, forWriting ? 'w' : 'r');
		if (!io)
			return NULL;
	}

	ext = get_file_extension(dir);
	if (ext != NULL)
	{
		for (i = __xfs_archiver_supported; (*i != NULL) && (ret == NULL); i++)
		{
			if (strcasecmp(ext, (*i)->extension) == 0)
				ret = try_open_directory(io, *i, dir, forWriting);
		}

		for (i = __xfs_archiver_supported; (*i != NULL) && (ret == NULL); i++)
		{
			if (strcasecmp(ext, (*i)->extension) != 0)
				ret = try_open_directory(io, *i, dir, forWriting);
		}
	}
	else
	{
		for (i = __xfs_archiver_supported; (*i != NULL) && (ret == NULL); i++)
			ret = try_open_directory(io, *i, dir, forWriting);
	}

	return ret;
}

static bool_t sanitize_platform_independent_Path(const char * src, char * dst)
{
	char *prev;
	char ch;

	while (*src == '/')
		src++;

	prev = dst;
	do
	{
		ch = *(src++);

		if ((ch == ':') || (ch == '\\'))
			return FALSE;

		if (ch == '/')
		{
			*dst = '\0';
			if ((strcmp(prev, ".") == 0) || (strcmp(prev, "..") == 0))
				return FALSE;

			while (*src == '/')
				src++;

			if (*src == '\0')
				break;

			prev = dst + 1;
		}

		*(dst++) = ch;
	} while (ch != '\0');

	return TRUE;
}

static bool_t part_of_mount_point(struct xfs_dir_handle_t * dh, char * name)
{
	size_t len, mpoint_len;

	if (dh->mpoint == NULL)
		return FALSE;
	else if (*name == '\0')
		return TRUE;

	len = strlen(name);
	mpoint_len = strlen(dh->mpoint);
	if (len > mpoint_len)
		return FALSE;

	if ((len + 1) == mpoint_len)
		return FALSE;

	if(strncmp(name, dh->mpoint, len) != 0)
		return FALSE;

	return (dh->mpoint[len] == '/');
}

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

        if (!sanitize_platform_independent_Path(mountPoint, tmpmntpnt))
            goto badDirHandle;
        mountPoint = tmpmntpnt;  /* sanitized version. */
    } /* if */

    dirHandle = open_directory(io, newDir, forWriting);
    if(!dirHandle)
    	goto badDirHandle;

    if (newDir == NULL)
        dirHandle->dname = NULL;
    else
    {
        dirHandle->dname = (char *) malloc(strlen(newDir) + 1);
        if (!dirHandle->dname)
           	goto badDirHandle;
        strcpy(dirHandle->dname, newDir);
    } /* else */

    if ((mountPoint != NULL) && (*mountPoint != '\0'))
    {
        dirHandle->mpoint = (char *)malloc(strlen(mountPoint)+2);
        if (!dirHandle->mpoint)
        	goto badDirHandle;
        strcpy(dirHandle->mpoint, mountPoint);
        strcat(dirHandle->mpoint, "/");
    } /* if */

    free(tmpmntpnt);
    return dirHandle;

badDirHandle:
    if (dirHandle != NULL)
    {
        dirHandle->archiver->close_archive(dirHandle->handle);
        free(dirHandle->dname);
        free(dirHandle->mpoint);
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
    	if(i->dhandle == dh)
    		return 0;
    }

    dh->archiver->close_archive(dh->handle);
    free(dh->dname);
    free(dh->mpoint);
    free(dh);
    return 1;
} /* freeDirHandle */


static char *calculateBaseDir(const char *argv0)
{
    const char dirsep = __xfs_platform_directory_separator();
    char *ret = NULL;
    char *ptr = NULL;

#if 0
    /* Give the platform layer first shot at this. */
    ret = __PHYSFS_platformCalcBaseDir(argv0);
    if (ret != NULL)
        return ret;

    /* We need argv0 to go on. */
    BAIL_IF_MACRO(argv0 == NULL, PHYSFS_ERR_ARGV0_IS_NULL, NULL);

    ptr = strrchr(argv0, dirsep);
    if (ptr != NULL)
    {
        const size_t size = ((size_t) (ptr - argv0)) + 1;
        ret = (char *) allocator.Malloc(size + 1);
        BAIL_IF_MACRO(!ret, PHYSFS_ERR_OUT_OF_MEMORY, NULL);
        memcpy(ret, argv0, size);
        ret[size] = '\0';
        return ret;
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
            if ((i->dname != NULL) && (strcmp(fname, i->dname) == 0))
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


static int verifyPath(struct xfs_dir_handle_t *h, char **_fname, int allowMissing)
{
    char *fname = *_fname;
    int ret = 1;
    char *start;
    char *end;

    if (*fname == '\0')  /* quick rejection. */
        return 1;

    /* !!! FIXME: This codeblock sucks. */
    if (h->mpoint != NULL)  /* NULL mountpoint means "/". */
    {
        size_t mpoint_len = strlen(h->mpoint);
        size_t len = strlen(fname);
        assert(mpoint_len > 1); /* root mount points should be NULL. */
        /* not under the mountpoint, so skip this archive. */
        if(len < mpoint_len-1)
        	return 0;
        /* !!! FIXME: Case insensitive? */
        ret = strncmp(h->mpoint, fname, mpoint_len-1);
        if(ret != 0)
        	return 0;
        if (len > mpoint_len-1)  /* corner case... */
        {
        	if(fname[mpoint_len-1]!='/')
        		return 0;
        }
        fname += mpoint_len-1;  /* move to start of actual archive path. */
        if (*fname == '/')
            fname++;
        *_fname = fname;  /* skip mountpoint for later use. */
        ret = 1;  /* may be reset, below. */
    } /* if */

    start = fname;
/*    if (!allowSymLinks)
    {
        while (1)
        {
            PHYSFS_Stat statbuf;
            int rc = 0;
            end = strchr(start, '/');

            if (end != NULL) *end = '\0';
            rc = h->archiver->stat(h->opaque, fname, &ret, &statbuf);
            if (rc)
                rc = (statbuf.filetype == PHYSFS_FILETYPE_SYMLINK);
            if (end != NULL) *end = '/';

             insecure path (has a disallowed symlink in it)?
            BAIL_IF_MACRO(rc, PHYSFS_ERR_SYMLINK_FORBIDDEN, 0);

             break out early if path element is missing.
            if (!ret)
            {

                 * We need to clear it if it's the last element of the path,
                 *  since this might be a non-existant file we're opening
                 *  for writing...

                if ((end == NULL) || (allowMissing))
                    ret = 1;
                break;
            }  if

            if (end == NULL)
                break;

            start = end + 1;
        }  while
    }  if */

    return ret;
} /* verifyPath */

/*
 * Broke out to seperate function so we can use stack allocation gratuitously.
 */
static void enumerateFromMountPoint(struct xfs_dir_handle_t *i, const char *arcfname,
		xfs_enumerate_callback callback,
                                    const char *_fname, void *data)
{
    const size_t len = strlen(arcfname);
    char *ptr = NULL;
    char *end = NULL;
    const size_t slen = strlen(i->mpoint) + 1;
    char *mountPoint = (char *) malloc(slen);

    if (mountPoint == NULL)
        return;  /* oh well. */

    strcpy(mountPoint, i->mpoint);
    ptr = mountPoint + ((len) ? len + 1 : 0);
    end = strchr(ptr, '/');
    assert(end);  /* should always find a terminating '/'. */
    *end = '\0';
    callback(data, _fname, ptr);
    free(mountPoint);
} /* enumerateFromMountPoint */


/* !!! FIXME: this should report error conditions. */
void PHYSFS_enumerateFilesCallback(const char *_fname,
		xfs_enumerate_callback callback,
                                   void *data)
{
    size_t len;
    char *fname;

    if(!callback || !_fname)
    	return;

    len = strlen(_fname) + 1;
    fname = (char *) malloc(len);
    if(!fname)
    	return;

    if (sanitize_platform_independent_Path(_fname, fname))
    {
    	struct xfs_dir_handle_t *i;

        //__PHYSFS_platformGrabMutex(stateLock);
       // noSyms = !allowSymLinks;
        for (i = searchPath; i != NULL; i = i->next)
        {
            char *arcfname = fname;
            if (part_of_mount_point(i, arcfname))
                enumerateFromMountPoint(i, arcfname, callback, _fname, data);

            else if (verifyPath(i, &arcfname, 0))
            {
                i->archiver->enumerate(i->handle, arcfname,
                                         callback, _fname, data);
            } /* else if */
        } /* for */
        //__PHYSFS_platformReleaseMutex(stateLock);
    } /* if */

    free(fname);
} /* PHYSFS_enumerateFilesCallback */


 static void printDir(void *data, const char *origdir, const char *fname)
 {
     printk(" * We've got [%s] in [%s].\n", fname, origdir);
 }

void tt(void)
{
	PHYSFS_addToSearchPath("/romdisk", 1);
	printk("init\r\n");
	PHYSFS_enumerateFilesCallback("/", printDir, NULL);

}
