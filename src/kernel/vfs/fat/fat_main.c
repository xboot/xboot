/*
 * kernel/vfs/ram/ram.c
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

#include "fat_control.h"
#include "fat_node.h"

/* 
 * Mount point operations 
 */
static int fatfs_mount(struct vfs_mount_t * m, const char *dev, u32 flags)
{
	int rc;
	struct fatfs_control *ctrl;
	struct fatfs_node *root;

	ctrl = calloc(1, sizeof(struct fatfs_control));
	if (!ctrl) {
		return -1;
	}

	/* Setup control info */
	rc = fatfs_control_init(ctrl, m->m_dev);
	if (rc) {
		goto fail;
	}

	/* Get the root fatfs node */
	root = m->m_root->v_data;
	rc = fatfs_node_init(ctrl, root);
	if (rc) {
		goto fail;
	}

	/* Handcraft the root fatfs node */
	root->parent = NULL;
	root->parent_dent_off = 0;
	root->parent_dent_len = sizeof(struct fat_dirent);
	memset(&root->parent_dent, 0, sizeof(struct fat_dirent));
	root->parent_dent.file_attributes = FAT_DIRENT_SUBDIR;
	if (ctrl->type == FAT_TYPE_32) {
		root->first_cluster = ctrl->first_root_cluster;
		root->parent_dent.first_cluster_hi = 
				__le16((root->first_cluster >> 16) & 0xFFFF);
		root->parent_dent.first_cluster_lo = 
					__le16(root->first_cluster & 0xFFFF);
		root->parent_dent.file_size = 0x0;
	} else {
		root->first_cluster = 0x0;
		root->parent_dent.first_cluster_hi = 0x0;
		root->parent_dent.file_size = 0x0;
	}
	root->parent_dent_dirty = FALSE;

	/* Handcraft the root vfs node */
	m->m_root->v_type = VNT_DIR;
	m->m_root->v_mode =  S_IFDIR | S_IRWXU | S_IRWXG | S_IRWXO;
	m->m_root->v_ctime = 0;
	m->m_root->v_atime = 0;
	m->m_root->v_mtime = 0;
	m->m_root->v_size = fatfs_node_get_size(root);

	/* Save control as mount point data */
	m->m_data = ctrl;

	return 0;

fail:
	free(ctrl);
	return rc;
}

static int fatfs_unmount(struct vfs_mount_t * m)
{
	int rc;
	struct fatfs_control *ctrl = m->m_data;

	if (!ctrl) {
		return -1;
	}

	rc = fatfs_control_exit(ctrl);

	free(ctrl);

	return rc;
}

static int fatfs_msync(struct vfs_mount_t * m)
{
	struct fatfs_control *ctrl = m->m_data;

	if (!ctrl) {
		return -1;
	}

	return fatfs_control_sync(ctrl);
}

static int fatfs_vget(struct vfs_mount_t * m, struct vfs_node_t * v)
{
	int rc;
	struct fatfs_node *node;
	struct fatfs_control *ctrl = m->m_data;

	node = calloc(1, sizeof(struct fatfs_node));
	if (!node) {
		return -1;
	}

	rc = fatfs_node_init(ctrl, node);

	v->v_data = node;

	return rc;
}

static int fatfs_vput(struct vfs_mount_t * m, struct vfs_node_t * v)
{
	int rc;
	struct fatfs_node *node = v->v_data;

	if (!node) {
		return -1;
	}

	rc = fatfs_node_exit(node);

	free(node);

	return rc;
}

/* 
 * Vnode operations 
 */
static u64_t fatfs_read(struct vfs_node_t * v, s64_t off, void *buf, u64_t len)
{
	struct fatfs_node *node = v->v_data;
	u32 filesize = fatfs_node_get_size(node);

	if (filesize <= (u32)off) {
		return 0;
	}

	if (filesize < (u32)(len + off)) {
		len = filesize - off;
	}

	return fatfs_node_read(node, (u32)off, len, buf);
}

