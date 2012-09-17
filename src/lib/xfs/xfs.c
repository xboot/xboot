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

static struct xfs_dir_handle_t * create_dir_handle(struct xfs_io_t * io, const char * dir, const char * mpoint, int forWriting)
{
	struct xfs_dir_handle_t * dhandle = NULL;
    char * tmp = NULL;
    size_t len;

    if (mpoint != NULL)
    {
    	len = strlen(mpoint) + 1;
        tmp = (char *) malloc(len);
        if(!tmp)
        	goto bad_dir_handle;

        if (!sanitize_platform_independent_Path(mpoint, tmp))
            goto bad_dir_handle;
        mpoint = tmp;
    }

    dhandle = open_directory(io, dir, forWriting);
    if(!dhandle)
    	goto bad_dir_handle;

    if (dir == NULL)
        dhandle->dname = NULL;
    else
    {
        dhandle->dname = (char *) malloc(strlen(dir) + 1);
        if (!dhandle->dname)
           	goto bad_dir_handle;
        strcpy(dhandle->dname, dir);
    }

    if ((mpoint != NULL) && (*mpoint != '\0'))
    {
        dhandle->mpoint = (char *)malloc(strlen(mpoint)+2);
        if (!dhandle->mpoint)
        	goto bad_dir_handle;
        strcpy(dhandle->mpoint, mpoint);
        strcat(dhandle->mpoint, "/");
    }

    free(tmp);
    return dhandle;

bad_dir_handle:
    if (dhandle != NULL)
    {
        dhandle->archiver->close_archive(dhandle->handle);
        free(dhandle->dname);
        free(dhandle->mpoint);
        free(dhandle);
    }

    free(tmp);
    return NULL;
}

static bool_t free_dir_handle(struct xfs_dir_handle_t * dh, struct xfs_file_handle_t * open_list)
{
	struct xfs_file_handle_t * i;

    if (dh == NULL)
        return TRUE;

    for (i = open_list; i != NULL; i = i->next)
    {
    	if(i->dhandle == dh)
    		return FALSE;
    }

    dh->archiver->close_archive(dh->handle);
    free(dh->dname);
    free(dh->mpoint);
    free(dh);
    return TRUE;
}

static char *calculateBaseDir(const char *argv0)
{
    const char dirsep = __xfs_platform_directory_separator();
    char *ret = NULL;
    char *ptr = NULL;

#if 0 //xxx to be review, by jjj
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
}

static bool_t do_mount(struct xfs_io_t * io, const char * fname, const char * mpoint, bool_t appendToPath)
{
	struct xfs_dir_handle_t * prev = NULL;
	struct xfs_dir_handle_t * dh;
	struct xfs_dir_handle_t * i;

    if (mpoint == NULL)
        mpoint = "/";

	__xfs_platform_lock();

	if(fname != NULL)
    {
    	for(i = __xfs_platform_get_context()->search_path; i != NULL; i = i->next)
    	{
            if ((i->dname != NULL) && (strcmp(fname, i->dname) == 0))
            {
            	 __xfs_platform_unlock();
            	 return TRUE;
            }
            prev = i;
        }
    }

    dh = create_dir_handle(io, fname, mpoint, 0);
    if(!dh)
    {
		__xfs_platform_unlock();
		return FALSE;
    }

    if (appendToPath)
    {
        if (prev == NULL)
        	__xfs_platform_get_context()->search_path = dh;
        else
            prev->next = dh;
    }
    else
    {
        dh->next = __xfs_platform_get_context()->search_path;
        __xfs_platform_get_context()->search_path = dh;
    }

    __xfs_platform_unlock();
    return TRUE;
}

bool_t xfs_mount(const char * dir, const char * mpoint, bool_t appendToPath)
{
	if (!dir)
		return FALSE;

	return do_mount(NULL, dir, mpoint, appendToPath);
}

