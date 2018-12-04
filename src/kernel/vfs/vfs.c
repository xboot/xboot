/*
 * kernel/vfs/vfs.c
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

#include <xboot.h>
#include <vfs/vfs.h>

static struct list_head __filesystem_list = {
	.next = &__filesystem_list,
	.prev = &__filesystem_list,
};
static spinlock_t __filesystem_lock = SPIN_LOCK_INIT();

static struct vfs_filesystem_t * search_filesystem(const char * name)
{
	struct vfs_filesystem_t * pos, * n;

	if(!name)
		return NULL;

	list_for_each_entry_safe(pos, n, &__filesystem_list, list)
	{
		if(strcmp(pos->name, name) == 0)
			return pos;
	}
	return NULL;
}

bool_t register_filesystem(struct vfs_filesystem_t * fs)
{
	irq_flags_t flags;

	if(!fs || !fs->name)
		return FALSE;

	if(search_filesystem(fs->name))
		return FALSE;

	spin_lock_irqsave(&__filesystem_lock, flags);
	list_add_tail(&fs->list, &__filesystem_list);
	spin_unlock_irqrestore(&__filesystem_lock, flags);
	return TRUE;
}

bool_t unregister_filesystem(struct vfs_filesystem_t * fs)
{
	irq_flags_t flags;

	if(!fs || !fs->name)
		return FALSE;

	spin_lock_irqsave(&__filesystem_lock, flags);
	list_del(&fs->list);
	spin_unlock_irqrestore(&__filesystem_lock, flags);
	return TRUE;
}












/** file descriptor structure */
struct file {
	spinlock_t f_lock;	/* file lock */
	u32_t f_flags;			/* open flag */
	loff_t f_offset;		/* current position in file */
	struct vfs_node_t *f_vnode;		/* vfs_node_t */
};

/* size of vfs_node_t hash table, must power 2 */
#define VFS_VNODE_HASH_SIZE		(32)

struct vfs_ctrl {
	spinlock_t fs_list_lock;
	struct list_head fs_list;
	spinlock_t mnt_list_lock;
	struct list_head mnt_list;
	spinlock_t vnode_list_lock[VFS_VNODE_HASH_SIZE];
	struct list_head vnode_list[VFS_VNODE_HASH_SIZE];
	spinlock_t fd_bmap_lock;
	unsigned long *fd_bmap;
	struct file fd[VFS_MAX_FD];
//	struct vmm_notifier_block bdev_client;
};

static struct vfs_ctrl vfsc;

/** Compare two path strings and return matched length. */
static int count_match(const char *path, char *mount_root)
{
	int len = 0;

	while (*path && *mount_root) {
		if ((*path++) != (*mount_root++))
			break;
		len++;
	}

	if (*mount_root != '\0') {
		return 0;
	}

	if ((len == 1) && (*(path - 1) == '/')) {
		return 1;
	}

	if ((*path == '\0') || (*path == '/')) {
		return len;
	}

	return 0;
}

#if 0
static int vfs_findroot(const char *path, struct vfs_mount_t **mp, char **root)
{
	struct vfs_mount_t *m, *tmp;
	int len, max_len = 0;

	if (!path || !mp || !root) {
		return -1;
	}

	/* find vfs_mount_t point from nearest path */
	m = NULL;

	vmm_mutex_lock(&vfsc.mnt_list_lock);

	list_for_each_entry(tmp, &vfsc.mnt_list, m_link) {
		len = count_match(path, tmp->m_path);
		if (len > max_len) {
			max_len = len;
			m = tmp;
		}
	}

	vmm_mutex_unlock(&vfsc.mnt_list_lock);

	if (m == NULL) {
		return -1;
	}

	*root = (char *)(path + max_len);
	while (**root == '/') {
		(*root)++;
	}
	*mp = m;

	return 0;
}
#endif

static int vfs_fd_alloc(void)
{
	int i, ret = -1;

	vmm_mutex_lock(&vfsc.fd_bmap_lock);

	for (i = 0; i < VFS_MAX_FD; i++) {
/*		if (!bitmap_isset(vfsc.fd_bmap, i)) {
			bitmap_setbit(vfsc.fd_bmap, i);
			ret = i;
			break;
		}*/
	}

	vmm_mutex_unlock(&vfsc.fd_bmap_lock);

	return ret;
}

static void vfs_fd_free(int fd)
{
	if (-1 < fd && fd < VFS_MAX_FD) {
		vmm_mutex_lock(&vfsc.fd_bmap_lock);

/*		if (bitmap_isset(vfsc.fd_bmap, fd)) {
			vmm_mutex_lock(&vfsc.fd[fd].f_lock);
			vfsc.fd[fd].f_flags = 0;
			vfsc.fd[fd].f_offset = 0;
			vfsc.fd[fd].f_vnode = NULL;
			vmm_mutex_unlock(&vfsc.fd[fd].f_lock);
			bitmap_clearbit(vfsc.fd_bmap, fd);
		}*/

		vmm_mutex_unlock(&vfsc.fd_bmap_lock);
	}
}

