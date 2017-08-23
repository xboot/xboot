/*
 * xfs/archiver-dir.c
 */

#include <xfs/archiver.h>

struct mhandle_dir_t {
	char * path;
};

struct fhandle_dir_t {
	int fd;
};

static char * concat(const char * str, ...)
{
	va_list args;
	const char *s;
	int len = strlen(str);
	va_start(args, str);
	while((s = va_arg(args, char *)))
	{
		len += strlen(s);
	}
	va_end(args);
	char * res = malloc(len + 1);
	if(!res)
		return NULL;
	strcpy(res, str);
	va_start(args, str);
	while((s = va_arg(args, char *)))
	{
		strcat(res, s);
	}
	va_end(args);
	return res;
}

static int is_file_exist(const char * path)
{
	if(!path)
		return 0;
//	if(access(path, F_OK) == 0)
//		return 1;
	return 1;
}

static int is_dir_exist(const char * path)
{
	void * d;

	if(!path)
		return 0;
	if(!(d = opendir(path)))
		return 0;
	closedir(d);
	return 1;
}

static void * dir_mount(const char * path)
{
	struct mhandle_dir_t * m;

	if(!is_dir_exist(path))
		return NULL;
	m = malloc(sizeof(struct mhandle_dir_t));
	if(!m)
		return NULL;
	m->path = strdup(path);
	return m;
}

static void dir_umount(void * m)
{
	struct mhandle_dir_t * mh = (struct mhandle_dir_t *)m;

	if(mh)
	{
		free(mh->path);
		free(mh);
	}
}

static void dir_walk(void * m, const char * name, xfs_walk_callback_t cb, void * data)
{
	struct mhandle_dir_t * mh = (struct mhandle_dir_t *)m;
	char * path = concat(mh->path, "/", name, NULL);
	struct dirent_t * entry;
	void * dir;

	if((dir = opendir(path)) == NULL)
		return;
	while((entry = readdir(dir)) != NULL)
	{
		if(strcmp(entry->d_name, ".") == 0)
			continue;
		else if(strcmp(entry->d_name, "..") == 0)
			continue;
		cb(name, entry->d_name, data);
	}
	closedir(dir);
	free(path);
}

static bool_t dir_exist(void * m, const char * name)
{
	struct mhandle_dir_t * mh = (struct mhandle_dir_t *)m;
	char * path = concat(mh->path, "/", name, NULL);
	return is_file_exist(path);
}

static bool_t dir_isdir(void * m, const char * name)
{
	struct mhandle_dir_t * mh = (struct mhandle_dir_t *)m;
	char * path = concat(mh->path, "/", name, NULL);
	return is_dir_exist(path);
}

static bool_t dir_mkdir(void * m, const char * name)
{
	return FALSE;
}

static bool_t dir_remove(void * m, const char * name)
{
	return FALSE;
}

static void * dir_open(void * m, const char * name, int mode)
{
	struct mhandle_dir_t * mh = (struct mhandle_dir_t *)m;
	char * path = concat(mh->path, "/", name, NULL);
	FILE * fh = NULL;

	fh = fopen(path, "r");
	free(path);
	return ((void *)fh);
}

static s64_t dir_read(void * f, void * buf, s64_t size)
{
	FILE * fh = (FILE *)f;
	return fread(buf, size, 1, fh);
}

static s64_t dir_write(void * f, void * buf, s64_t size)
{
	FILE * fh = (FILE *)f;
	return fwrite(buf, size, 1, fh);
}

static s64_t dir_seek(void * f, s64_t offset)
{
	FILE * fh = (FILE *)f;

	fseek(fh, offset, SEEK_SET);
	return ((s64_t)ftell(fh));
}

static s64_t dir_length(void * f)
{
	return 0;
}

static void dir_close(void * f)
{
	FILE * fh = (FILE *)f;

	if(fh)
		fclose(fh);
}

struct xfs_archiver_t archiver_dir = {
	.name		= "",
	.mount		= dir_mount,
	.umount 	= dir_umount,
	.walk		= dir_walk,
	.exist		= dir_exist,
	.isdir		= dir_isdir,
	.mkdir		= dir_mkdir,
	.remove		= dir_remove,
	.open		= dir_open,
	.read		= dir_read,
	.write		= dir_write,
	.seek		= dir_seek,
	.length		= dir_length,
	.close		= dir_close,
};

static __init void archiver_dir_init(void)
{
	register_archiver(&archiver_dir);
}

static __exit void archiver_dir_exit(void)
{
	unregister_archiver(&archiver_dir);
}

core_initcall(archiver_dir_init);
core_exitcall(archiver_dir_exit);