static u64_t fatfs_write(struct vfs_node_t * v, s64_t off, void *buf, u64_t len)
{
	u32 wlen;
	struct fatfs_node *node = v->v_data;

	wlen = fatfs_node_write(node, (u32)off, len, buf);

	/* Size and mtime might have changed */
	v->v_size = fatfs_node_get_size(node);
	v->v_mtime = fatfs_pack_timestamp(node->parent_dent.lmodify_date_year,
					  node->parent_dent.lmodify_date_month,
					  node->parent_dent.lmodify_date_day,
					  node->parent_dent.lmodify_time_hours,
					  node->parent_dent.lmodify_time_minutes,
					  node->parent_dent.lmodify_time_seconds);

	return wlen;
}

static int fatfs_truncate(struct vfs_node_t * v, s64_t off)
{
	int rc;
	struct fatfs_node *node = v->v_data;

	if ((u32)off <= fatfs_node_get_size(node)) {
		return -1;
	}

	rc = fatfs_node_truncate(node, (u32)off);
	if (rc) {
		return rc;
	}

	/* Size and mtime might have changed */
	v->v_size = fatfs_node_get_size(node);
	v->v_mtime = fatfs_pack_timestamp(node->parent_dent.lmodify_date_year,
					  node->parent_dent.lmodify_date_month,
					  node->parent_dent.lmodify_date_day,
					  node->parent_dent.lmodify_time_hours,
					  node->parent_dent.lmodify_time_minutes,
					  node->parent_dent.lmodify_time_seconds);

	return 0;
}

static int fatfs_sync(struct vfs_node_t * v)
{
	struct fatfs_node *node = v->v_data;

	if (!node) {
		return -1;
	}

	return fatfs_node_sync(node);
}

static int fatfs_readdir(struct vfs_node_t * dv, s64_t off, struct vfs_dirent_t * d)
{
	struct fatfs_node *dnode = dv->v_data;

	return fatfs_node_read_dirent(dnode, off, d);
}

static int fatfs_lookup(struct vfs_node_t * dv, const char *name, struct vfs_node_t * v)
{
	int rc;
	u32 off, len;
	struct fat_dirent dent;
	struct fatfs_node *node = v->v_data;
	struct fatfs_node *dnode = dv->v_data;

	rc = fatfs_node_find_dirent(dnode, name, &dent, &off, &len);
	if (rc) {
		return rc;
	}

	node->ctrl = dnode->ctrl;
	node->parent = dnode;
	node->parent_dent_off = off;
	node->parent_dent_len = len;
	memcpy(&node->parent_dent, &dent, sizeof(struct fat_dirent));
	if (dnode->ctrl->type == FAT_TYPE_32) {
		node->first_cluster = __le16(dent.first_cluster_hi);
		node->first_cluster = node->first_cluster << 16;
	} else {
		node->first_cluster = 0;
	}
	node->first_cluster |= __le16(dent.first_cluster_lo);

	v->v_mode = 0;

	if (dent.file_attributes & FAT_DIRENT_SUBDIR) {
		v->v_type = VNT_DIR;
		v->v_mode |= S_IFDIR;
	} else {
		v->v_type = VNT_REG;
		v->v_mode |= S_IFREG;
	}

	v->v_mode |= (S_IRWXU | S_IRWXG | S_IRWXO);
	if (dent.file_attributes & FAT_DIRENT_READONLY) {
		v->v_mode &= ~(S_IWUSR| S_IWGRP | S_IWOTH);
	}

	v->v_ctime = fatfs_pack_timestamp(dent.create_date_year,
					  dent.create_date_month,
					  dent.create_date_day,
					  dent.create_time_hours,
					  dent.create_time_minutes,
					  dent.create_time_seconds);
	v->v_atime = fatfs_pack_timestamp(dent.laccess_date_year,
					  dent.laccess_date_month,
					  dent.laccess_date_day,
					  0, 0, 0);
	v->v_mtime = fatfs_pack_timestamp(dent.lmodify_date_year,
					  dent.lmodify_date_month,
					  dent.lmodify_date_day,
					  dent.lmodify_time_hours,
					  dent.lmodify_time_minutes,
					  dent.lmodify_time_seconds);

	v->v_size = fatfs_node_get_size(node);

	return 0;
}

