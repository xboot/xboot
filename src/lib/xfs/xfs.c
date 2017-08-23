/*
 * xfs/xfs.c
 */

#include <xfs/xfs.h>

static char * normal_path(const char * path)
{
	char * p, * q, * buf;
	char c;

	if(!path)
		return NULL;
	while(*path == '/')
		path++;
	p = q = buf = malloc(strlen(path) + 1);

	do
	{
		c = *(path++);
		if((c == ':') || (c == '\\'))
		{
			free(buf);
			return NULL;
		}
		if(c == '/')
		{
			*q = '\0';
			if((strcmp(p, ".") == 0) || (strcmp(p, "..") == 0))
			{
				free(buf);
				return NULL;
			}
			while(*path == '/')
				path++;
			if(*path == '\0')
				break;
			p = q + 1;
		}
		*(q++) = c;
	} while(c != '\0');

	return buf;
}

bool_t xfs_mount(struct xfs_context_t * ctx, const char * path)
{
	struct xfs_path_t * pos, * n;
	struct xfs_path_t * p;
	irq_flags_t flags;

	if(!ctx || !path)
		return FALSE;

	list_for_each_entry_safe(pos, n, &ctx->mounts.list, list)
	{
		if(strcmp(pos->path, path) == 0)
			return FALSE;
	}

	p = malloc(sizeof(struct xfs_path_t));
	if(!p)
		return FALSE;

	p->mhandle = mount_archiver(path, &p->archiver);
	if(!p->mhandle)
	{
		free(p);
		return FALSE;
	}
	p->path = strdup(path);

	spin_lock_irqsave(&ctx->lock, flags);
	init_list_head(&p->list);
	list_add_tail(&p->list, &ctx->mounts.list);
	spin_unlock_irqrestore(&ctx->lock, flags);

	return TRUE;
}

bool_t xfs_umount(struct xfs_context_t * ctx, const char * path)
{
	struct xfs_path_t * pos, * n;
	irq_flags_t flags;

	if(!ctx || !path)
		return FALSE;

	list_for_each_entry_safe(pos, n, &ctx->mounts.list, list)
	{
		if(strcmp(pos->path, path) == 0)
		{
			spin_lock_irqsave(&ctx->lock, flags);
			list_del(&pos->list);
			spin_unlock_irqrestore(&ctx->lock, flags);

			pos->archiver->umount(pos->mhandle);
			free(pos->path);
			free(pos);
			return TRUE;
		}
	}

	return FALSE;
}

void xfs_walk(struct xfs_context_t * ctx, const char * name, xfs_walk_callback_t cb, void * data)
{
	struct xfs_path_t * pos, * n;
	char * path;

	path = normal_path(name);
	if(!path)
		return;

	list_for_each_entry_safe_reverse(pos, n, &ctx->mounts.list, list)
	{
		pos->archiver->walk(pos->mhandle, path, cb, data);
	}
	free(path);
}

bool_t xfs_exist(struct xfs_context_t * ctx, const char * name)
{
	struct xfs_path_t * pos, * n;
	char * path;

	path = normal_path(name);
	if(!path)
		return FALSE;

	list_for_each_entry_safe_reverse(pos, n, &ctx->mounts.list, list)
	{
		if(pos->archiver->exist(pos->mhandle, path))
			return TRUE;
	}

	free(path);
	return FALSE;
}

bool_t xfs_isdir(struct xfs_context_t * ctx, const char * name)
{
	struct xfs_path_t * pos, * n;
	char * path;

	path = normal_path(name);
	if(!path)
		return FALSE;

	list_for_each_entry_safe_reverse(pos, n, &ctx->mounts.list, list)
	{
		if(pos->archiver->isdir(pos->mhandle, path))
			return TRUE;
	}

	free(path);
	return FALSE;
}

bool_t xfs_mkdir(struct xfs_context_t * ctx, const char * name)
{
	return FALSE;
}

bool_t xfs_remove(struct xfs_context_t * ctx, const char * name)
{
	return FALSE;
}

struct xfs_file_t * xfs_open_read(struct xfs_context_t * ctx, const char * name)
{
	struct xfs_path_t * pos, * n;
	struct xfs_file_t * file = NULL;
	char * path;
	void * f;

	path = normal_path(name);
	if(!path)
		return NULL;

	list_for_each_entry_safe_reverse(pos, n, &ctx->mounts.list, list)
	{
		f = pos->archiver->open(pos->mhandle, name, XFS_OPEN_MODE_READ);
		if(f)
		{
			file = malloc(sizeof(struct xfs_file_t));
			file->ctx = ctx;
			file->path = pos;
			file->fhandle = f;
			break;
		}
	}

	free(path);
	return file;
}

struct xfs_file_t * xfs_open_write(struct xfs_context_t * ctx, const char * name)
{
	return NULL;
}

struct xfs_file_t * xfs_open_append(struct xfs_context_t * ctx, const char * name)
{
	return NULL;
}

s64_t xfs_read(struct xfs_file_t * file, void * buf, s64_t size)
{
	if(file)
		return file->path->archiver->read(file->fhandle, buf, size);
	return 0;
}

s64_t xfs_write(struct xfs_file_t * file, void * buf, s64_t size)
{
	if(file)
		return file->path->archiver->write(file->fhandle, buf, size);
	return 0;
}

s64_t xfs_seek(struct xfs_file_t * file, s64_t offset)
{
	if(file)
		return file->path->archiver->seek(file->fhandle, offset);
	return FALSE;
}

s64_t xfs_length(struct xfs_file_t * file)
{
	if(file)
		return file->path->archiver->length(file->fhandle);
	return 0;
}

void xfs_close(struct xfs_file_t * file)
{
	if(file)
	{
		file->path->archiver->close(file->fhandle);
		free(file);
	}
}

struct xfs_context_t * __xfs_alloc(void)
{
	struct xfs_context_t * ctx;

	ctx = malloc(sizeof(struct xfs_context_t));
	if(!ctx)
		return NULL;
	memset(ctx, 0, sizeof(struct xfs_context_t));

	init_list_head(&ctx->mounts.list);
	spin_lock_init(&ctx->lock);
	return ctx;
}

void __xfs_free(struct xfs_context_t * ctx)
{
	struct xfs_context_t * pctx = (struct xfs_context_t *)ctx;

	if(pctx)
	{
		free(pctx);
	}
}

static char * __xfs_platform_absolute_path(const char * path)
{
	char buf[MAX_PATH];
	char * ret;

	if(vfs_path_conv(path, buf) == 0)
		ret = strdup(buf);
	else
		ret = strdup("/");
	return ret;
}

void __xfs_init(struct xfs_context_t * ctx, const char * path)
{
	char * p;

	if(path)
	{
		p = __xfs_platform_absolute_path(path);
		xfs_mount(ctx, "/romdisk/framework");
		xfs_mount(ctx, p);
		free(p);
	}
}
