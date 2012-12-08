/*
 * framework/xfs/platform.c
 */

#include <runtime.h>
#include <framework/xfs/platform.h>

static void __bubble_sort(void * a, size_t lo, size_t hi, int (*cmp)(void *, size_t, size_t), void (*swap)(void *, size_t, size_t))
{
	size_t i;
	int sorted;

	do
	{
		sorted = 1;
		for(i = lo; i < hi; i++)
		{
			if(cmp(a, i, i + 1) > 0)
			{
				swap(a, i, i + 1);
				sorted = 0;
			}
		}
	} while(!sorted);
}

static void __quick_sort(void * a, size_t lo, size_t hi, int (*cmp)(void *, size_t, size_t), void (*swap)(void *, size_t, size_t))
{
	size_t i;
	size_t j;
	size_t v;

	if((hi - lo) <= 4)
		__bubble_sort(a, lo, hi, cmp, swap);
	else
	{
		i = (hi + lo) / 2;

		if(cmp(a, lo, i) > 0)
			swap(a, lo, i);
		if(cmp(a, lo, hi) > 0)
			swap(a, lo, hi);
		if(cmp(a, i, hi) > 0)
			swap(a, i, hi);

		j = hi - 1;
		swap(a, i, j);
		i = lo;
		v = j;

		while(1)
		{
			while(cmp(a, ++i, v) < 0) { }
			while(cmp(a, --j, v) > 0) { }
			if(j < i)
				break;
			swap(a, i, j);
		}

		if(i != (hi - 1))
			swap(a, i, hi - 1);

		__quick_sort(a, lo, j, cmp, swap);
		__quick_sort(a, i + 1, hi, cmp, swap);
	}
}

void __xfs_platform_sort(void * a, size_t max, int (*cmp)(void *, size_t, size_t), void (*swap)(void *, size_t, size_t))
{
	if(max > 0)
		__quick_sort(a, 0, max - 1, cmp, swap);
}

inline struct xfs_context_t * __xfs_platform_get_context(void)
{
	return (runtime_get()->__xfs_ctx);
}

inline void __xfs_platform_lock(void)
{
	void * lock = __xfs_platform_get_context()->lock;
	lock = (void *)lock;
}

inline void __xfs_platform_unlock(void)
{
	void * lock = __xfs_platform_get_context()->lock;
	lock = (void *)lock;
}

inline const char * __xfs_platform_directory_separator(void)
{
	return ("/");
}

char * __xfs_platform_absolute_path(const char * path)
{
	char buf[MAX_PATH];
	char * ret;

	if(vfs_path_conv(path, buf) == 0)
		ret = strdup(buf);
	else
		ret = strdup("/");

	return ret;
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

		else if (strcmp(entry->d_name, "..") == 0)
			continue;

		cb(cbdata, odir, entry->d_name);
	}

	closedir(dir);
}
