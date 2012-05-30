/*
 * xfs/platform.c
 */

#include <xboot.h>
#include <xfs/platform.h>

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


char * __xfs_platform_directory_separator(void)
{
	return ("/");
}

void * __xfs_platform_open_read(const char * path)
{
    //return doOpen(filename, O_RDONLY);
    return 0;
}

void * __xfs_platform_open_write(const char * path)
{
    //return doOpen(filename, O_WRONLY | O_CREAT | O_TRUNC);
	return 0;
}

void * __xfs_platform_open_append(const char * path)
{
    //return doOpen(filename, O_WRONLY | O_CREAT | O_APPEND);
	return 0;
}

s64_t __xfs_platform_read(void * handle, void * buf, u64_t len)
{
/*
    const int fd = *((int *) opaque);
    ssize_t rc = 0;

    if (!__PHYSFS_ui64FitsAddressSpace(len))
        BAIL_MACRO(PHYSFS_ERR_INVALID_ARGUMENT, -1);

    rc = read(fd, buffer, (size_t) len);
    BAIL_IF_MACRO(rc == -1, errcodeFromErrno(), -1);
    assert(rc >= 0);
    assert(rc <= len);
    return (PHYSFS_sint64) rc;
*/
	return 0;
}

s64_t __xfs_platform_write(void * handle, const void * buf, u64_t len)
{
/*	const int fd = *((int *) opaque);
	ssize_t rc = 0;

	if (!__PHYSFS_ui64FitsAddressSpace(len))
		BAIL_MACRO(PHYSFS_ERR_INVALID_ARGUMENT, -1);

	rc = write(fd, (void *) buffer, (size_t) len);
	BAIL_IF_MACRO(rc == -1, errcodeFromErrno(), rc);
	assert(rc >= 0);
	assert(rc <= len);
	return (PHYSFS_sint64) rc;*/
	return 0;
}

int __xfs_platform_seek(void * handle, u64_t pos)
{
/*    const int fd = *((int *) opaque);
    const int rc = lseek(fd, (off_t) pos, SEEK_SET);
    BAIL_IF_MACRO(rc == -1, errcodeFromErrno(), 0);
    return 1;*/
	return 0;
}

s64_t __xfs_platform_tell(void * handle)
{
/*    const int fd = *((int *) opaque);
    PHYSFS_sint64 retval;
    retval = (PHYSFS_sint64) lseek(fd, 0, SEEK_CUR);
    BAIL_IF_MACRO(retval == -1, errcodeFromErrno(), -1);
    return retval;*/
	return 0;
}

s64_t __xfs_platform_length(void * handle)
{
/*    const int fd = *((int *) opaque);
    struct stat statbuf;
    BAIL_IF_MACRO(fstat(fd, &statbuf) == -1, errcodeFromErrno(), -1);
    return ((PHYSFS_sint64) statbuf.st_size);*/
	return 0;
}

int __xfs_platform_flush(void * handle)
{
/*    const int fd = *((int *) opaque);
    BAIL_IF_MACRO(fsync(fd) == -1, errcodeFromErrno(), 0);
    return 1;*/
	return 0;
}

void __xfs_platform_close(void * handle)
{
/*    const int fd = *((int *) opaque);
    (void) close(fd);   we don't check this. You should have used flush!
    allocator.Free(opaque);*/
	return 0;
}

int __xfs_platform_mkdir(const char * path)
{
	return 0;
}

int __xfs_platform_delete(const char * path)
{
/*    BAIL_IF_MACRO(remove(path) == -1, errcodeFromErrno(), 0);
    return 1;*/
	return 0;
}

int __xfs_platform_stat(const char * path, int * exists, struct xfs_stat_t * st)
{
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
