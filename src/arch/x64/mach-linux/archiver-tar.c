/*
 * archiver-tar.c
 *
 * Copyright(c) 2007-2020 Jianjun Jiang <8192542@qq.com>
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

#include <sandbox.h>
#include <xfs/archiver.h>

enum {
	FILE_TYPE_NORMAL		= '0',
	FILE_TYPE_HARD_LINK		= '1',
	FILE_TYPE_SYMBOLIC_LINK = '2',
	FILE_TYPE_CHAR_DEVICE	= '3',
	FILE_TYPE_BLOCK_DEVICE	= '4',
	FILE_TYPE_DIRECTORY		= '5',
	FILE_TYPE_FIFO			= '6',
	FILE_TYPE_CONTIGOUS		= '7',
};

struct tar_header_t
{
	/* File name */
	int8_t name[100];

	/* File mode */
	int8_t mode[8];

	/* User id */
	int8_t uid[8];

	/* Group id */
	int8_t gid[8];

	/* File size in bytes */
	int8_t size[12];

	/* Last modification time */
	int8_t mtime[12];

	/* Checksum for header block */
	int8_t chksum[8];

	/* File type */
	int8_t filetype;

	/* Link filename */
	int8_t linkname[100];

	/* Magic indicator "ustar" */
	int8_t magic[6];

	/* Version */
	int8_t version[2];

	/* User name */
	int8_t uname[32];

	/* Group name */
	int8_t gname[32];

	/* Device major number */
	int8_t devmajor[8];

	/* Device minor number */
	int8_t devminor[8];

	/* Filename prefix */
	int8_t prefix[155];

	/* Reserver */
	int8_t reserver[12];
} __attribute__ ((packed));

struct mhandle_tar_t {
	struct list_head list;
	struct hlist_head * hash;
	int hsize;
	int fd;
};

struct fhandle_tar_t
{
	struct list_head head;
	struct hlist_node node;
	char * name;
	int64_t start;
	int64_t size;
	int64_t offset;
	int isdir;
	int fd;
};

static struct hlist_head * fhandle_hash(struct mhandle_tar_t * m, const char * name)
{
	return &m->hash[shash(name) % m->hsize];
}

static struct fhandle_tar_t * search_fhandle(struct mhandle_tar_t * m, const char * name)
{
	struct fhandle_tar_t * pos;
	struct hlist_node * n;

	if(!name)
		return NULL;

	hlist_for_each_entry_safe(pos, n, fhandle_hash(m, name), node)
	{
		if((strcmp(pos->name, name) == 0))
			return pos;
	}
	return NULL;
}

static struct mhandle_tar_t * alloc_mhandle(int fd)
{
	struct mhandle_tar_t * m;
	struct fhandle_tar_t * f;
	struct tar_header_t header;
	int64_t off;
	int64_t size;
	int hsize = 0;
	int i, l;
	char * p;

	off = 0;
	while(1)
	{
		sandbox_file_seek(fd, off);
		if(sandbox_file_read(fd, &header, sizeof(struct tar_header_t)) != sizeof(struct tar_header_t))
			break;
		if(strncmp((const char *)(header.magic), "ustar", 5) != 0)
			break;

		size = strtoll((const char *)(header.size), NULL, 0);
		if(size < 0)
			break;

		if((header.filetype == FILE_TYPE_NORMAL) || (header.filetype == FILE_TYPE_DIRECTORY))
			hsize++;

		if(size == 0)
			off += sizeof(struct tar_header_t);
		else
			off += sizeof(struct tar_header_t) + (((size + 512) >> 9) << 9);
	}
	if(hsize == 0)
		return NULL;

	m = malloc(sizeof(struct mhandle_tar_t));
	if(!m)
		return NULL;

	m->hsize = hsize * 2;
	m->fd = fd;
	m->hash = malloc(sizeof(struct hlist_head) * m->hsize);
	if(!m->hash)
	{
		free(m);
		return NULL;
	}
	init_list_head(&m->list);
	for(i = 0; i < m->hsize; i++)
		init_hlist_head(&m->hash[i]);

	off = 0;
	while(1)
	{
		sandbox_file_seek(fd, off);
		if(sandbox_file_read(fd, &header, sizeof(struct tar_header_t)) != sizeof(struct tar_header_t))
			break;
		if(strncmp((const char *)(header.magic), "ustar", 5) != 0)
			break;

		size = strtoll((const char *)(header.size), NULL, 0);
		if(size < 0)
			break;

		if((header.filetype == FILE_TYPE_NORMAL) || (header.filetype == FILE_TYPE_DIRECTORY))
		{
			f = malloc(sizeof(struct fhandle_tar_t));
			if(!f)
				break;

			p = (char *)header.name;
			l = strlen(p);
			if(l > 0 && p[l - 1] == '/')
				p[l - 1] = '\0';

			f->name = strdup(p);
			f->start = off + sizeof(struct tar_header_t);
			f->size = size;
			f->offset = 0;
			f->isdir = (header.filetype == FILE_TYPE_DIRECTORY) ? TRUE : FALSE;
			f->fd = fd;
			init_list_head(&f->head);
			list_add_tail(&f->head, &m->list);
			init_hlist_node(&f->node);
			hlist_add_head(&f->node, fhandle_hash(m, f->name));
		}

		if(size == 0)
			off += sizeof(struct tar_header_t);
		else
			off += sizeof(struct tar_header_t) + (((size + 512) >> 9) << 9);
	}

	return m;
}