static struct file *vfs_fd_to_file(int fd)
{
	return (-1 < fd && fd < VFS_MAX_FD) ? &vfsc.fd[fd] : NULL;
}

static u32_t vfs_vnode_hash(struct vfs_mount_t *m, const char *path)
{
	u32_t val = 0;

	if (path) {
		while (*path) {
			val = ((val << 5) + val) + *path++;
		}
	}

	return (val ^ (u32_t)(unsigned long)m) & (VFS_VNODE_HASH_SIZE - 1);
}


static struct vfs_node_t *vfs_vnode_vget(struct vfs_mount_t *m, const char *path)
{
	int err;
	u32_t hash;
	struct vfs_node_t *v;

	v = NULL;
	hash = vfs_vnode_hash(m, path);

	if (!(v = vmm_zalloc(sizeof(struct vfs_node_t)))) {
		return NULL;
	}

	INIT_LIST_HEAD(&v->v_link);
	INIT_MUTEX(&v->v_lock);
	v->v_mount = m;
	arch_atomic_write(&v->v_refcnt, 1);
	if (strlcpy(v->v_path, path, sizeof(v->v_path)) >=
	    sizeof(v->v_path)) {
		vmm_free(v);
		return NULL;
	}

	/* request to allocate fs specific data for vfs_node_t. */
	vmm_mutex_lock(&m->m_lock);
	err = m->m_fs->vget(m, v);
	vmm_mutex_unlock(&m->m_lock);
	if (err) {
		vmm_free(v);
		return NULL;
	}

	arch_atomic_add(&m->m_refcnt, 1);

	vmm_mutex_lock(&vfsc.vnode_list_lock[hash]);
	list_add(&v->v_link, &vfsc.vnode_list[hash]);
	vmm_mutex_unlock(&vfsc.vnode_list_lock[hash]);

	return v;
}

static struct vfs_node_t *vfs_vnode_lookup(struct vfs_mount_t *m, const char *path)
{
	u32_t hash;
	bool_t found = FALSE;
	struct vfs_node_t *v = NULL;

	hash = vfs_vnode_hash(m, path);

	vmm_mutex_lock(&vfsc.vnode_list_lock[hash]);

	list_for_each_entry(v, &vfsc.vnode_list[hash], v_link) {
		if ((v->v_mount == m) &&
		    (!strncmp(v->v_path, path, VFS_MAX_PATH))) {
			found = TRUE;
			break;
		}
	}

	vmm_mutex_unlock(&vfsc.vnode_list_lock[hash]);

	if (!found) {
		return NULL;
	}

	arch_atomic_add(&v->v_refcnt, 1);

	return v;
}

static void vfs_vnode_vref(struct vfs_node_t *v)
{
	arch_atomic_add(&v->v_refcnt, 1);
}

static void vfs_vnode_vput(struct vfs_node_t *v)
{
	u32_t hash;

	if (arch_atomic_sub_return(&v->v_refcnt, 1)) {
		return;
	}

	hash = vfs_vnode_hash(v->v_mount, v->v_path);

	vmm_mutex_lock(&vfsc.vnode_list_lock[hash]);
	list_del(&v->v_link);
	vmm_mutex_unlock(&vfsc.vnode_list_lock[hash]);

	/* deallocate fs specific data from this vfs_node_t */
	vmm_mutex_lock(&v->v_mount->m_lock);
	v->v_mount->m_fs->vput(v->v_mount, v);
	vmm_mutex_unlock(&v->v_mount->m_lock);

	arch_atomic_sub(&v->v_mount->m_refcnt, 1);

	vmm_free(v);
}

static int vfs_vnode_stat(struct vfs_node_t *v, struct vfs_stat_t *st)
{
	u32_t mode;

	memset(st, 0, sizeof(struct vfs_stat_t));
/*
	st->st_ino = (u64_t)(unsigned long)v;
	vmm_mutex_lock(&v->v_lock);
	st->st_size = v->v_size;
	mode = v->v_mode & (S_IRWXU|S_IRWXG|S_IRWXO);
	st->st_ctime = v->v_ctime;
	st->st_atime = v->v_atime;
	st->st_mtime = v->v_mtime;
	vmm_mutex_unlock(&v->v_lock);

	switch (v->v_type) {
	case VREG:
		mode |= S_IFREG;
		break;
	case VDIR:
		mode |= S_IFDIR;
		break;
	case VBLK:
		mode |= S_IFBLK;
		break;
	case VCHR:
		mode |= S_IFCHR;
		break;
	case VLNK:
		mode |= S_IFLNK;
		break;
	case VSOCK:
		mode |= S_IFSOCK;
		break;
	case VFIFO:
		mode |= S_IFIFO;
		break;
	default:
		return VMM_EFAIL;
	};
	st->st_mode = mode;

	if (v->v_type == VCHR || v->v_type == VBLK)
		st->st_dev = (u64)(unsigned long)v->v_data;

	st->st_uid = 0;
	st->st_gid = 0;*/

	return 0;
}