bool_t xfs_add_to_search_path(const char * dir, bool_t appendToPath)
{
	return do_mount(NULL, dir, NULL, appendToPath);
}

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
}

static void enumerate_from_mount_point(struct xfs_dir_handle_t * i, const char * arcfname, xfs_enumerate_callback cb, const char * _fname, void * data)
{
	const size_t len = strlen(arcfname);
	char * ptr = NULL;
	char * end = NULL;
	const size_t slen = strlen(i->mpoint) + 1;
	char * mpoint = (char *) malloc(slen);

	if (mpoint == NULL)
		return;

	strcpy(mpoint, i->mpoint);
	ptr = mpoint + ((len) ? len + 1 : 0);
	end = strchr(ptr, '/');
	assert(end);
	*end = '\0';
	cb(data, _fname, ptr);
	free(mpoint);
}

void xfs_enumerate_files_callback(const char * _fname, xfs_enumerate_callback cb, void * data)
{
	size_t len;
	char * fname;

	if (!cb || !_fname)
		return;

	len = strlen(_fname) + 1;
	fname = (char *) malloc(len);
	if (!fname)
		return;

	if (sanitize_platform_independent_Path(_fname, fname))
	{
		struct xfs_dir_handle_t * i;

		__xfs_platform_lock();
		for (i = __xfs_platform_get_context()->search_path; i != NULL;
				i = i->next)
		{
			char * arcfname = fname;
			if (part_of_mount_point(i, arcfname))
				enumerate_from_mount_point(i, arcfname, cb, _fname, data);

			else if (verifyPath(i, &arcfname, 0))
			{
				i->archiver->enumerate(i->handle, arcfname, cb, _fname, data);
			}
		}
		__xfs_platform_unlock();
	}

	free(fname);
}

static bool_t close_file_handle_list(struct xfs_file_handle_t ** list)
{
	struct xfs_file_handle_t * i;
	struct xfs_file_handle_t * next = NULL;
	struct xfs_io_t * io;

	for(i = *list; i != NULL; i = next)
	{
		io = i->io;
		next = i->next;

		if (!io->flush(io))
		{
			*list = i;
			return FALSE;
		}

		io->destroy(io);
		free(i);
	}

	*list = NULL;
	return TRUE;
}

static void free_search_path(void)
{
	struct xfs_dir_handle_t * i;
	struct xfs_dir_handle_t * next = NULL;

	close_file_handle_list(&__xfs_platform_get_context()->open_read_list);

	if (__xfs_platform_get_context()->search_path != NULL)
	{
		for (i = __xfs_platform_get_context()->search_path; i != NULL; i = next)
		{
			next = i->next;
			free_dir_handle(i, __xfs_platform_get_context()->open_read_list);
		}
		__xfs_platform_get_context()->search_path = NULL;
	}
}

void xfs_free_list(void * list)
{
	void ** i;

	if (list != NULL)
	{
		for (i = (void **) list; *i != NULL; i++)
			free(*i);

		free(list);
	}
}

bool_t xfs_unmount(const char * dir)
{
	struct xfs_dir_handle_t * i;
	struct xfs_dir_handle_t * prev = NULL;
	struct xfs_dir_handle_t * next = NULL;

	if (!dir)
		return FALSE;

	__xfs_platform_lock();

	for (i = __xfs_platform_get_context()->search_path; i != NULL; i = i->next)
	{
		if (strcmp(i->dname, dir) == 0)
		{
			next = i->next;
			if(!free_dir_handle(i, __xfs_platform_get_context()->open_read_list))
			{
				__xfs_platform_unlock();
				return FALSE;
			}

			if (prev == NULL)
				__xfs_platform_get_context()->search_path = next;
			else
				prev->next = next;

			__xfs_platform_unlock();
			return TRUE;
		}
		prev = i;
	}

	__xfs_platform_unlock();
	return FALSE;
}

bool_t xfs_remove_from_search_path(const char * dir)
{
    return xfs_unmount(dir);
}

