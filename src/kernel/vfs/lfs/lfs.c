/*
 * kernel/vfs/lfs/lfs.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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

#include <xboot.h>
#include <lfs.h>
#include <vfs/vfs.h>

struct lfs_context_t {
	struct lfs_config cfg;
	struct block_t * blk;
	spinlock_t lock;
};

static int lfs_block_read(const struct lfs_config * cfg, lfs_block_t block, lfs_off_t off, void * buffer, lfs_size_t size)
{
	struct lfs_context_t * ctx = (struct lfs_context_t *)cfg->context;

	if(block_read(ctx->blk, (u8_t *)buffer, (u64_t)block * cfg->block_size + (u64_t)off, size) == size)
		return 0;
	return -1;
}

static int lfs_block_prog(const struct lfs_config * cfg, lfs_block_t block, lfs_off_t off, const void * buffer, lfs_size_t size)
{
	struct lfs_context_t * ctx = (struct lfs_context_t *)cfg->context;

	if(block_write(ctx->blk, (u8_t *)buffer, (u64_t)block * cfg->block_size + (u64_t)off, size) == size)
		return 0;
	return -1;
}

static int lfs_block_erase(const struct lfs_config * cfg, lfs_block_t block)
{
	return 0;
}

static int lfs_block_sync(const struct lfs_config * cfg)
{
	struct lfs_context_t * ctx = (struct lfs_context_t *)cfg->context;

	block_sync(ctx->blk);
	return 0;
}

static int lfs_block_lock(const struct lfs_config * cfg)
{
	struct lfs_context_t * ctx = (struct lfs_context_t *)cfg->context;
	spin_lock(&ctx->lock);
	return 0;
}

static int lfs_block_unlock(const struct lfs_config * cfg)
{
	struct lfs_context_t * ctx = (struct lfs_context_t *)cfg->context;
	spin_unlock(&ctx->lock);
	return 0;
}

static struct lfs_context_t * lfs_context_alloc(struct block_t * blk)
{
	struct lfs_context_t * ctx;

	if(!blk)
		return NULL;

	ctx = malloc(sizeof(struct lfs_context_t));
	if(!ctx)
		return NULL;

	ctx->cfg.context = (void *)ctx;
	ctx->cfg.read = lfs_block_read;
	ctx->cfg.prog = lfs_block_prog;
	ctx->cfg.erase = lfs_block_erase;
	ctx->cfg.sync = lfs_block_sync;
	ctx->cfg.lock = lfs_block_lock;
	ctx->cfg.unlock = lfs_block_unlock;
	ctx->cfg.read_size = 16;
	ctx->cfg.prog_size = 16;
	ctx->cfg.block_size = block_size(blk);
	ctx->cfg.block_count = block_count(blk);
	ctx->cfg.block_cycles = 500;
	ctx->cfg.cache_size = 16;
	ctx->cfg.lookahead_size = 16;
	ctx->cfg.read_buffer = NULL;
	ctx->cfg.prog_buffer = NULL;
	ctx->cfg.lookahead_buffer = NULL;
	ctx->cfg.name_max = 0;
	ctx->cfg.file_max = 0;
	ctx->cfg.attr_max = 0;
	ctx->cfg.metadata_max = 0;
	ctx->blk = blk;
	spin_lock_init(&ctx->lock);

	return ctx;
}

static void lfs_context_free(struct lfs_context_t * ctx)
{
	if(ctx)
		free(ctx);
}

int lfs_format_block(struct block_t * blk)
{
	struct lfs_context_t * ctx;
	lfs_t lfs;

	ctx = lfs_context_alloc(blk);
	if(ctx)
	{
		lfs_format(&lfs, &ctx->cfg);
		lfs_context_free(ctx);
		return 1;
	}
    return 0;
}

int lfs_test(int argc, char ** argv)
{
	struct lfs_context_t * ctx;
	lfs_t lfs;
	lfs_file_t file;

	ctx = lfs_context_alloc(search_block("blk-spinor.0.private"));
	if(ctx)
	{
		// mount the filesystem
		int err = lfs_mount(&lfs, &ctx->cfg);
		// reformat if we can't mount the filesystem
		// this should only happen on the first boot
		if (err) {
			lfs_format(&lfs, &ctx->cfg);
			lfs_mount(&lfs, &ctx->cfg);
		}
		// read current count
		uint32_t boot_count = 0;
		lfs_file_open(&lfs, &file, "boot_count", LFS_O_RDWR | LFS_O_CREAT);
		lfs_file_read(&lfs, &file, &boot_count, sizeof(boot_count));
		// update boot count
		boot_count += 1;
		lfs_file_rewind(&lfs, &file);
		lfs_file_write(&lfs, &file, &boot_count, sizeof(boot_count));
		// remember the storage is not updated until the file is closed successfully
		lfs_file_close(&lfs, &file);
		// release any resources we were using
		lfs_unmount(&lfs);
		// print the boot count
		printf("boot_count: %d\r\n", boot_count);
		lfs_context_free(ctx);
	}
	return 0;
}

struct littlefs_node_t {
	struct list_head entry;
	struct list_head children;
	enum vfs_node_type_t type;
	char * name;
	u32_t mode;
	char * buf;
	u64_t buflen;
	u64_t size;
};

static struct littlefs_node_t * littlefs_node_alloc(const char * name, enum vfs_node_type_t type)
{
	struct littlefs_node_t * rn;

	rn = malloc(sizeof(struct littlefs_node_t));
	if(!rn)
		return NULL;

	rn->name = strdup(name);
	if(!rn->name)
	{
		free(rn);
		return NULL;
	}
	init_list_head(&rn->entry);
	init_list_head(&rn->children);
	rn->type = type;
	rn->mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
	rn->buf = NULL;
	rn->buflen = 0;
	rn->size = 0;

	return rn;
}

static void littlefs_node_free(struct littlefs_node_t * rn)
{
	if(rn->name)
		free(rn->name);
	if(rn->buf)
	{
		free(rn->buf);
		rn->buf = NULL;
		rn->buflen = 0;
	}
	free(rn);
}

static struct littlefs_node_t * littlefs_node_add(struct littlefs_node_t * rn, const char * name, enum vfs_node_type_t type)
{
	struct littlefs_node_t * n;

	n = littlefs_node_alloc(name, type);
	if(!n)
		return NULL;
	list_add_tail(&n->entry, &rn->children);
	return n;
}

static int littlefs_node_remove(struct littlefs_node_t * drn, struct littlefs_node_t * rn)
{
	struct littlefs_node_t * pos, * n;

	list_for_each_entry_safe(pos, n, &(drn->children), entry)
	{
		if(pos == rn)
		{
			list_del(&pos->entry);
			littlefs_node_free(pos);
			return 0;
		}
	}
	return -1;
}

static int lfsfs_rename_node(struct littlefs_node_t * rn, const char * name)
{
	if(rn->name)
		free(rn->name);
	rn->name = strdup(name);
	if(!rn->name)
		return -1;
	return 0;
}

static int littlefs_mount(struct vfs_mount_t * m, const char * dev)
{
	struct littlefs_node_t * rn;

	if(dev)
		return -1;
	rn = littlefs_node_alloc("/", VNT_DIR);
	if(!rn)
		return -1;
	m->m_root->v_data = (void *)rn;
	m->m_data = NULL;
	return 0;
}

static int littlefs_unmount(struct vfs_mount_t * m)
{
	littlefs_node_free(m->m_root->v_data);
	m->m_data = NULL;
	return 0;
}

static int littlefs_msync(struct vfs_mount_t * m)
{
	return 0;
}

static int littlefs_vget(struct vfs_mount_t * m, struct vfs_node_t * n)
{
	return 0;
}

static int littlefs_vput(struct vfs_mount_t * m, struct vfs_node_t * n)
{
	return 0;
}

static u64_t littlefs_read(struct vfs_node_t * n, s64_t off, void * buf, u64_t len)
{
	struct littlefs_node_t * rn;
	u64_t sz;

	if(n->v_type != VNT_REG)
		return 0;

	if(off >= n->v_size)
		return 0;

	sz = len;
	if((n->v_size - off) < sz)
		sz = n->v_size - off;

	rn = n->v_data;
	memcpy(buf, rn->buf + off, sz);
	return sz;
}

static u64_t littlefs_write(struct vfs_node_t * n, s64_t off, void * buf, u64_t len)
{
	struct littlefs_node_t * rn;
	void * nbuf;
	u64_t nsize;
	u64_t epos;

	if(n->v_type != VNT_REG)
		return 0;

	rn = n->v_data;
	epos = n->v_size;

	if(off + len > epos)
	{
		epos = off + len;
		if(epos > rn->buflen)
		{
			nsize = (epos + 0xfff) & ~0xfff;
			nbuf = malloc(nsize);
			if(!nbuf)
				return -1;
			if(rn->size != 0)
			{
				memcpy(nbuf, rn->buf, n->v_size);
				free(rn->buf);
			}
			rn->buf = nbuf;
			rn->buflen = nsize;
		}
		rn->size = epos;
		n->v_size = epos;
	}
	memcpy(rn->buf + off, buf, len);

	return len;
}

static int littlefs_truncate(struct vfs_node_t * n, s64_t off)
{
	struct littlefs_node_t * rn;
	void * nbuf;
	u64_t nsize;

	rn = n->v_data;

	if(off == 0)
	{
		if(rn->buf != NULL)
		{
			free(rn->buf);
			rn->buf = NULL;
			rn->buflen = 0;
		}
	}
	else if(off > rn->buflen)
	{
		nsize = (off + 0xfff) & ~0xfff;
		nbuf = malloc(nsize);
		if(!nbuf)
			return -1;
		if(rn->size != 0)
		{
			memcpy(nbuf, rn->buf, n->v_size);
			free(rn->buf);
		}
		rn->buf = nbuf;
		rn->buflen = nsize;
	}
	rn->size = off;
	n->v_size = off;

	return 0;
}

static int littlefs_sync(struct vfs_node_t * n)
{
	return 0;
}

static int littlefs_readdir(struct vfs_node_t * dn, s64_t off, struct vfs_dirent_t * d)
{
	struct littlefs_node_t * pos, * n;
	struct littlefs_node_t * drn;
	s64_t i = 0;

	drn = dn->v_data;
	list_for_each_entry_safe(pos, n, &(drn->children), entry)
	{
		if(i++ == off)
		{
			if(pos->type == VNT_DIR)
				d->d_type = VDT_DIR;
			else
				d->d_type = VDT_REG;
			strlcpy(d->d_name, pos->name, sizeof(d->d_name));
			d->d_off = off;
			d->d_reclen = 1;
			return 0;
		}
	}
	return -1;
}

static int littlefs_lookup(struct vfs_node_t * dn, const char * name, struct vfs_node_t * n)
{
	struct littlefs_node_t * pos, * tn;
	struct littlefs_node_t * drn;

	if(!name || (*name == '\0'))
		return -1;

	drn = dn->v_data;
	list_for_each_entry_safe(pos, tn, &(drn->children), entry)
	{
		if(strcmp(name, pos->name) == 0)
		{
			n->v_atime = 0;
			n->v_mtime = 0;
			n->v_ctime = 0;
			n->v_mode = 0;
			if(pos->type == VNT_DIR)
				n->v_mode |= S_IFDIR;
			else
				n->v_mode |= S_IFREG;
			n->v_mode |= pos->mode & (S_IRWXU | S_IRWXG | S_IRWXO);
			n->v_type = pos->type;
			n->v_size = pos->size;
			n->v_data = (void *)pos;
			return 0;
		}
	}
	return -1;
}

static int littlefs_create(struct vfs_node_t * dn, const char * name, u32_t mode)
{
	struct littlefs_node_t * rn;

	if(!S_ISREG(mode))
		return -1;
	rn = littlefs_node_add(dn->v_data, name, VNT_REG);
	if(!rn)
		return -1;
	rn->mode = mode & (S_IRWXU | S_IRWXG | S_IRWXO);
	return 0;
}

static int littlefs_remove(struct vfs_node_t * dn, struct vfs_node_t * n, const char * name)
{
	return littlefs_node_remove(dn->v_data, n->v_data);
}

static int littlefs_rename(struct vfs_node_t * sn, const char * sname, struct vfs_node_t * n, struct vfs_node_t * dn, const char * dname)
{
	struct littlefs_node_t * rn, * orn;

	if(sn == dn)
	{
		if(lfsfs_rename_node(n->v_data, dname) < 0)
			return -1;
	}
	else
	{
		orn = n->v_data;
		rn = littlefs_node_add(dn->v_data, dname, VNT_REG);
		if(!rn)
			return -1;
		if(n->v_type == VNT_REG)
		{
			rn->buf = orn->buf;
			rn->buflen = orn->buflen;
			rn->size = orn->size;
			orn->buf = NULL;
			orn->buflen = 0;
			orn->size = 0;
		}
		littlefs_node_remove(sn->v_data, n->v_data);
	}
	return 0;
}

static int littlefs_mkdir(struct vfs_node_t * dn, const char * name, u32_t mode)
{
	struct littlefs_node_t * rn;

	if(!S_ISDIR(mode))
		return -1;
	rn = littlefs_node_add(dn->v_data, name, VNT_DIR);
	if(!rn)
		return -1;
	rn->mode = mode & (S_IRWXU | S_IRWXG | S_IRWXO);
	rn->size = 0;
	return 0;
}

static int littlefs_rmdir(struct vfs_node_t * dn, struct vfs_node_t * n, const char * name)
{
	return littlefs_node_remove(dn->v_data, n->v_data);
}

static int littlefs_chmod(struct vfs_node_t * n, u32_t mode)
{
	struct littlefs_node_t * rn;

	rn = n->v_data;
	rn->mode = mode & (S_IRWXU | S_IRWXG | S_IRWXO);
	return 0;
}

static struct filesystem_t lfs = {
	.name		= "lfs",

	.mount		= littlefs_mount,
	.unmount	= littlefs_unmount,
	.msync		= littlefs_msync,
	.vget		= littlefs_vget,
	.vput		= littlefs_vput,

	.read		= littlefs_read,
	.write		= littlefs_write,
	.truncate	= littlefs_truncate,
	.sync		= littlefs_sync,
	.readdir	= littlefs_readdir,
	.lookup		= littlefs_lookup,
	.create		= littlefs_create,
	.remove		= littlefs_remove,
	.rename		= littlefs_rename,
	.mkdir		= littlefs_mkdir,
	.rmdir		= littlefs_rmdir,
	.chmod		= littlefs_chmod,
};

static __init void filesystem_littlefs_init(void)
{
	register_filesystem(&lfs);
}

static __exit void filesystem_littlefs_exit(void)
{
	unregister_filesystem(&lfs);
}

core_initcall(filesystem_littlefs_init);
core_exitcall(filesystem_littlefs_exit);