static void free_mhandle(struct mhandle_tar_t * m)
{
	struct fhandle_tar_t * pos, * n;

	if(m)
	{
		list_for_each_entry_safe(pos, n, &m->list, head)
		{
			list_del(&pos->head);
			hlist_del(&pos->node);
			free(pos->name);
			free(pos);
		}
		free(m->hash);
		free(m);
	}
}

static void * tar_mount(const char * path, int * writable)
{
	struct mhandle_tar_t * m;
	struct tar_header_t header;
	int fd;

	if(!sandbox_file_isfile(path))
		return NULL;

	fd = sandbox_file_open(path, "r");
	if(fd < 0)
		return NULL;

	if((sandbox_file_read(fd, &header, sizeof(struct tar_header_t)) != sizeof(struct tar_header_t)) || (strncmp((const char *)(header.magic), "ustar", 5) != 0))
	{
		sandbox_file_close(fd);
		return NULL;
	}

	m = alloc_mhandle(fd);
	if(!m)
	{
		sandbox_file_close(fd);
		return NULL;
	}

	if(writable)
		*writable = 0;
	return m;
}

static void tar_umount(void * m)
{
	struct mhandle_tar_t * mh = (struct mhandle_tar_t *)m;

	if(mh)
	{
		sandbox_file_close(mh->fd);
		free_mhandle(mh);
	}
}

static void tar_walk(void * m, const char * name, xfs_walk_callback_t cb, void * data)
{
	struct mhandle_tar_t * mh = (struct mhandle_tar_t *)m;
	struct fhandle_tar_t * fh = search_fhandle(mh, name);
	struct fhandle_tar_t * pos, * n;
	char * p;
	int l = strlen(name);

	if((l == 0) && name)
	{
		list_for_each_entry_safe(pos, n, &mh->list, head)
		{
			if(strncmp(name, pos->name, l) == 0)
			{
				p = &pos->name[l];
				if(p && !strchr(p, '/'))
					cb(name, p, data);
			}
		}
	}
	else if(fh && fh->isdir)
	{
		list_for_each_entry_safe(pos, n, &mh->list, head)
		{
			if(strncmp(name, pos->name, l) == 0)
			{
				p = &pos->name[l];
				if(*p++ == '/')
				{
					if(p && !strchr(p, '/'))
						cb(name, p, data);
				}
			}
		}
	}
}

static bool_t tar_isdir(void * m, const char * name)
{
	struct mhandle_tar_t * mh = (struct mhandle_tar_t *)m;
	struct fhandle_tar_t * fh = search_fhandle(mh, name);
	return (fh && fh->isdir) ? TRUE : FALSE;
}

static bool_t tar_isfile(void * m, const char * name)
{
	struct mhandle_tar_t * mh = (struct mhandle_tar_t *)m;
	struct fhandle_tar_t * fh = search_fhandle(mh, name);
	return (fh && !fh->isdir) ? TRUE : FALSE;
}

static bool_t tar_mkdir(void * m, const char * name)
{
	return FALSE;
}

static bool_t tar_remove(void * m, const char * name)
{
	return FALSE;
}

static void * tar_open(void * m, const char * name, int mode)
{
	struct mhandle_tar_t * mh = (struct mhandle_tar_t *)m;
	struct fhandle_tar_t * fh;

	if(mode != XFS_OPEN_MODE_READ)
		return NULL;
	fh = search_fhandle(mh, name);
	if(!fh || fh->isdir)
		return NULL;
	fh->offset = 0;
	return ((void *)fh);
}

static s64_t tar_read(void * f, void * buf, s64_t size)
{
	struct fhandle_tar_t * fh = (struct fhandle_tar_t *)f;
	s64_t len;
	if(size > fh->size - fh->offset)
		size = fh->size - fh->offset;
	sandbox_file_seek(fh->fd, fh->start + fh->offset);
	len = sandbox_file_read(fh->fd, buf, size);
	fh->offset += len;
	return len;
}

static s64_t tar_write(void * f, void * buf, s64_t size)
{
	return 0;
}

static s64_t tar_seek(void * f, s64_t offset)
{
	struct fhandle_tar_t * fh = (struct fhandle_tar_t *)f;
	if(offset < 0)
		fh->offset = 0;
	else if(offset > fh->size)
		fh->offset = fh->size;
	else
		fh->offset = offset;
	sandbox_file_seek(fh->fd, fh->start + fh->offset);
	return fh->offset;
}

static s64_t tar_tell(void * f)
{
	struct fhandle_tar_t * fh = (struct fhandle_tar_t *)f;
	return fh->offset;
}

static s64_t tar_length(void * f)
{
	struct fhandle_tar_t * fh = (struct fhandle_tar_t *)f;
	return fh->size;
}

static void tar_close(void * f)
{
	struct fhandle_tar_t * fh = (struct fhandle_tar_t *)f;
	fh->offset = 0;
}

static struct xfs_archiver_t archiver_tar = {
	.name		= "tar",
	.mount		= tar_mount,
	.umount 	= tar_umount,
	.walk		= tar_walk,
	.isdir		= tar_isdir,
	.isfile		= tar_isfile,
	.mkdir		= tar_mkdir,
	.remove		= tar_remove,
	.open		= tar_open,
	.read		= tar_read,
	.write		= tar_write,
	.seek		= tar_seek,
	.tell		= tar_tell,
	.length		= tar_length,
	.close		= tar_close,
};

static __init void archiver_tar_init(void)
{
	register_archiver(&archiver_tar);
}

static __exit void archiver_tar_exit(void)
{
	unregister_archiver(&archiver_tar);
}

core_initcall(archiver_tar_init);
core_exitcall(archiver_tar_exit);