const char * xfs_get_mount_point(const char * dir)
{
	struct xfs_dir_handle_t * i;

	__xfs_platform_lock();

	for (i = __xfs_platform_get_context()->search_path; i != NULL; i = i->next)
	{
		if (strcmp(i->dname, dir) == 0)
		{
			const char * retval = ((i->mpoint) ? i->mpoint : "/");
			__xfs_platform_unlock();
			return retval;
		}
	}

	__xfs_platform_unlock();
	return NULL;
}

static int doMkdir(const char *_dname, char *dname)
{
	struct xfs_dir_handle_t *h;
	char *start;
	char *end;
	int retval = 0;
	int exists = 1;

	if(!sanitize_platform_independent_Path(_dname, dname))
		return 0;

	__xfs_platform_lock();
	if(!__xfs_platform_get_context()->write_dir)
	{
		__xfs_platform_unlock();
		return 0;
	}

	h = __xfs_platform_get_context()->write_dir;
	if(!verifyPath(h, &dname, 1))
	{
		__xfs_platform_unlock();
		return 0;
	}

	start = dname;
	while (1)
	{
		end = strchr(start, '/');
		if (end != NULL)
			*end = '\0';

		if (exists)
		{
			struct xfs_stat_t statbuf;
			const bool_t rc = h->archiver->stat(h->handle, dname, &statbuf);
			if(rc)
				exists = 1;
			else
				exists = 0;
			retval = ((rc) && (statbuf.type == XFS_FILETYPE_DIRECTORY));
		}

		if (!exists)
			retval = h->archiver->mkdir(h->handle, dname);

		if (!retval)
			break;

		if (end == NULL)
			break;

		*end = '/';
		start = end + 1;
	}

	__xfs_platform_unlock();
	return retval;
}

int xfs_mkdir(const char *_dname)
{
    int retval = 0;
    char *dname;
    size_t len;

    if(!_dname)
    	return 0;
    len = strlen(_dname) + 1;
    dname = (char *)malloc(len);
    if(!dname)
    	return 0;
    retval = doMkdir(_dname, dname);
    free(dname);
    return retval;
}

bool_t xfs_set_write_dir(const char * dir)
{
    bool_t ret = TRUE;

    __xfs_platform_lock();

    if(__xfs_platform_get_context()->write_dir != NULL)
    {
    	if(!free_dir_handle(__xfs_platform_get_context()->write_dir, __xfs_platform_get_context()->open_write_list))
    	{
    	    __xfs_platform_unlock();
    	    return FALSE;
    	}
    	__xfs_platform_get_context()->write_dir = NULL;
    }

    if(dir != NULL)
    {
    	__xfs_platform_get_context()->write_dir = create_dir_handle(NULL, dir, NULL, 1);
    	ret = (__xfs_platform_get_context()->write_dir) ? TRUE : FALSE;
    }

    __xfs_platform_unlock();
    return ret;
}

char * xfs_get_write_dir(void)
{
	char * ret = NULL;

	__xfs_platform_lock();

	if(__xfs_platform_get_context()->write_dir != NULL)
		ret = (__xfs_platform_get_context()->write_dir)->dname;

	__xfs_platform_unlock();
	return ret;
}

bool_t xfs_init(struct xfs_context_t * ctx)
{
	return TRUE;
}

bool_t xfs_exit(struct xfs_context_t * ctx)
{
	return TRUE;
}








//xxx
//-----------------------------------------------------------------------------
//xxx for test
static void printDir(void *data, const char *origdir, const char *fname)
{
	printk(" * We've got [%s] in [%s].\n", fname, origdir);
}

void tt(void)
{
	xfs_add_to_search_path("/", 1);
	xfs_set_write_dir("/");

	printk("init\r\n");
	xfs_enumerate_files_callback("/", printDir, NULL);

	xfs_mkdir("bba/a/b/c/d");
	xfs_enumerate_files_callback("/", printDir, NULL);
}