static int fatfs_create(struct vfs_node_t * dv, const char *name, u32 mode)
{
	int rc;
	u8 fileattr;
	u32 off, len;
	u32 year, mon, day, hour, min, sec;
	struct fat_dirent dent;
	struct fatfs_node *dnode = dv->v_data;

	rc = fatfs_node_find_dirent(dnode, name, &dent, &off, &len);
	if (rc != -1) {
		if (!rc) {
			return -1;
		} else {
			return rc;
		}
	}

	fatfs_current_timestamp(&year, &mon, &day, &hour, &min, &sec);

	fileattr = 0;
	if (!(mode & (S_IWUSR|S_IWGRP|S_IWOTH))) {
		fileattr |= FAT_DIRENT_READONLY;
	}

	memset(&dent, 0, sizeof(dent));
	dent.file_attributes = fileattr;
	dent.create_time_millisecs = 0;
	dent.create_time_seconds = sec;
	dent.create_time_minutes = min;
	dent.create_time_hours = hour;
	dent.create_date_day = day;
	dent.create_date_month = mon;
	dent.create_date_year = year;
	dent.laccess_date_day = day;
	dent.laccess_date_month = mon;
	dent.laccess_date_year = year;
	dent.first_cluster_hi = 0;
	dent.lmodify_time_seconds = sec;
	dent.lmodify_time_minutes = min;
	dent.lmodify_time_hours = hour;
	dent.lmodify_date_day = day;
	dent.lmodify_date_month = mon;
	dent.lmodify_date_year = year;
	dent.first_cluster_lo = 0;
	dent.file_size = 0;

	rc = fatfs_node_add_dirent(dnode, name, &dent);
	if (rc) {
		return rc;
	}

	return 0;
}

static int fatfs_remove(struct vfs_node_t * dv, struct vfs_node_t * v, const char *name)
{
	int rc;
	u32 off, len, clust;
	struct fat_dirent dent;
	struct fatfs_node *dnode = dv->v_data;
	struct fatfs_node *node = v->v_data;

	rc = fatfs_node_find_dirent(dnode, name, &dent, &off, &len);
	if (rc) {
		return rc;
	}

	if (dnode->ctrl->type == FAT_TYPE_32) {
		clust = __le16(dent.first_cluster_hi);
		clust = clust << 16;
	} else {
		clust = 0;
	}
	clust |= __le16(dent.first_cluster_lo);

	if (node->first_cluster != clust) {
		return -1;
	}

	rc = fatfs_node_del_dirent(dnode, name, off, len);
	if (rc) {
		return rc;
	}

	return 0;
}

static int fatfs_rename(struct vfs_node_t * sv, const char *sname, struct vfs_node_t * v,
			 struct vfs_node_t * dv, const char *dname)
{
	int rc;
	u32 off, len;
	struct fat_dirent dent;
	struct fatfs_node *snode = sv->v_data;
	struct fatfs_node *dnode = dv->v_data;

	rc = fatfs_node_find_dirent(dnode, dname, &dent, &off, &len);
	if (rc != -1) {
		if (!rc) {
			return -1;
		} else {
			return rc;
		}
	}

	rc = fatfs_node_find_dirent(snode, sname, &dent, &off, &len);
	if (rc) {
		return rc;
	}

	rc = fatfs_node_del_dirent(snode, sname, off, len);
	if (rc) {
		return rc;
	}

	rc = fatfs_node_add_dirent(dnode, dname, &dent);
	if (rc) {
		return rc;
	}

	return 0;
}

