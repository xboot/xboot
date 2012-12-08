/*
 * framework/xfs/xfs.c
 */

#include <framework/xfs/platform.h>
#include <framework/xfs/archiver.h>
#include <framework/xfs/xfs.h>

static const struct xfs_archiver_t * const __xfs_archivers[] = {
	&__xfs_archiver_direct,
	&__xfs_archiver_zip,
	NULL,
};

static const char * find_filename_extension(const char * name)
{
	const char * ret = NULL;
	const char * p;

	if(name != NULL)
	{
		ret = p = strchr(name, '.');

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

static struct xfs_dir_handle_t * try_open_directory(const struct xfs_archiver_t * archive, const char * name, int forWriting)
{
	struct xfs_dir_handle_t * ret = NULL;
	void * handle;

	if(archive->is_archive(name, forWriting))
	{
		handle = archive->open_archive(name, forWriting);
		if(handle != NULL)
		{
			ret = (struct xfs_dir_handle_t *)malloc(sizeof(struct xfs_dir_handle_t));
			if(ret == NULL)
				archive->dir_close(handle);
			else
			{
				memset(ret, '\0', sizeof(struct xfs_dir_handle_t));
				ret->mpoint = NULL;
				ret->archiver = archive;
				ret->handle = handle;
			}
		}
	}

	return ret;
}

static struct xfs_dir_handle_t * open_directory(const char * name, int forWriting)
{
	struct xfs_dir_handle_t * ret = NULL;
	const struct xfs_archiver_t * const *i;
	const char * ext;

	if(!__xfs_platform_exists(name))
		return NULL;

	ext = find_filename_extension(name);
	if(ext != NULL)
	{
		for(i = __xfs_archivers; (*i != NULL ) && (ret == NULL); i++)
		{
			if(strcasecmp(ext, (*i)->extension) == 0)
				ret = try_open_directory(*i, name, forWriting);
		}

		for(i = __xfs_archivers; (*i != NULL) && (ret == NULL); i++)
		{
			if(strcasecmp(ext, (*i)->extension) != 0)
				ret = try_open_directory(*i, name, forWriting);
		}
	}
	else
	{
		for(i = __xfs_archivers; (*i != NULL) && (ret == NULL); i++)
			ret = try_open_directory(*i, name, forWriting);
	}

	return ret;
}

static bool_t sanitize_platform_independent_path(const char * src, char * dst)
{
    char * prev;
    char ch;

    while(*src == '/')
        src++;

    prev = dst;
    do
    {
        ch = *(src++);

        if((ch == ':') || (ch == '\\'))
        	return FALSE;

        if(ch == '/')
        {
            *dst = '\0';
            if ((strcmp(prev, ".") == 0) || (strcmp(prev, "..") == 0))
            	return FALSE;

            while(*src == '/')
                src++;

            if(*src == '\0')
                break;

            prev = dst + 1;
        }

        *(dst++) = ch;
    } while(ch != '\0');

    return TRUE;
}

static bool_t part_of_mount_point(struct xfs_dir_handle_t * dh, char * name)
{
    size_t len, mpoint_len;

    if(dh->mpoint == NULL)
    	return FALSE;
    else if(*name == '\0')
    	return TRUE;

    len = strlen(name);
    mpoint_len = strlen(dh->mpoint);
    if(len > mpoint_len)
    	return FALSE;

    if((len + 1) == mpoint_len)
    	return FALSE;

	if(strncmp(name, dh->mpoint, len) != 0)
    	return FALSE;

	if(dh->mpoint[len] == '/')
		return TRUE;
	return FALSE;
}

static struct xfs_dir_handle_t * create_dir_handle(const char * dir, const char * mpoint, int forWriting)
{
    struct xfs_dir_handle_t * dhandle = NULL;
    char * tmp = NULL;
    size_t len;

    if(!dir)
    	goto bad_dir_handle;

    if(mpoint != NULL)
    {
    	len = strlen(mpoint) + 1;
        tmp = (char *)malloc(len);
        if(!tmp)
        	goto bad_dir_handle;
        if(!sanitize_platform_independent_path(mpoint, tmp))
            goto bad_dir_handle;
        mpoint = tmp;
    }

    dhandle = open_directory(dir, forWriting);
    if(!dhandle)
    	goto bad_dir_handle;

    dhandle->dname = (char *)malloc(strlen(dir) + 1);
    if(!dhandle->dname)
    	goto bad_dir_handle;
    strcpy(dhandle->dname, dir);

    if((mpoint != NULL) && (*mpoint != '\0'))
    {
        dhandle->mpoint = (char *)malloc(strlen(mpoint) + 2);
        if(!dhandle->mpoint)
        	goto bad_dir_handle;
        strcpy(dhandle->mpoint, mpoint);
        strcat(dhandle->mpoint, "/");
    }

    free(tmp);
    return dhandle;

bad_dir_handle:
    if(dhandle != NULL )
	{
		dhandle->archiver->dir_close(dhandle->handle);
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

    if(dh == NULL)
        return TRUE;

    for(i = open_list; i != NULL; i = i->next)
    {
    	if(i->dhandle == dh)
    		return FALSE;
    }

	dh->archiver->dir_close(dh->handle);
	free(dh->dname);
	free(dh->mpoint);
	free(dh);
	return TRUE;
}

static bool_t close_file_handle_list(struct xfs_file_handle_t ** list)
{
	struct xfs_file_handle_t * i;
	struct xfs_file_handle_t * next = NULL;

	for(i = *list; i != NULL ; i = next)
	{
		next = i->next;
		if(!i->archiver->file_close(i->handle))
		{
			*list = i;
			return FALSE;
		}

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

	if(__xfs_platform_get_context()->search_path != NULL )
	{
		for(i = __xfs_platform_get_context()->search_path; i != NULL ; i = next)
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

	for(i = (void **)list; *i != NULL ; i++)
		free(*i);

	free(list);
}

const char * xfs_get_directory_separator(void)
{
	return __xfs_platform_directory_separator();
}

const char * xfs_get_base_dir(void)
{
	return __xfs_platform_get_context()->base_dir;
}

const char * xfs_get_user_dir(void)
{
	return __xfs_platform_get_context()->user_dir;
}

const char * xfs_get_write_dir(void)
{
	const char * ret = NULL;

	__xfs_platform_lock();

	if(__xfs_platform_get_context()->write_dir != NULL)
		ret = (__xfs_platform_get_context()->write_dir)->dname;

	__xfs_platform_unlock();
	return (ret);
}

bool_t xfs_set_write_dir(const char * dir)
{
	bool_t ret = TRUE;

	__xfs_platform_lock();

	if(__xfs_platform_get_context()->write_dir != NULL )
	{
		if (!free_dir_handle(__xfs_platform_get_context()->write_dir, __xfs_platform_get_context()->open_write_list))
		{
			__xfs_platform_unlock();
			return FALSE;
		}
		__xfs_platform_get_context()->write_dir = NULL;
	}

	if(dir != NULL )
	{
		__xfs_platform_get_context()->write_dir = create_dir_handle(dir, NULL, 1);
		ret = (__xfs_platform_get_context()->write_dir) ? TRUE : FALSE;
	}

	__xfs_platform_unlock();
	return (ret);
}

bool_t xfs_mount(const char * dir, const char * mpoint, int appendToPath)
{
	struct xfs_dir_handle_t * dh;
	struct xfs_dir_handle_t * prev = NULL;
	struct xfs_dir_handle_t * i;

	if(dir == NULL )
		return FALSE;

	if(mpoint == NULL )
		mpoint = "/";

	__xfs_platform_lock();

	for(i = __xfs_platform_get_context()->search_path; i != NULL ; i = i->next)
	{
		if(strcmp(dir, i->dname) == 0)
		{
			__xfs_platform_unlock();
			return TRUE;
		}
		prev = i;
	}

	dh = create_dir_handle(dir, mpoint, 0);
	if(dh == NULL )
	{
		__xfs_platform_unlock();
		return FALSE;
	}

	if(appendToPath)
	{
		if(prev == NULL)
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

bool_t xfs_add_to_search_path(const char * dir, int appendToPath)
{
	return xfs_mount(dir, NULL, appendToPath);
}

bool_t xfs_remove_from_search_path(const char * dir)
{
	struct xfs_dir_handle_t * i;
	struct xfs_dir_handle_t * prev = NULL;
	struct xfs_dir_handle_t * next = NULL;

	if(dir == NULL )
		return FALSE;

	__xfs_platform_lock();
	for(i = __xfs_platform_get_context()->search_path; i != NULL ; i = i->next)
	{
		if(strcmp(i->dname, dir) == 0)
		{
			next = i->next;
			if(!free_dir_handle(i, __xfs_platform_get_context()->open_read_list))
			{
				__xfs_platform_unlock();
				return FALSE;
			}

			if(prev == NULL)
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

const char * xfs_get_mount_point(const char * dir)
{
	struct xfs_dir_handle_t * i;
	char * ret;

	__xfs_platform_lock();

	for(i = __xfs_platform_get_context()->search_path; i != NULL ; i = i->next)
	{
		if(strcmp(i->dname, dir) == 0)
		{
			ret = ((i->mpoint) ? i->mpoint : "/");
			__xfs_platform_unlock();
			return ret;
		}
	}

	__xfs_platform_unlock();
	return NULL;
}

static bool_t verify_path(struct xfs_dir_handle_t * df, char ** name, int allowMissing)
{
	char * fname = *name;
	bool_t ret = TRUE;
	char * start;
	char * end;

	if(*fname == '\0')
		return TRUE;

	if(df->mpoint != NULL)
	{
		size_t mntpntlen = strlen(df->mpoint);
		size_t len = strlen(fname);
		assert(mntpntlen > 1);

		if(len < mntpntlen - 1)
			return FALSE;

		ret = strncmp(df->mpoint, fname, mntpntlen - 1);
		if(ret != 0)
			return FALSE;
		if (len > mntpntlen - 1)
		{
			if(fname[mntpntlen - 1] != '/')
				return FALSE;
		}
		fname += mntpntlen - 1;
		if (*fname == '/')
			fname++;
		*name = fname;
		ret = 1;
	}

	start = fname;

//xxx	if (!allowSymLinks)
	if(0)
	{
		while(1)
		{
			bool_t rc = FALSE;
			end = strchr(start, '/');

			if (end != NULL)
				*end = '\0';
			rc = df->archiver->is_symlink(df->handle, fname, &ret);
			if (end != NULL)
				*end = '/';

			if(rc)
				return FALSE;

			if (!ret)
			{
				if ((end == NULL )|| (allowMissing))
					ret = 1;
				break;
			}

			if (end == NULL )
				break;

			start = end + 1;
		}
	}

	return (ret);
}

static bool_t do_mkdir(const char * _dname, char * dname)
{
	struct xfs_dir_handle_t * h;
	char * start;
	char * end;
	bool_t ret = 0;
	int exists = 1;

	if (!sanitize_platform_independent_path(_dname, dname))
		return FALSE;

	__xfs_platform_lock();
	if (__xfs_platform_get_context()->write_dir == NULL )
	{
		__xfs_platform_unlock();
		return FALSE;
	}
	h = __xfs_platform_get_context()->write_dir;
	if (!verify_path(h, &dname, 1))
	{
		__xfs_platform_unlock();
		return FALSE;
	}

	start = dname;
	while (1)
	{
		end = strchr(start, '/');
		if (end != NULL)
			*end = '\0';

		if (exists)
			ret = h->archiver->is_directory(h->handle, dname, &exists);

		if (!exists)
			ret = h->archiver->mkdir(h->handle, dname);

		if (!ret)
			break;

		if (end == NULL)
			break;

		*end = '/';
		start = end + 1;
	}

	__xfs_platform_unlock();
	return ret;
}

bool_t xfs_mkdir(const char * dir)
{
	bool_t ret = FALSE;
	char * dname;
	size_t len;

	if (dir == NULL )
		return FALSE;

	len = strlen(dir) + 1;
	dname = (char *)malloc(len);
	if (dname == NULL )
		return FALSE;

	ret = do_mkdir(dir, dname);

	free(dname);
	return ret;
}

static bool_t do_delete(const char * _fname, char * fname)
{
	bool_t ret;
    struct xfs_dir_handle_t * dh;

    if(!sanitize_platform_independent_path(_fname, fname))
    	return FALSE;

    __xfs_platform_lock();

    if(__xfs_platform_get_context()->write_dir == NULL)
    {
    	__xfs_platform_unlock();
    	return FALSE;
    }
    dh = __xfs_platform_get_context()->write_dir;

    if(!verify_path(dh, &fname, 0))
    {
    	 __xfs_platform_unlock();
    	return FALSE;
    }
    ret = dh->archiver->remove(dh->handle, fname);

    __xfs_platform_unlock();
    return ret;
}

bool_t xfs_delete(const char * name)
{
	bool_t ret;
	char * fname;
	size_t len;

	if(name == NULL)
		return FALSE;

	len = strlen(name) + 1;
	fname = (char *) malloc(len);
	if(fname == NULL )
		return FALSE;

	ret = do_delete(name, fname);

	free(fname);
	return ret;
}

const char * xfs_get_real_dir(const char * name)
{
	struct xfs_dir_handle_t * i;
	const char * ret = NULL;
	char * fname = NULL;
	size_t len;

	if (name == NULL )
		return NULL ;
	len = strlen(name) + 1;
	fname = malloc(len);
	if (fname == NULL)
		return NULL ;

	if(sanitize_platform_independent_path(name, fname))
	{
		__xfs_platform_lock();

		for (i = __xfs_platform_get_context()->search_path; ((i != NULL )&& (ret == NULL)); i = i->next)
		{
			char * arcfname = fname;

			if (part_of_mount_point(i, arcfname))
				ret = i->dname;
			else if (verify_path(i, &arcfname, 0))
			{
				if (i->archiver->exists(i->handle, arcfname))
				ret = i->dname;
			}
		}

		__xfs_platform_unlock();
	}

	free(fname);
	return ret;
}

bool_t xfs_exists(const char * name)
{
	return (xfs_get_real_dir(name) != NULL);
}

s64_t xfs_get_last_modtime(const char * name)
{
    s64_t ret = -1;
    char *fname;
    size_t len;

    if(name == NULL)
    	return -1;
    len = strlen(name) + 1;
    fname = (char *)malloc(len);
    if(fname == NULL)
    	return -1;
    if (sanitize_platform_independent_path(name, fname))
    {
        if (*fname == '\0')
            ret = 1;
        else
        {
            struct xfs_dir_handle_t *i;
            int exists = 0;
            __xfs_platform_lock();
            for (i = __xfs_platform_get_context()->search_path; ((i != NULL) && (!exists)); i = i->next)
            {
                char *arcfname = fname;
                exists = part_of_mount_point(i, arcfname);
                if (exists)
                    ret = 1;
                else if (verify_path(i, &arcfname, 0))
                {
                    ret = i->archiver->get_last_modtime(i->handle, arcfname, &exists);
                }
            }
            __xfs_platform_unlock();
        }
    }

    free(fname);
    return(ret);
}

bool_t xfs_is_directory(const char * name)
{
    bool_t retval = FALSE;
    size_t len;
    char *fname;

    if(name == NULL)
    	return FALSE;
    len = strlen(name) + 1;
    fname = (char *)malloc(len);
    if(fname == NULL)
    	return FALSE;

    if (!sanitize_platform_independent_path(name, fname))
        retval = FALSE;

    else if (*fname == '\0')
        retval = TRUE;  /* Root is always a dir.  :) */

    else
    {
        struct xfs_dir_handle_t *i;
        int exists = 0;

        __xfs_platform_lock();
        for (i = __xfs_platform_get_context()->search_path; ((i != NULL) && (!exists)); i = i->next)
        {
            char *arcfname = fname;
            if ((exists = part_of_mount_point(i, arcfname)) != 0)
                retval = 1;
            else if (verify_path(i, &arcfname, 0))
                retval = i->archiver->is_directory(i->handle, arcfname, &exists);
        }
        __xfs_platform_unlock();
    }

    free(fname);
    return(retval);
}

bool_t xfs_is_symlink(const char * name)
{
	bool_t retval = FALSE;
    size_t len;
    char *fname;

    //xxx if(!allowSymLinks)
    if(0)
    	return FALSE;

    if(name == NULL)
    	return FALSE;

    len = strlen(name) + 1;
    fname = (char *) malloc(len);
    if(fname == NULL)
    	return FALSE;

    if (!sanitize_platform_independent_path(name, fname))
        retval = FALSE;

    else if (*fname == '\0')
        retval = TRUE;  /* Root is never a symlink. */

    else
    {
        struct xfs_dir_handle_t *i;
        int fileExists = 0;

        __xfs_platform_lock();
        for (i = __xfs_platform_get_context()->search_path; ((i != NULL) && (!fileExists)); i = i->next)
        {
            char *arcfname = fname;
            if ((fileExists = part_of_mount_point(i, arcfname)) != 0)
                retval = FALSE;  /* virtual dir...not a symlink. */
            else if (verify_path(i, &arcfname, 0))
                retval = i->archiver->is_symlink(i->handle, arcfname, &fileExists);
        }
        __xfs_platform_unlock();
    }

    free(fname);
    return(retval);
}

static struct xfs_file_t *doOpenWrite(const char *_fname, int appending)
{
    struct xfs_file_handle_t *fh = NULL;
    size_t len;
    char *fname;

    if(_fname == NULL)
    	return NULL;

    len = strlen(_fname) + 1;
    fname = (char *) malloc(len);
    if(fname == NULL)
    	return NULL;

    if (sanitize_platform_independent_path(_fname, fname))
    {
        void *handle = NULL;
        struct xfs_dir_handle_t *h = NULL;
        const struct xfs_archiver_t *f;

        __xfs_platform_lock();

        if(!__xfs_platform_get_context()->write_dir)
        	goto doOpenWriteEnd;

        h = __xfs_platform_get_context()->write_dir;
        if(!verify_path(h, &fname, 0))
        	goto doOpenWriteEnd;

        f = h->archiver;
        if (appending)
            handle = f->open_append(h->handle, fname);
        else
            handle = f->open_write(h->handle, fname);

        if(handle == NULL)
        	goto doOpenWriteEnd;

        fh = (struct xfs_file_handle_t *) malloc(sizeof (struct xfs_file_handle_t));
        if (fh == NULL)
        {
            f->file_close(handle);
            goto doOpenWriteEnd;
        }
        else
        {
            memset(fh, '\0', sizeof (struct xfs_file_handle_t));
            fh->handle = handle;
            fh->dhandle = h;
            fh->archiver = h->archiver;
            fh->next = __xfs_platform_get_context()->open_write_list;
            __xfs_platform_get_context()->open_write_list = fh;
        }

doOpenWriteEnd:
        __xfs_platform_unlock();
    }

    free(fname);
    return((struct xfs_file_t *) fh);
}

struct xfs_file_t * xfs_open_write(const char * name)
{
    return(doOpenWrite(name, 0));
}

struct xfs_file_t * xfs_open_append(const char * name)
{
    return(doOpenWrite(name, 1));
}

struct xfs_file_t *xfs_open_read(const char * name)
{
    struct xfs_file_handle_t *fh = NULL;
    char *fname;
    size_t len;

    if(name == NULL)
    	return NULL;

    len = strlen(name) + 1;
    fname = (char *) malloc(len);
    if(fname == NULL)
    	return NULL;

    if (sanitize_platform_independent_path(name, fname))
    {
        int fileExists = 0;
        struct xfs_dir_handle_t *i = NULL;
        void *handle = NULL;

        __xfs_platform_lock();

        if(!__xfs_platform_get_context()->search_path)
        	goto openReadEnd;

        /* !!! FIXME: Why aren't we using a for loop here? */
        i = __xfs_platform_get_context()->search_path;

        do
        {
            char *arcfname = fname;
            if (verify_path(i, &arcfname, 0))
            {
                handle = i->archiver->open_read(i->handle, arcfname, &fileExists);
                if (handle)
                    break;
            } /* if */
            i = i->next;
        } while ((i != NULL) && (!fileExists));

        /* !!! FIXME: may not set an error if open_read didn't fail. */
        if(handle == NULL)
        	goto openReadEnd;

        fh = (struct xfs_file_handle_t *) malloc(sizeof (struct xfs_file_handle_t));
        if (fh == NULL)
        {
            i->archiver->file_close(handle);
            goto openReadEnd;
        } /* if */

        memset(fh, '\0', sizeof (struct xfs_file_handle_t));
        fh->handle = handle;
        fh->forReading = 1;
        fh->dhandle = i;
        fh->archiver = i->archiver;
        fh->next = __xfs_platform_get_context()->open_read_list;
        __xfs_platform_get_context()->open_read_list = fh;

        openReadEnd:
        __xfs_platform_unlock();
    } /* if */

    free(fname);
    return((struct xfs_file_t *) fh);
} /* xfs_open_read */


static int closeHandleInOpenList(struct xfs_file_handle_t **list, struct xfs_file_handle_t *handle)
{
    struct xfs_file_handle_t *prev = NULL;
    struct xfs_file_handle_t *i;
    int rc = 1;

    for (i = *list; i != NULL; i = i->next)
    {
        if (i == handle)  /* handle is in this list? */
        {
            u8_t *tmp = handle->buffer;
            rc = xfs_flush((struct xfs_file_t *) handle);
            if (rc)
                rc = handle->archiver->file_close(handle->handle);
            if (!rc)
                return(-1);

            if (tmp != NULL)  /* free any associated buffer. */
                free(tmp);

            if (prev == NULL)
                *list = handle->next;
            else
                prev->next = handle->next;

            free(handle);
            return(1);
        } /* if */
        prev = i;
    } /* for */

    return(0);
} /* closeHandleInOpenList */


bool_t xfs_close(struct xfs_file_t * h)
{
    struct xfs_file_handle_t * handle = (struct xfs_file_handle_t *)h;
    int rc;

    __xfs_platform_lock();

    /* -1 == close failure. 0 == not found. 1 == success. */
    rc = closeHandleInOpenList(&__xfs_platform_get_context()->open_read_list, handle);
    if(rc == -1)
    {
    	__xfs_platform_unlock();
    	return FALSE;
    }

    if (!rc)
    {
        rc = closeHandleInOpenList(&__xfs_platform_get_context()->open_write_list, handle);
        if(rc == -1)
        {
        	__xfs_platform_unlock();
        	return FALSE;
        }
    } /* if */

    __xfs_platform_unlock();
    if(!rc)
    	return FALSE;
    return(TRUE);
} /* xfs_close */


static s64_t doBufferedRead(struct xfs_file_handle_t *fh, void *buffer,
                                    u32_t objSize,
                                    u32_t objCount)
{
    s64_t retval = 0;
    u32_t remainder = 0;

    while (objCount > 0)
    {
        u32_t buffered = fh->buffill - fh->bufpos;
        u64_t mustread = (objSize * objCount) - remainder;
        u32_t copied;

        if (buffered == 0) /* need to refill buffer? */
        {
            s64_t rc = fh->archiver->read(fh->handle, fh->buffer,
                                                1, fh->bufsize);
            if (rc <= 0)
            {
                fh->bufpos -= remainder;
                return(((rc == -1) && (retval == 0)) ? -1 : retval);
            } /* if */

            buffered = fh->buffill = (u32_t) rc;
            fh->bufpos = 0;
        } /* if */

        if (buffered > mustread)
            buffered = (u32_t) mustread;

        memcpy(buffer, fh->buffer + fh->bufpos, (size_t) buffered);
        buffer = ((u8_t *) buffer) + buffered;
        fh->bufpos += buffered;
        buffered += remainder;  /* take remainder into account. */
        copied = (buffered / objSize);
        remainder = (buffered % objSize);
        retval += copied;
        objCount -= copied;
    } /* while */

    return(retval);
} /* doBufferedRead */


s64_t xfs_read(struct xfs_file_t *handle, void *buffer,
                          u32_t objSize, u32_t objCount)
{
    struct xfs_file_handle_t *fh = (struct xfs_file_handle_t *) handle;

    if(!fh->forReading)
    	return -1;
    if(objSize == 0)
    	return 0;
    if(objCount == 0)
    	return 0;
    if (fh->buffer != NULL)
        return(doBufferedRead(fh, buffer, objSize, objCount));

    return(fh->archiver->read(fh->handle, buffer, objSize, objCount));
} /* xfs_read */


static s64_t doBufferedWrite(struct xfs_file_t *handle, const void *buffer,
                                     u32_t objSize,
                                     u32_t objCount)
{
    struct xfs_file_handle_t *fh = (struct xfs_file_handle_t *) handle;

    /* whole thing fits in the buffer? */
    if (fh->buffill + (objSize * objCount) < fh->bufsize)
    {
        memcpy(fh->buffer + fh->buffill, buffer, objSize * objCount);
        fh->buffill += (objSize * objCount);
        return(objCount);
    } /* if */

    /* would overflow buffer. Flush and then write the new objects, too. */
    if(!xfs_flush(handle))
    	return (-1);
    return(fh->archiver->write(fh->handle, buffer, objSize, objCount));
} /* doBufferedWrite */


s64_t xfs_write(struct xfs_file_t *handle, const void *buffer,
                           u32_t objSize, u32_t objCount)
{
    struct xfs_file_handle_t *fh = (struct xfs_file_handle_t *) handle;

    if(fh->forReading)
    	return -1;
    if(objSize == 0)
    	return 0;
    if(objCount == 0)
    	return 0;
    if (fh->buffer != NULL)
        return(doBufferedWrite(handle, buffer, objSize, objCount));

    return(fh->archiver->write(fh->handle, buffer, objSize, objCount));
} /* PHYSFS_write */


bool_t xfs_eof(struct xfs_file_t * handle)
{
    struct xfs_file_handle_t *fh = (struct xfs_file_handle_t *) handle;

    if (!fh->forReading)  /* never EOF on files opened for write/append. */
        return FALSE;

    /* eof if buffer is empty and archiver says so. */
    return((fh->bufpos == fh->buffill) && (fh->archiver->eof(fh->handle)));
} /* PHYSFS_eof */

s64_t xfs_tell(struct xfs_file_t * handle)
{
    struct xfs_file_handle_t *fh = (struct xfs_file_handle_t *) handle;
    s64_t pos = fh->archiver->tell(fh->handle);
    s64_t retval = fh->forReading ?
                            (pos - fh->buffill) + fh->bufpos :
                            (pos + fh->buffill);
    return(retval);
} /* xfs_tell */


bool_t xfs_seek(struct xfs_file_t * handle, u64_t pos)
{
    struct xfs_file_handle_t * fh = (struct xfs_file_handle_t *) handle;

    if(!xfs_flush(handle))
    	return FALSE;

    if (fh->buffer && fh->forReading)
    {
        /* avoid throwing away our precious buffer if seeking within it. */
        s64_t offset = pos - xfs_tell(handle);
        if ( /* seeking within the already-buffered range? */
            ((offset >= 0) && (offset <= fh->buffill - fh->bufpos)) /* fwd */
            || ((offset < 0) && (-offset <= fh->bufpos)) /* backward */ )
        {
            fh->bufpos += (u32_t) offset;
            return TRUE; /* successful seek */
        } /* if */
    } /* if */

    /* we have to fall back to a 'raw' seek. */
    fh->buffill = fh->bufpos = 0;
    return(fh->archiver->seek(fh->handle, pos));
} /* PHYSFS_seek */


s64_t xfs_length(struct xfs_file_t * handle)
{
    struct xfs_file_handle_t *fh = (struct xfs_file_handle_t *) handle;
    return(fh->archiver->length(fh->handle));
}

int PHYSFS_setBuffer(struct xfs_file_t *handle, u64_t _bufsize)
{
    struct xfs_file_handle_t *fh = (struct xfs_file_handle_t *) handle;
    u32_t bufsize;

    /* !!! FIXME: Unlocalized string. */
    if(_bufsize > 0xFFFFFFFF)
    	return 0;
    bufsize = (u32_t) _bufsize;

    if(!xfs_flush(handle))
    	return 0;

    /*
     * For reads, we need to move the file pointer to where it would be
     *  if we weren't buffering, so that the next read will get the
     *  right chunk of stuff from the file. xfs_flush() handles writes.
     */
    if ((fh->forReading) && (fh->buffill != fh->bufpos))
    {
        u64_t pos;
        s64_t curpos = fh->archiver->tell(fh->handle);
        if(curpos == -1)
        	return 0;
        pos = ((curpos - fh->buffill) + fh->bufpos);
        if(!fh->archiver->seek(fh->handle, pos))
        	return 0;
    } /* if */

    if (bufsize == 0)  /* delete existing buffer. */
    {
        if (fh->buffer != NULL)
        {
            free(fh->buffer);
            fh->buffer = NULL;
        } /* if */
    } /* if */

    else
    {
        u8_t *newbuf;
        newbuf = (u8_t *) realloc(fh->buffer, bufsize);
        if(newbuf == NULL)
        	return 0;
        fh->buffer = newbuf;
    } /* else */

    fh->bufsize = bufsize;
    fh->buffill = fh->bufpos = 0;
    return(1);
} /* PHYSFS_setBuffer */


bool_t xfs_flush(struct xfs_file_t * handle)
{
    struct xfs_file_handle_t *fh = (struct xfs_file_handle_t *) handle;
    s64_t rc;

    if ((fh->forReading) || (fh->bufpos == fh->buffill))
        return TRUE;  /* open for read or buffer empty are successful no-ops. */

    /* dump buffer to disk. */
    rc = fh->archiver->write(fh->handle, fh->buffer + fh->bufpos,
                          fh->buffill - fh->bufpos, 1);
    if(rc <= 0)
    	return FALSE;
    fh->bufpos = fh->buffill = 0;
    return TRUE;
} /* xfs_flush */

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
    char *mpoint = (char *) malloc(slen);

    if (mpoint == NULL)
        return;  /* oh well. */

    strcpy(mpoint, i->mpoint);
    ptr = mpoint + ((len) ? len + 1 : 0);
    end = strchr(ptr, '/');
    assert(end);  /* should always find a terminating '/'. */
    *end = '\0';
    callback(data, _fname, ptr);
    free(mpoint);
} /* enumerateFromMountPoint */

/* !!! FIXME: this should report error conditions. */
void PHYSFS_enumerateFilesCallback(const char *_fname,
                                   xfs_enumerate_callback callback,
                                   void *data)
{
    size_t len;
    char *fname;

    if(_fname == NULL)
    	return;

    if(callback == NULL)
    	return;

    len = strlen(_fname) + 1;
    fname = (char *) malloc(len);
    if(fname == NULL)
    	return;

    if (sanitize_platform_independent_path(_fname, fname))
    {
        struct xfs_dir_handle_t *i;
        int noSyms;

        __xfs_platform_lock();
       //xxx noSyms = !allowSymLinks;
        for (i = __xfs_platform_get_context()->search_path; i != NULL; i = i->next)
        {
            char *arcfname = fname;
            if (part_of_mount_point(i, arcfname))
                enumerateFromMountPoint(i, arcfname, callback, _fname, data);

            else if (verify_path(i, &arcfname, 0))
            {
                i->archiver->enumerate(i->handle, arcfname, callback, _fname, data);
            }
        }
        __xfs_platform_unlock();
    }

    free(fname);
}

typedef struct
{
    char **list;
    u32_t size;
    const char *errorstr;
} EnumStringListCallbackData;

static int locateInStringList(const char *str,
                              char **list,
                              u32_t *pos)
{
    u32_t len = *pos;
    u32_t half_len;
    u32_t lo = 0;
    u32_t middle;
    int cmp;

    while (len > 0)
    {
        half_len = len >> 1;
        middle = lo + half_len;
        cmp = strcmp(list[middle], str);

        if (cmp == 0)  /* it's in the list already. */
            return(1);
        else if (cmp > 0)
            len = half_len;
        else
        {
            lo = middle + 1;
            len -= half_len + 1;
        } /* else */
    } /* while */

    *pos = lo;
    return(0);
} /* locateInStringList */

static void enumFilesCallback(void *data, const char *origdir, const char *str)
{
    u32_t pos;
    void *ptr;
    char *newstr;
    EnumStringListCallbackData *pecd = (EnumStringListCallbackData *) data;

    /*
     * See if file is in the list already, and if not, insert it in there
     *  alphabetically...
     */
    pos = pecd->size;
    if (locateInStringList(str, pecd->list, &pos))
        return;  /* already in the list. */

    ptr = realloc(pecd->list, (pecd->size + 2) * sizeof (char *));
    newstr = (char *) malloc(strlen(str) + 1);
    if (ptr != NULL)
        pecd->list = (char **) ptr;

    if ((ptr == NULL) || (newstr == NULL))
        return;  /* better luck next time. */

    strcpy(newstr, str);

    if (pos != pecd->size)
    {
        memmove(&pecd->list[pos+1], &pecd->list[pos],
                 sizeof (char *) * ((pecd->size) - pos));
    } /* if */

    pecd->list[pos] = newstr;
    pecd->size++;
} /* enumFilesCallback */

char ** PHYSFS_enumerateFiles(const char * path)
{
	EnumStringListCallbackData ecd;
    memset(&ecd, '\0', sizeof (ecd));
    ecd.list = (char **) malloc(sizeof (char *));
    if(ecd.list == NULL)
    	return NULL;
    PHYSFS_enumerateFilesCallback(path, enumFilesCallback, &ecd);
    ecd.list[ecd.size] = NULL;
    return(ecd.list);
}

bool_t xfs_init(const char * path)
{
	char * p;

	p = __xfs_platform_absolute_path(path);
	xfs_add_to_search_path(p, 1);
	free(p);

	return TRUE;
}

struct xfs_context_t * __xfs_alloc(void)
{
	struct xfs_context_t * ctx;

	ctx = malloc(sizeof(struct xfs_context_t));
	if(!ctx)
		return NULL;

	memset(ctx, 0, sizeof(struct xfs_context_t));
	return ctx;
}

void __xfs_free(struct xfs_context_t * ctx)
{
	if(!ctx)
		return;

	close_file_handle_list(&ctx->open_write_list);
	xfs_set_write_dir(NULL);
	free_search_path();

	if(ctx->base_dir)
		free(ctx->base_dir);
	if(ctx->user_dir)
		free(ctx->user_dir);

	free(ctx);
}

//-----------------------------------------------------------------------------
//xxx for test
static void printDir(void *data, const char *origdir, const char *fname)
{
	printk(" * We've got [%s] in [%s].\n", fname, origdir);
}

void tt(void)
{
	 int rc;
	struct xfs_file_t * f;
	 char buf[128];

    if (!xfs_add_to_search_path(".", 1))
    {
        fprintf(stderr, "PHYSFS_addToSearchPath(): %s\n", "error");
        return ;
    } /* if */

    if (! xfs_set_write_dir("."))
    {
        fprintf(stderr, "PHYSFS_setWriteDir(): %s\n", "error");
        return;
    } /* if */

    if (!xfs_mkdir("/a/b/c"))
    {
        fprintf(stderr, "PHYSFS_mkdir(): %s\n", "error");
        return;
    } /* if */

    if (!xfs_mkdir("/a/b/C"))
    {
        fprintf(stderr, "PHYSFS_mkdir(): %s\n", "error");
        return;
    } /* if */

    f = xfs_open_write("/a/b/c/x.txt");
    xfs_close(f);
    if (f == NULL)
    {
        fprintf(stderr, "xfs_open_write(): %s\n", "error");
        return ;
    } /* if */

    f = xfs_open_write("/a/b/C/X.txt");
    xfs_close(f);
    if (f == NULL)
    {
        fprintf(stderr, "xfs_open_write(): %s\n", "error");
        return ;
    } /* if */

    printk("Testing completed.\n");
    printk("  If no errors were reported, you're good to go.\n");

/*    xfs_delete("/a/b/c/x.txt");
    xfs_delete("/a/b/C/X.txt");
    xfs_delete("/a/b/c");
    xfs_delete("/a/b/C");
    xfs_delete("/a/b");
    xfs_delete("/a");*/

/*	xfs_add_to_search_path("/romdisk/test.zip", 1);
	xfs_set_write_dir("/tmp");

	printk("init\r\n");

    char **files = PHYSFS_enumerateFiles("/");
    char **i;
    for (i = files; *i != NULL; i++)
    {
        const char *dirorfile = xfs_is_directory(*i) ? "Directory" : "File";
        printk(" * %s '%s' is in root of attached data.\n", dirorfile, *i);
    }
    xfs_free_list(files);*/

}
