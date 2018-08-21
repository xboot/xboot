/*
 * kernel/xfs/xfs.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <sha1.h>
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

bool_t xfs_mount(struct xfs_context_t * ctx, const char * path, int writable)
{
	struct xfs_path_t * pos, * n;
	struct xfs_path_t * p;
	irq_flags_t flags;
	int w;

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

	p->mhandle = mount_archiver(path, &p->archiver, &w);
	if(!p->mhandle)
	{
		free(p);
		return FALSE;
	}
	p->path = strdup(path);
	p->writable = (writable && w) ? 1 : 0;

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

bool_t xfs_isfile(struct xfs_context_t * ctx, const char * name)
{
	struct xfs_path_t * pos, * n;
	char * path;

	path = normal_path(name);
	if(!path)
		return FALSE;

	list_for_each_entry_safe_reverse(pos, n, &ctx->mounts.list, list)
	{
		if(pos->archiver->isfile(pos->mhandle, path))
			return TRUE;
	}
	free(path);
	return FALSE;
}

bool_t xfs_mkdir(struct xfs_context_t * ctx, const char * name)
{
	struct xfs_path_t * pos, * n;
	char * path;
	int ret = FALSE;

	path = normal_path(name);
	if(!path)
		return FALSE;

	list_for_each_entry_safe_reverse(pos, n, &ctx->mounts.list, list)
	{
		if(pos->writable)
		{
			ret = pos->archiver->mkdir(pos->mhandle, path);
			break;
		}
	}
	free(path);
	return ret;
}

bool_t xfs_remove(struct xfs_context_t * ctx, const char * name)
{
	struct xfs_path_t * pos, * n;
	char * path;
	int ret = FALSE;

	path = normal_path(name);
	if(!path)
		return FALSE;

	list_for_each_entry_safe_reverse(pos, n, &ctx->mounts.list, list)
	{
		if(pos->writable)
		{
			ret = pos->archiver->remove(pos->mhandle, path);
			break;
		}
	}
	free(path);
	return ret;
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
		f = pos->archiver->open(pos->mhandle, path, XFS_OPEN_MODE_READ);
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
	struct xfs_path_t * pos, * n;
	struct xfs_file_t * file = NULL;
	char * path;
	void * f;

	path = normal_path(name);
	if(!path)
		return NULL;

	list_for_each_entry_safe_reverse(pos, n, &ctx->mounts.list, list)
	{
		if(pos->writable)
		{
			f = pos->archiver->open(pos->mhandle, path, XFS_OPEN_MODE_WRITE);
			if(f)
			{
				file = malloc(sizeof(struct xfs_file_t));
				file->ctx = ctx;
				file->path = pos;
				file->fhandle = f;
				break;
			}
		}
	}
	free(path);
	return file;
}

struct xfs_file_t * xfs_open_append(struct xfs_context_t * ctx, const char * name)
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
		if(pos->writable)
		{
			f = pos->archiver->open(pos->mhandle, path, XFS_OPEN_MODE_APPEND);
			if(f)
			{
				file = malloc(sizeof(struct xfs_file_t));
				file->ctx = ctx;
				file->path = pos;
				file->fhandle = f;
				break;
			}
		}
	}
	free(path);
	return file;
}

s64_t xfs_read(struct xfs_file_t * file, void * buf, s64_t size)
{
	if(file)
		return file->path->archiver->read(file->fhandle, buf, size);
	return 0;
}

s64_t xfs_write(struct xfs_file_t * file, void * buf, s64_t size)
{
	if(file && file->path->writable)
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

struct xfs_context_t * __xfs_alloc(const char * path)
{
	struct xfs_context_t * ctx;
	struct stat st;
	char fpath[MAX_PATH];
	char userdata[256];
	uint8_t digest[20];

	ctx = malloc(sizeof(struct xfs_context_t));
	if(!ctx)
		return NULL;
	memset(ctx, 0, sizeof(struct xfs_context_t));
	init_list_head(&ctx->mounts.list);
	spin_lock_init(&ctx->lock);

	if(path && vfs_path_conv(path, fpath) >= 0)
	{
		xfs_mount(ctx, "/framework", 0);
		xfs_mount(ctx, fpath, 0);
		sha1_hash(fpath, strlen(fpath), digest);
		sprintf(userdata, "/private/userdata/%s-%02x%02x%02x%02x%02x%02x%02x%02x", basename(fpath),
			digest[0], digest[1], digest[2], digest[3], digest[4], digest[5], digest[6], digest[7]);
		if(stat(userdata, &st) != 0)
			mkdir(userdata, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
		xfs_mount(ctx, userdata, 1);
	}
	return ctx;
}

void __xfs_free(struct xfs_context_t * ctx)
{
	struct xfs_path_t * pos, * n;
	irq_flags_t flags;

	if(!ctx)
		return;

	list_for_each_entry_safe(pos, n, &ctx->mounts.list, list)
	{
		spin_lock_irqsave(&ctx->lock, flags);
		list_del(&pos->list);
		spin_unlock_irqrestore(&ctx->lock, flags);

		pos->archiver->umount(pos->mhandle);
		free(pos->path);
		free(pos);
	}
	free(ctx);
}