static int fatfs_mkdir(struct vfs_node_t * dv, const char *name, u32 mode)
{
	int rc;
	u8 fileattr;
	u32 off, len;
	u32 year, mon, day, hour, min, sec;
	struct fat_dirent dent;
	struct fatfs_node *dnode = dv->v_data;

	rc = fatfs_node_find_dirent(dnode, name, &dent, &off, &len);
	if (rc != -1) {
		if (!rc) {
			return -1;
		} else {
			return rc;
		}
	}

	fatfs_current_timestamp(&year, &mon, &day, &hour, &min, &sec);

	fileattr = FAT_DIRENT_SUBDIR;
	if (!(mode & (S_IWUSR|S_IWGRP|S_IWOTH))) {
		fileattr |= FAT_DIRENT_READONLY;
	}

	memset(&dent, 0, sizeof(dent));
	dent.file_attributes = fileattr;
	dent.create_time_millisecs = 0;
	dent.create_time_seconds = sec;
	dent.create_time_minutes = min;
	dent.create_time_hours = hour;
	dent.create_date_day = day;
	dent.create_date_month = mon;
	dent.create_date_year = year;
	dent.laccess_date_day = day;
	dent.laccess_date_month = mon;
	dent.laccess_date_year = year;
	dent.first_cluster_hi = 0;
	dent.lmodify_time_seconds = sec;
	dent.lmodify_time_minutes = min;
	dent.lmodify_time_hours = hour;
	dent.lmodify_date_day = day;
	dent.lmodify_date_month = mon;
	dent.lmodify_date_year = year;
	dent.first_cluster_lo = 0;
	dent.file_size = 0;

	rc = fatfs_node_add_dirent(dnode, name, &dent);
	if (rc) {
		return rc;
	}

	return 0;
}

static int fatfs_rmdir(struct vfs_node_t * dv, struct vfs_node_t * v, const char *name)
{
	int rc;
	u32 off, len, clust;
	struct fat_dirent dent;
	struct fatfs_node *dnode = dv->v_data;
	struct fatfs_node *node = v->v_data;

	rc = fatfs_node_find_dirent(dnode, name, &dent, &off, &len);
	if (rc) {
		return rc;
	}

	if (dnode->ctrl->type == FAT_TYPE_32) {
		clust = __le16(dent.first_cluster_hi);
		clust = clust << 16;
	} else {
		clust = 0;
	}
	clust |= __le16(dent.first_cluster_lo);

	if (node->first_cluster != clust) {
		return -1;
	}

	rc = fatfs_control_truncate_clusters(node->ctrl, clust);
	if (rc) {
		return rc;
	}

	rc = fatfs_node_del_dirent(dnode, name, off, len);
	if (rc) {
		return rc;
	}

	return 0;
}

static int fatfs_chmod(struct vfs_node_t * v, u32 mode)
{
	u8 fileattr;
	u32 year, mon, day;
	struct fatfs_node *node = v->v_data;

	fatfs_current_timestamp(&year, &mon, &day, NULL, NULL, NULL);

	fileattr = 0;
	switch (v->v_type) {
	case VNT_DIR:
		fileattr |= FAT_DIRENT_SUBDIR;
		break;
	default:
		break;
	};

	if (!(mode & (S_IWUSR|S_IWGRP|S_IWOTH))) {
		fileattr |= FAT_DIRENT_READONLY;
	}

	node->parent_dent.file_attributes = fileattr;
	node->parent_dent.laccess_date_day = day;
	node->parent_dent.laccess_date_month = day;
	node->parent_dent.laccess_date_year = day;
	node->parent_dent_dirty = TRUE;

	v->v_mode &= ~(S_IRWXU|S_IRWXG|S_IRWXO);
	v->v_mode |= mode;

	return 0;
}

static struct filesystem_t fat = {
	.name		= "fat",

	.mount		= fatfs_mount,
	.unmount	= fatfs_unmount,
	.msync		= fatfs_msync,
	.vget		= fatfs_vget,
	.vput		= fatfs_vput,

	.read		= fatfs_read,
	.write		= fatfs_write,
	.truncate	= fatfs_truncate,
	.sync		= fatfs_sync,
	.readdir	= fatfs_readdir,
	.lookup		= fatfs_lookup,
	.create		= fatfs_create,
	.remove		= fatfs_remove,
	.rename		= fatfs_rename,
	.mkdir		= fatfs_mkdir,
	.rmdir		= fatfs_rmdir,
	.chmod		= fatfs_chmod,
};

static __init void filesystem_fat_init(void)
{
	register_filesystem(&fat);
}

static __exit void filesystem_fat_exit(void)
{
	unregister_filesystem(&fat);
}

core_initcall(filesystem_fat_init);
core_exitcall(filesystem_fat_exit);
