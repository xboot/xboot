/*
 * xfs/platform.c
 */

#include <xfs/platform.h>

struct xfs_context_t * __xfs_platform_init(void)
{
	struct xfs_context_t * ctx;

	ctx = malloc(sizeof(struct xfs_context_t));
	if(!ctx)
		return NULL;

	memset(ctx, 0, sizeof(struct xfs_context_t));
	xfs_init(ctx);

	return ctx;
}

void __xfs_platform_exit(struct xfs_context_t * ctx)
{
	if(!ctx)
		return;

	xfs_exit(ctx);
	free(ctx);
}

void __xfs_platform_lock(void)
{
	void * lock = __get_runtime()->__xfs_ctx->lock;
	lock = 0;
}

void __xfs_platform_unlock(void)
{
	void * lock = __get_runtime()->__xfs_ctx->lock;
	lock = 0;
}

struct xfs_context_t * __xfs_platform_get_context(void)
{
	return (__get_runtime()->__xfs_ctx);
}

const char * __xfs_platform_directory_separator(void)
{
	return ("/");
}

char * __xfs_platform_cvt_to_dependent(const char * prepend, const char * dirname, const char * append)
{
	int len = ((prepend) ? strlen(prepend) : 0)	+ ((append) ? strlen(append) : 0) + strlen(dirname) + 1;
	char * buf = malloc(len);

	if(!buf)
		return NULL;

	if(prepend)
		strcpy(buf, prepend);
	else
		buf[0] = '\0';

	strcat(buf, dirname);

	if(append)
		strcat(buf, append);

	return (buf);
}

static void * __do_open(const char * filename, const char * mode)
{
	FILE * f;

	f = fopen(filename, mode);
	if(!f)
		return NULL;

	return ((void *)f);
}

void * __xfs_platform_open_read(const char * filename)
{
	return __do_open(filename, "r");
}

void * __xfs_platform_open_write(const char * filename)
{
	return __do_open(filename, "w");
}

void * __xfs_platform_open_append(const char * filename)
{
	return __do_open(filename, "a");
}

s64_t __xfs_platform_read(void * handle, void * buf, u32_t size, u32_t count)
{
	FILE * f = handle;
	s64_t ret = 0;

	ret = fread(buf, size, count, f);
	return ret;
}

s64_t __xfs_platform_write(void * handle, const void * buf, u32_t size, u32_t count)
{
	FILE * f = handle;
	s64_t ret = 0;

	ret = fwrite(buf, size, count, f);
	return ret;
}

s64_t __xfs_platform_get_last_modtime(const char * filename)
{
	struct stat _st;

	if(stat(filename, &_st) != 0)
		return -1;
	return (s64_t)(_st.st_mtime);
}

s64_t __xfs_platform_tell(void * handle)
{
	FILE * f = handle;

	return ((s64_t)ftell(f));
}

s64_t __xfs_platform_length(void * handle)
{
	FILE * f = handle;
	fpos_t pos = ftell(f);
	s64_t len = 0;

	if(fseek(f, 0, SEEK_END) == 0)
	{
		len = ftell(f);
		fseek(f, pos, SEEK_SET);
	}

	return len;
}

bool_t __xfs_platform_seek(void * handle, u64_t pos)
{
	FILE * f = handle;

	if(fseek(f, pos, SEEK_SET) == 0)
		return TRUE;

	return FALSE;
}

bool_t __xfs_platform_eof(void * handle)
{
	FILE * f = handle;

	if(feof(f) != 0)
		return TRUE;
	return FALSE;
}

bool_t __xfs_platform_flush(void * handle)
{
	FILE * f = handle;

	if(fflush(f) == 0)
		return TRUE;
	return FALSE;
}

bool_t __xfs_platform_close(void * handle)
{
	FILE * f = handle;

	fclose(f);
	return TRUE;
}

bool_t __xfs_platform_exists(const char * filename)
{
    struct stat _st;

	if(stat(filename, &_st) != 0)
		return FALSE;
	return TRUE;
}

bool_t __xfs_platform_is_link(const char * filename)
{
	struct stat _st;

	if(stat(filename, &_st) != 0)
		return FALSE;

    if(S_ISLNK(_st.st_mode))
    	return TRUE;
    return FALSE;
}

bool_t __xfs_platform_is_directory(const char * filename)
{
	struct stat _st;

	if(stat(filename, &_st) != 0)
		return FALSE;

    if(S_ISDIR(_st.st_mode))
    	return TRUE;
    return FALSE;
}

bool_t __xfs_platform_mkdir(const char * path)
{
	if(mkdir(path, (S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)) == 0)
		return TRUE;
	return FALSE;
}

bool_t __xfs_platform_delete(const char * path)
{
	struct stat _st;
	int ret;

	if(stat(path, &_st) != 0)
		return FALSE;

    if(S_ISDIR(_st.st_mode))
        ret = rmdir(path);
    else
        ret = unlink(path);

    if(ret == 0)
    	return TRUE;
    return FALSE;
}

void __xfs_platform_enumerate(const char * path, xfs_enumerate_callback cb, const char * odir, void * cbdata)
{
	void * dir;
	struct dirent * entry;

	if( (dir = opendir(path)) == NULL )
    	return;

	while ((entry = readdir(dir)) != NULL)
	{
		if (strcmp(entry->d_name, ".") == 0)
			continue;

		if (strcmp(entry->d_name, "..") == 0)
			continue;

		cb(cbdata, odir, entry->d_name);
	}

	closedir(dir);
}
