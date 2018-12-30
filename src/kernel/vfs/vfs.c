/*
 * kernel/vfs/vfs.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
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

static struct kobj_t * search_class_filesystem_kobj(void)
{
	struct kobj_t * kclass = kobj_search_directory_with_create(kobj_get_root(), "class");
	return kobj_search_directory_with_create(kclass, "filesystem");
}

struct filesystem_t * search_filesystem(const char * name)
{
	struct filesystem_t * pos, * n;

	if(!name)
		return NULL;

	list_for_each_entry_safe(pos, n, &__filesystem_list, list)
	{
		if(strcmp(pos->name, name) == 0)
			return pos;
	}
	return NULL;
}

bool_t register_filesystem(struct filesystem_t * fs)
{
	irq_flags_t flags;

	if(!fs || !fs->name)
		return FALSE;

	if(search_filesystem(fs->name))
		return FALSE;

	fs->kobj = kobj_alloc_directory(fs->name);
	kobj_add(search_class_filesystem_kobj(), fs->kobj);

	spin_lock_irqsave(&__filesystem_lock, flags);
	list_add_tail(&fs->list, &__filesystem_list);
	spin_unlock_irqrestore(&__filesystem_lock, flags);

	return TRUE;
}

bool_t unregister_filesystem(struct filesystem_t * fs)
{
	irq_flags_t flags;

	if(!fs || !fs->name)
		return FALSE;

	spin_lock_irqsave(&__filesystem_lock, flags);
	list_del(&fs->list);
	spin_unlock_irqrestore(&__filesystem_lock, flags);
	kobj_remove(search_class_filesystem_kobj(), fs->kobj);
	kobj_remove_self(fs->kobj);

	return TRUE;
}

struct vfs_file_t {
	struct mutex_t f_lock;
	struct vfs_node_t * f_node;
	s64_t f_offset;
	u32_t f_flags;
};

static struct list_head mnt_list;
static struct mutex_t mnt_list_lock;
static struct vfs_file_t fd_file[VFS_MAX_FD];
static struct mutex_t fd_file_lock;
struct list_head node_list[VFS_NODE_HASH_SIZE];
static struct mutex_t node_list_lock[VFS_NODE_HASH_SIZE];

static int count_match(const char * path, char * mount_root)
{
	int len = 0;

	while(*path && *mount_root)
	{
		if((*path++) != (*mount_root++))
			break;
		len++;
	}

	if(*mount_root != '\0')
		return 0;

	if((len == 1) && (*(path - 1) == '/'))
		return 1;

	if((*path == '\0') || (*path == '/'))
		return len;

	return 0;
}

static int vfs_findroot(const char * path, struct vfs_mount_t ** mp, char ** root)
{
	struct vfs_mount_t * pos, * m = NULL;
	int len, max_len = 0;

	if(!path || !mp || !root)
		return -1;

	mutex_lock(&mnt_list_lock);
	list_for_each_entry(pos, &mnt_list, m_link)
	{
		len = count_match(path, pos->m_path);
		if(len > max_len)
		{
			max_len = len;
			m = pos;
		}
	}
	mutex_unlock(&mnt_list_lock);

	if(!m)
		return -1;

	*root = (char *)(path + max_len);
	while(**root == '/')
	{
		(*root)++;
	}
	*mp = m;

	return 0;
}

static int vfs_fd_alloc(void)
{
	int i, fd = -1;

	mutex_lock(&fd_file_lock);
	for(i = 3; i < VFS_MAX_FD; i++)
	{
		if(fd_file[i].f_node == NULL)
		{
			fd = i;
			break;
		}
	}
	mutex_unlock(&fd_file_lock);

	return fd;
}

static void vfs_fd_free(int fd)
{
	if((fd >= 3) && (fd < VFS_MAX_FD))
	{
		mutex_lock(&fd_file_lock);
		if(fd_file[fd].f_node)
		{
			mutex_lock(&fd_file[fd].f_lock);
			fd_file[fd].f_node = NULL;
			fd_file[fd].f_offset = 0;
			fd_file[fd].f_flags = 0;
			mutex_unlock(&fd_file[fd].f_lock);
		}
		mutex_unlock(&fd_file_lock);
	}
}

static struct vfs_file_t * vfs_fd_to_file(int fd)
{
	return ((fd >= 0) && (fd < VFS_MAX_FD)) ? &fd_file[fd] : NULL;
}

static u32_t vfs_node_hash(struct vfs_mount_t * m, const char * path)
{
	u32_t val = 0;

	if(path)
	{
		while(*path)
			val = ((val << 5) + val) + *path++;
	}
	return (val ^ (u32_t)((unsigned long)m)) & (VFS_NODE_HASH_SIZE - 1);
}

static struct vfs_node_t * vfs_node_get(struct vfs_mount_t * m, const char * path)
{
	struct vfs_node_t * n;
	u32_t hash = vfs_node_hash(m, path);
	int err;

	if(!(n = calloc(1, sizeof(struct vfs_node_t))))
		return NULL;

	init_list_head(&n->v_link);
	mutex_init(&n->v_lock);
	n->v_mount = m;
	atomic_set(&n->v_refcnt, 1);
	if(strlcpy(n->v_path, path, sizeof(n->v_path)) >= sizeof(n->v_path))
	{
		free(n);
		return NULL;
	}

	mutex_lock(&m->m_lock);
	err = m->m_fs->vget(m, n);
	mutex_unlock(&m->m_lock);
	if(err)
	{
		free(n);
		return NULL;
	}

	atomic_add(&m->m_refcnt, 1);
	mutex_lock(&node_list_lock[hash]);
	list_add(&n->v_link, &node_list[hash]);
	mutex_unlock(&node_list_lock[hash]);

	return n;
}

static struct vfs_node_t * vfs_node_lookup(struct vfs_mount_t * m, const char * path)
{
	struct vfs_node_t * n;
	u32_t hash = vfs_node_hash(m, path);
	int found = 0;

	mutex_lock(&node_list_lock[hash]);
	list_for_each_entry(n, &node_list[hash], v_link)
	{
		if((n->v_mount == m) && (!strncmp(n->v_path, path, VFS_MAX_PATH)))
		{
			found = 1;
			break;
		}
	}
	mutex_unlock(&node_list_lock[hash]);

	if(!found)
		return NULL;
	atomic_add(&n->v_refcnt, 1);

	return n;
}

static void vfs_node_ref(struct vfs_node_t * n)
{
	atomic_add(&n->v_refcnt, 1);
}

static void vfs_node_put(struct vfs_node_t * n)
{
	u32_t hash;

	if(atomic_sub_return(&n->v_refcnt, 1))
		return;

	hash = vfs_node_hash(n->v_mount, n->v_path);
	mutex_lock(&node_list_lock[hash]);
	list_del(&n->v_link);
	mutex_unlock(&node_list_lock[hash]);

	mutex_lock(&n->v_mount->m_lock);
	n->v_mount->m_fs->vput(n->v_mount, n);
	mutex_unlock(&n->v_mount->m_lock);

	atomic_sub(&n->v_mount->m_refcnt, 1);
	free(n);
}

static int vfs_node_stat(struct vfs_node_t * n, struct vfs_stat_t * st)
{
	u32_t mode;

	memset(st, 0, sizeof(struct vfs_stat_t));

	st->st_ino = (u64_t)((unsigned long)n);
	mutex_lock(&n->v_lock);
	st->st_size = n->v_size;
	mode = n->v_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
	st->st_ctime = n->v_ctime;
	st->st_atime = n->v_atime;
	st->st_mtime = n->v_mtime;
	mutex_unlock(&n->v_lock);

	switch(n->v_type)
	{
	case VNT_REG:
		mode |= S_IFREG;
		break;
	case VNT_DIR:
		mode |= S_IFDIR;
		break;
	case VNT_BLK:
		mode |= S_IFBLK;
		break;
	case VNT_CHR:
		mode |= S_IFCHR;
		break;
	case VNT_LNK:
		mode |= S_IFLNK;
		break;
	case VNT_SOCK:
		mode |= S_IFSOCK;
		break;
	case VNT_FIFO:
		mode |= S_IFIFO;
		break;
	default:
		return -1;
	};
	st->st_mode = mode;

	if(n->v_type == VNT_CHR || n->v_type == VNT_BLK)
		st->st_dev = (u64_t)((unsigned long)n->v_data);
	st->st_uid = 0;
	st->st_gid = 0;

	return 0;
}

static int vfs_node_access(struct vfs_node_t * n, u32_t mode)
{
	u32_t m;

	mutex_lock(&n->v_lock);
	m = n->v_mode;
	mutex_unlock(&n->v_lock);

	if((mode & R_OK) && !(m & (S_IRUSR | S_IRGRP | S_IROTH)))
		return -1;

	if(mode & W_OK)
	{
		if(n->v_mount->m_flags & MOUNT_RDONLY)
			return -1;

		if(!(m & (S_IWUSR | S_IWGRP | S_IWOTH)))
			return -1;
	}

	if((mode & X_OK) && !(m & (S_IXUSR | S_IXGRP | S_IXOTH)))
		return -1;

	return 0;
}

static void vfs_node_release(struct vfs_node_t * n)
{
	struct vfs_mount_t * m;
	char path[VFS_MAX_PATH];
	char * p;

	if(!n)
		return;

	m = n->v_mount;

	if(m->m_root == n)
	{
		vfs_node_put(n);
		return;
	}

	if(strlcpy(path, n->v_path, sizeof(path)) >= sizeof(path))
		return;

	vfs_node_put(n);

	while(1)
	{
		p = strrchr(path, '/');
		if(!p)
			break;
		*p = '\0';

		if(path[0] == '\0')
			break;

		n = vfs_node_lookup(m, path);
		if(!n)
			continue;

		vfs_node_put(n);
		vfs_node_put(n);
	}
	vfs_node_put(m->m_root);
}

static int vfs_node_acquire(const char * path, struct vfs_node_t ** np)
{
	struct vfs_mount_t * m;
	struct vfs_node_t * dn, * n;
	char node[VFS_MAX_PATH];
	char * p;
	int err, i, j;

	if(vfs_findroot(path, &m, &p))
		return -1;

	if(!m->m_root)
		return -1;

	dn = n = m->m_root;
	vfs_node_ref(dn);

	i = 0;
	while(*p != '\0')
	{
		while(*p == '/')
			p++;

		if(*p == '\0')
			break;

		node[i] = '/';
		i++;
		j = i;
		while(*p != '\0' && *p != '/')
		{
			node[i] = *p;
			p++;
			i++;
		}
		node[i] = '\0';

		n = vfs_node_lookup(m, node);
		if(n == NULL)
		{
			n = vfs_node_get(m, node);
			if(n == NULL)
			{
				vfs_node_put(dn);
				return -1;
			}

			mutex_lock(&n->v_lock);
			mutex_lock(&dn->v_lock);
			err = dn->v_mount->m_fs->lookup(dn, &node[j], n);
			mutex_unlock(&dn->v_lock);
			mutex_unlock(&n->v_lock);
			if(err || (*p == '/' && n->v_type != VNT_DIR))
			{
				vfs_node_release(n);
				return err;
			}
		}
		dn = n;
	}
	*np = n;

	return 0;
}

static void vfs_force_unmount(struct vfs_mount_t * m)
{
	struct vfs_mount_t * tm;
	struct vfs_node_t * n;
	int found;
	int i;

	while(1)
	{
		found = 0;
		list_for_each_entry(tm, &mnt_list, m_link)
		{
			if(tm->m_covered && tm->m_covered->v_mount == m)
			{
				found = 1;
				break;
			}
		}
		if(!found)
			break;
		vfs_force_unmount(tm);
	}
	list_del(&m->m_link);

	mutex_lock(&fd_file_lock);
	for(i = 0; i < VFS_MAX_FD; i++)
	{
		if(fd_file[i].f_node && (fd_file[i].f_node->v_mount == m))
		{
			mutex_lock(&fd_file[i].f_lock);
			fd_file[i].f_node = NULL;
			fd_file[i].f_offset = 0;
			fd_file[i].f_flags = 0;
			mutex_unlock(&fd_file[i].f_lock);
		}
	}
	mutex_unlock(&fd_file_lock);

	for(i = 0; i < VFS_NODE_HASH_SIZE; i++)
	{
		mutex_lock(&node_list_lock[i]);
		while(1)
		{
			found = 0;
			list_for_each_entry(n, &node_list[i], v_link)
			{
				if(n->v_mount == m)
				{
					found = 1;
					break;
				}
			}
			if(!found)
				break;

			list_del(&n->v_link);
			mutex_lock(&n->v_mount->m_lock);
			n->v_mount->m_fs->vput(n->v_mount, n);
			mutex_unlock(&n->v_mount->m_lock);
			free(n);
		}
		mutex_unlock(&node_list_lock[i]);
	}

	mutex_lock(&m->m_lock);
	m->m_fs->unmount(m);
	mutex_unlock(&m->m_lock);

	if(m->m_covered)
		vfs_node_release(m->m_covered);
	free(m);
}

int vfs_mount(const char * dev, const char * dir, const char * fsname, u32_t flags)
{
	struct block_t * bdev;
	struct filesystem_t * fs;
	struct vfs_mount_t * m, * tm;
	struct vfs_node_t * n, * n_covered;
	int err;

	if(!dir || *dir == '\0' || !(flags & MOUNT_MASK))
		return -1;

	if(!(fs = search_filesystem(fsname)))
		return -1;

	if(dev != NULL)
	{
		if(!(bdev = search_block(dev)))
			return -1;
	}
	else
	{
		bdev = NULL;
	}

	if(!(m = calloc(1, sizeof(struct vfs_mount_t))))
		return -1;

	init_list_head(&m->m_link);
	mutex_init(&m->m_lock);
	m->m_fs = fs;
	m->m_flags = flags & MOUNT_MASK;
	atomic_set(&m->m_refcnt, 0);
	if(strlcpy(m->m_path, dir, sizeof(m->m_path)) >= sizeof(m->m_path))
	{
		free(m);
		return -1;
	}
	m->m_dev = bdev;

	if(*dir == '/' && *(dir + 1) == '\0')
	{
		n_covered = NULL;
	}
	else
	{
		if(vfs_node_acquire(dir, &n_covered) != 0)
		{
			free(m);
			return -1;
		}
		if(n_covered->v_type != VNT_DIR)
		{
			vfs_node_release(n_covered);
			free(m);
			return -1;
		}
	}
	m->m_covered = n_covered;

	if(!(n = vfs_node_get(m, "/")))
	{
		if(m->m_covered)
			vfs_node_release(m->m_covered);
		free(m);
		return -1;
	}
	n->v_type = VNT_DIR;
	n->v_flags = VNF_ROOT;
	n->v_mode = S_IFDIR | S_IRWXU | S_IRWXG | S_IRWXO;
	m->m_root = n;

	mutex_lock(&m->m_lock);
	err = m->m_fs->mount(m, dev, flags);
	mutex_unlock(&m->m_lock);
	if(err != 0)
	{
		vfs_node_release(m->m_root);
		if(m->m_covered)
			vfs_node_release(m->m_covered);
		free(m);
		return err;
	}

	if(m->m_flags & MOUNT_RDONLY)
		m->m_root->v_mode &= ~(S_IWUSR|S_IWGRP|S_IWOTH);

	mutex_lock(&mnt_list_lock);
	list_for_each_entry(tm, &mnt_list, m_link)
	{
		if(!strcmp(tm->m_path, dir) || ((dev != NULL) && (tm->m_dev == bdev)))
		{
			mutex_unlock(&mnt_list_lock);
			mutex_lock(&m->m_lock);
			m->m_fs->unmount(m);
			mutex_unlock(&m->m_lock);
			vfs_node_release(m->m_root);
			if(m->m_covered)
				vfs_node_release(m->m_covered);
			free(m);
			return -1;
		}
	}
	list_add(&m->m_link, &mnt_list);
	mutex_unlock(&mnt_list_lock);

	return 0;
}

int vfs_unmount(const char * path)
{
	struct vfs_mount_t * m;
	int found;
	int err;

	mutex_lock(&mnt_list_lock);
	found = 0;
	list_for_each_entry(m, &mnt_list, m_link)
	{
		if(!strcmp(path, m->m_path))
		{
			found = 1;
			break;
		}
	}
	if(!found)
	{
		mutex_unlock(&mnt_list_lock);
		return -1;
	}
	if(atomic_get(&m->m_refcnt) > 1)
	{
		mutex_unlock(&mnt_list_lock);
		return -1;
	}
	list_del(&m->m_link);
	mutex_unlock(&mnt_list_lock);

	mutex_lock(&m->m_lock);
	err = m->m_fs->msync(m);
	m->m_fs->unmount(m);
	mutex_unlock(&m->m_lock);

	vfs_node_release(m->m_root);
	if(m->m_covered)
		vfs_node_release(m->m_covered);
	free(m);

	return err;
}

int vfs_sync(void)
{
	struct vfs_mount_t * m;

	mutex_lock(&mnt_list_lock);
	list_for_each_entry(m, &mnt_list, m_link)
	{
		mutex_lock(&m->m_lock);
		m->m_fs->msync(m);
		mutex_unlock(&m->m_lock);
	}
	mutex_unlock(&mnt_list_lock);

	return 0;
}

struct vfs_mount_t * vfs_mount_get(int index)
{
	struct vfs_mount_t * m = NULL;
	int found = 0;

	if(index < 0)
		return NULL;

	mutex_lock(&mnt_list_lock);
	list_for_each_entry(m, &mnt_list, m_link)
	{
		if(!index)
		{
			found = 1;
			break;
		}
		index--;
	}
	mutex_unlock(&mnt_list_lock);

	if(!found)
		return NULL;
	return m;
}

u32_t vfs_mount_count(void)
{
	struct vfs_mount_t * m;
	u32_t ret = 0;

	mutex_lock(&mnt_list_lock);
	list_for_each_entry(m, &mnt_list, m_link)
	{
		ret++;
	}
	mutex_unlock(&mnt_list_lock);

	return ret;
}

static int vfs_lookup_dir(const char * path, struct vfs_node_t ** np, char ** name)
{
	struct vfs_node_t * n;
	char buf[VFS_MAX_PATH];
	char * file, * dir;
	int err;

	if(strlcpy(buf, path, sizeof(buf)) >= sizeof(buf))
		return -1;

	file = strrchr(buf, '/');
	if(!file)
		return -1;

	if(!buf[0])
		return -1;
	if(file == buf)
	{
		dir = "/";
	}
	else
	{
		*file = '\0';
		dir = buf;
	}

	if((err = vfs_node_acquire(dir, &n)))
		return err;
	if(n->v_type != VNT_DIR)
	{
		vfs_node_release(n);
		return -1;
	}
	*np = n;

	*name = strrchr(path, '/');
	if(*name == NULL)
		return -1;
	*name += 1;

	return 0;
}

int vfs_open(const char * path, u32_t flags, u32_t mode)
{
	struct vfs_node_t * n, * dn;
	struct vfs_file_t * f;
	char * filename;
	int err, fd;

	if(!path || !(flags & O_ACCMODE))
		return -1;

	if(flags & O_CREAT)
	{
		err = vfs_node_acquire(path, &n);
		if(err)
		{
			if((err = vfs_lookup_dir(path, &dn, &filename)))
				return err;
			if((err = vfs_node_access(dn, W_OK)))
			{
				vfs_node_release(dn);
				return err;
			}
			mode &= ~S_IFMT;
			mode |= S_IFREG;
			mutex_lock(&dn->v_lock);
			err = dn->v_mount->m_fs->create(dn, filename, mode);
			mutex_unlock(&dn->v_lock);
			vfs_node_release(dn);
			if(err)
				return err;
			if((err = vfs_node_acquire(path, &n)))
				return err;
			flags &= ~O_TRUNC;
		}
		else
		{
			if(flags & O_EXCL)
			{
				vfs_node_release(n);
				return -1;
			}
			flags &= ~O_CREAT;
		}
	}
	else
	{
		if((err = vfs_node_acquire(path, &n)))
		{
			return err;
		}
		if((flags & O_WRONLY) || (flags & O_TRUNC))
		{
			if((err = vfs_node_access(n, W_OK)))
			{
				vfs_node_release(n);
				return err;
			}
			if(n->v_type == VNT_DIR)
			{
				vfs_node_release(n);
				return -1;
			}
		}
	}

	if(flags & O_TRUNC)
	{
		if(!(flags & O_WRONLY) || (n->v_type == VNT_DIR))
		{
			vfs_node_release(n);
			return -1;
		}
		mutex_lock(&n->v_lock);
		err = n->v_mount->m_fs->truncate(n, 0);
		mutex_unlock(&n->v_lock);
		if(err)
		{
			vfs_node_release(n);
			return err;
		}
	}

	fd = vfs_fd_alloc();
	if(fd < 0)
	{
		vfs_node_release(n);
		return -1;
	}
	f = vfs_fd_to_file(fd);

	mutex_lock(&f->f_lock);
	f->f_node = n;
	f->f_offset = 0;
	f->f_flags = flags;
	mutex_unlock(&f->f_lock);

	return fd;
}

int vfs_close(int fd)
{
	struct vfs_node_t * n;
	struct vfs_file_t * f;
	int err;

	f = vfs_fd_to_file(fd);
	if(!f)
		return -1;

	mutex_lock(&f->f_lock);
	n = f->f_node;
	if(!n)
	{
		mutex_unlock(&f->f_lock);
		return -1;
	}

	mutex_lock(&n->v_lock);
	err = n->v_mount->m_fs->sync(n);
	mutex_unlock(&n->v_lock);
	if(err)
	{
		mutex_unlock(&f->f_lock);
		return err;
	}
	vfs_node_release(n);
	mutex_unlock(&f->f_lock);

	vfs_fd_free(fd);
	return 0;
}

u64_t vfs_read(int fd, void * buf, u64_t len)
{
	struct vfs_node_t * n;
	struct vfs_file_t * f;
	u64_t ret;

	if(!buf || !len)
		return 0;

	f = vfs_fd_to_file(fd);
	if(!f)
		return 0;

	mutex_lock(&f->f_lock);
	n = f->f_node;
	if(!n)
	{
		mutex_unlock(&f->f_lock);
		return 0;
	}
	if(n->v_type != VNT_REG)
	{
		mutex_unlock(&f->f_lock);
		return 0;
	}

	if(!(f->f_flags & O_RDONLY))
	{
		mutex_unlock(&f->f_lock);
		return 0;
	}

	mutex_lock(&n->v_lock);
	ret = n->v_mount->m_fs->read(n, f->f_offset, buf, len);
	mutex_unlock(&n->v_lock);

	f->f_offset += ret;
	mutex_unlock(&f->f_lock);

	return ret;
}

u64_t vfs_write(int fd, void * buf, u64_t len)
{
	struct vfs_node_t * n;
	struct vfs_file_t * f;
	u64_t ret;

	if(!buf || !len)
		return 0;

	f = vfs_fd_to_file(fd);
	if(!f)
		return 0;

	mutex_lock(&f->f_lock);
	n = f->f_node;
	if(!n)
	{
		mutex_unlock(&f->f_lock);
		return 0;
	}
	if(n->v_type != VNT_REG)
	{
		mutex_unlock(&f->f_lock);
		return 0;
	}

	if(!(f->f_flags & O_WRONLY))
	{
		mutex_unlock(&f->f_lock);
		return 0;
	}

	mutex_lock(&n->v_lock);
	ret = n->v_mount->m_fs->write(n, f->f_offset, buf, len);
	mutex_unlock(&n->v_lock);

	f->f_offset += ret;
	mutex_unlock(&f->f_lock);

	return ret;
}

s64_t vfs_lseek(int fd, s64_t off, int whence)
{
	struct vfs_node_t * n;
	struct vfs_file_t * f;
	s64_t ret;

	f = vfs_fd_to_file(fd);
	if(!f)
		return 0;

	mutex_lock(&f->f_lock);
	n = f->f_node;
	if(!n)
	{
		mutex_unlock(&f->f_lock);
		return 0;
	}

	mutex_lock(&n->v_lock);
	switch(whence)
	{
	case VFS_SEEK_SET:
		if(off < 0)
			off = 0;
		else if(off > (loff_t)n->v_size)
			off = n->v_size;
		break;

	case VFS_SEEK_CUR:
		if((f->f_offset + off) > (loff_t)n->v_size)
			off = n->v_size;
		else if((f->f_offset + off) < 0)
			off = 0;
		else
			off = f->f_offset + off;
		break;

	case VFS_SEEK_END:
		if(off > 0)
			off = n->v_size;
		else if((n->v_size + off) < 0)
			off = 0;
		else
			off = n->v_size + off;
		break;

	default:
		mutex_unlock(&n->v_lock);
		ret = f->f_offset;
		mutex_unlock(&f->f_lock);
		return ret;
	}

	if(off <= n->v_size)
		f->f_offset = off;
	mutex_unlock(&n->v_lock);

	ret = f->f_offset;
	mutex_unlock(&f->f_lock);

	return ret;
}

int vfs_fsync(int fd)
{
	struct vfs_node_t * n;
	struct vfs_file_t * f;
	int err;

	f = vfs_fd_to_file(fd);
	if(!f)
		return -1;

	mutex_lock(&f->f_lock);
	n = f->f_node;
	if(!n)
	{
		mutex_unlock(&f->f_lock);
		return -1;
	}
	if(!(f->f_flags & O_WRONLY))
	{
		mutex_unlock(&f->f_lock);
		return -1;
	}
	mutex_lock(&n->v_lock);
	err = n->v_mount->m_fs->sync(n);
	mutex_unlock(&n->v_lock);
	mutex_unlock(&f->f_lock);

	return err;
}

int vfs_fchmod(int fd, u32_t mode)
{
	struct vfs_node_t * n;
	struct vfs_file_t * f;
	int err;

	f = vfs_fd_to_file(fd);
	if(!f)
		return -1;

	mutex_lock(&f->f_lock);
	n = f->f_node;
	if(!n)
	{
		mutex_unlock(&f->f_lock);
		return -1;
	}
	mode &= (S_IRWXU | S_IRWXG | S_IRWXO);
	mutex_lock(&n->v_lock);
	err = n->v_mount->m_fs->chmod(n, mode);
	mutex_unlock(&n->v_lock);
	mutex_unlock(&f->f_lock);

	return err;
}

int vfs_fstat(int fd, struct vfs_stat_t * st)
{
	struct vfs_node_t * n;
	struct vfs_file_t * f;
	int err;

	if(!st)
		return -1;

	f = vfs_fd_to_file(fd);
	if(!f)
		return -1;

	mutex_lock(&f->f_lock);
	n = f->f_node;
	if(!n)
	{
		mutex_unlock(&f->f_lock);
		return -1;
	}
	err = vfs_node_stat(n, st);
	mutex_unlock(&f->f_lock);

	return err;
}

int vfs_opendir(const char * name)
{
	struct vfs_node_t * n;
	struct vfs_file_t * f;
	int fd;

	if(!name)
		return -1;

	if((fd = vfs_open(name, O_RDONLY, 0)) < 0)
		return fd;

	f = vfs_fd_to_file(fd);
	if(!f)
		return -1;

	mutex_lock(&f->f_lock);
	n = f->f_node;
	if(!n)
	{
		mutex_unlock(&f->f_lock);
		return -1;
	}

	if(n->v_type != VNT_DIR)
	{
		mutex_unlock(&f->f_lock);
		vfs_close(fd);
		return -1;
	}
	mutex_unlock(&f->f_lock);

	return fd;
}

int vfs_closedir(int fd)
{
	struct vfs_node_t * n;
	struct vfs_file_t * f;

	f = vfs_fd_to_file(fd);
	if(!f)
		return -1;

	mutex_lock(&f->f_lock);
	n = f->f_node;
	if(!n)
	{
		mutex_unlock(&f->f_lock);
		return -1;
	}

	if(n->v_type != VNT_DIR)
	{
		mutex_unlock(&f->f_lock);
		return -1;
	}
	mutex_unlock(&f->f_lock);

	return vfs_close(fd);
}

int vfs_readdir(int fd, struct vfs_dirent_t * dir)
{
	struct vfs_node_t * n;
	struct vfs_file_t * f;
	int err;

	if(!dir)
		return -1;

	f = vfs_fd_to_file(fd);
	if(!f)
		return -1;

	mutex_lock(&f->f_lock);
	n = f->f_node;
	if(!n)
	{
		mutex_unlock(&f->f_lock);
		return -1;
	}
	if(n->v_type != VNT_DIR)
	{
		mutex_unlock(&f->f_lock);
		return -1;
	}
	mutex_lock(&n->v_lock);
	err = n->v_mount->m_fs->readdir(n, f->f_offset, dir);
	mutex_unlock(&n->v_lock);
	if(!err)
		f->f_offset += dir->d_reclen;
	mutex_unlock(&f->f_lock);

	return err;
}

int vfs_rewinddir(int fd)
{
	struct vfs_node_t * n;
	struct vfs_file_t * f;

	f = vfs_fd_to_file(fd);
	if(!f)
		return -1;

	mutex_lock(&f->f_lock);
	n = f->f_node;
	if(!n)
	{
		mutex_unlock(&f->f_lock);
		return -1;
	}
	if(n->v_type != VNT_DIR)
	{
		mutex_unlock(&f->f_lock);
		return -1;
	}
	f->f_offset = 0;
	mutex_unlock(&f->f_lock);

	return 0;
}

int vfs_mkdir(const char * path, u32_t mode)
{
	struct vfs_node_t * n, * dn;
	char * name;
	int err;

	if(!path)
		return -1;

	if(!(err = vfs_node_acquire(path, &n)))
	{
		vfs_node_release(n);
		return -1;
	}

	if((err = vfs_lookup_dir(path, &dn, &name)))
		return err;

	if((err = vfs_node_access(dn, W_OK)))
	{
		vfs_node_release(dn);
		return err;
	}

	mode &= ~S_IFMT;
	mode |= S_IFDIR;

	mutex_lock(&dn->v_lock);

	err = dn->v_mount->m_fs->mkdir(dn, name, mode);
	if(err)
		goto fail;
	err = dn->v_mount->m_fs->sync(dn);

fail:
	mutex_unlock(&dn->v_lock);
	vfs_node_release(dn);

	return err;
}

static int vfs_check_dir_empty(const char * path)
{
	struct vfs_dirent_t dir;
	int err, fd, count;

	if((fd = vfs_opendir(path)) < 0)
		return fd;

	count = 0;
	do
	{
		err = vfs_readdir(fd, &dir);
		if(err)
			break;
		if((strcmp(dir.d_name, ".") != 0) && (strcmp(dir.d_name, "..") != 0))
			count++;
		if(count)
			break;
	} while(1);

	vfs_closedir(fd);
	if(count)
		return -1;

	return 0;
}

int vfs_rmdir(const char * path)
{
	struct vfs_node_t * n, * dn;
	char * name;
	int err;

	if(!path)
		return -1;

	if((err = vfs_check_dir_empty(path)))
		return err;

	if((err = vfs_node_acquire(path, &n)))
		return err;

	if((n->v_flags == VNF_ROOT) || (atomic_get(&n->v_refcnt) >= 2))
	{
		vfs_node_release(n);
		return -1;
	}

	if((err = vfs_node_access(n, W_OK)))
	{
		vfs_node_release(n);
		return err;
	}

	if((err = vfs_lookup_dir(path, &dn, &name)))
	{
		vfs_node_release(n);
		return err;
	}

	mutex_lock(&dn->v_lock);
	mutex_lock(&n->v_lock);

	err = dn->v_mount->m_fs->rmdir(dn, n, name);
	if(err)
		goto fail;

	err = n->v_mount->m_fs->sync(n);
	if(err)
		goto fail;
	err = dn->v_mount->m_fs->sync(dn);

fail:
	mutex_unlock(&n->v_lock);
	mutex_unlock(&dn->v_lock);
	vfs_node_release(n);
	vfs_node_release(dn);

	return err;
}

int vfs_rename(const char * src, const char * dst)
{
	struct vfs_node_t * n1, * n2, * sn, * dn;
	char * sname, * dname;
	int err, len;

	if(!strncmp(src, dst, VFS_MAX_PATH))
		return -1;

	if(!strcmp(src, "/"))
		return -1;

	len = strlen(src);
	if((len < strlen(dst)) && !strncmp(src, dst, len) && (dst[len] == '/'))
		return -1;

	if((err = vfs_node_acquire(src, &n1)))
		return err;

	if((err = vfs_node_access(n1, W_OK)))
		goto fail1;

	if(atomic_get(&n1->v_refcnt) >= 2)
	{
		err = -1;
		goto fail1;
	}

	if((err = vfs_lookup_dir(src, &sn, &sname)))
		goto fail1;

	err = vfs_node_acquire(dst, &n2);
	if(!err)
	{
		vfs_node_release(n2);
		err = -1;
		goto fail2;
	}

	if((err = vfs_lookup_dir(dst, &dn, &dname)))
		goto fail2;

	if(sn->v_mount != dn->v_mount)
	{
		err = -1;
		goto fail3;
	}

	mutex_lock(&n1->v_lock);
	mutex_lock(&sn->v_lock);

	if(dn != sn)
		mutex_lock(&dn->v_lock);

	err = sn->v_mount->m_fs->rename(sn, sname, n1, dn, dname);
	if(err)
		goto fail4;

	err = sn->v_mount->m_fs->sync(sn);
	if(err)
		goto fail4;

	if(dn != sn)
		err = dn->v_mount->m_fs->sync(dn);

fail4:
	if(dn != sn)
		mutex_unlock(&dn->v_lock);
	mutex_unlock(&sn->v_lock);
	mutex_unlock(&n1->v_lock);
fail3:
	vfs_node_release(dn);
fail2:
	vfs_node_release(sn);
fail1:
	vfs_node_release(n1);

	return err;
}

int vfs_unlink(const char * path)
{
	struct vfs_node_t * n, * dn;
	char * name;
	int err;

	if(!path)
		return -1;

	if((err = vfs_node_acquire(path, &n)))
		return err;

	if(n->v_type == VNT_DIR)
	{
		vfs_node_release(n);
		return -1;
	}

	if((n->v_flags == VNF_ROOT) || (atomic_get(&n->v_refcnt) >= 2))
	{
		vfs_node_release(n);
		return -1;
	}

	if((err = vfs_node_access(n, W_OK)))
	{
		vfs_node_release(n);
		return err;
	}

	if((err = vfs_lookup_dir(path, &dn, &name)))
	{
		vfs_node_release(n);
		return err;
	}

	mutex_lock(&n->v_lock);
	err = n->v_mount->m_fs->truncate(n, 0);
	if(err)
		goto fail1;

	err = n->v_mount->m_fs->sync(n);
	if(err)
		goto fail1;

	mutex_lock(&dn->v_lock);
	err = dn->v_mount->m_fs->remove(dn, n, name);
	if(err)
		goto fail2;
	err = dn->v_mount->m_fs->sync(dn);

fail2:
	mutex_unlock(&dn->v_lock);
fail1:
	mutex_unlock(&n->v_lock);
	vfs_node_release(dn);
	vfs_node_release(n);

	return err;
}

int vfs_access(const char * path, u32_t mode)
{
	struct vfs_node_t * n;
	int err;

	if(!path)
		return -1;

	if((err = vfs_node_acquire(path, &n)))
		return err;

	err = vfs_node_access(n, mode);
	vfs_node_release(n);

	return err;
}

int vfs_chmod(const char * path, u32_t mode)
{
	struct vfs_node_t * n;
	int err;

	if(!path)
		return -1;

	if((err = vfs_node_acquire(path, &n)))
		return err;

	mode &= (S_IRWXU | S_IRWXG | S_IRWXO);

	mutex_lock(&n->v_lock);
	err = n->v_mount->m_fs->chmod(n, mode);
	if(err)
		goto fail;
	err = n->v_mount->m_fs->sync(n);

fail:
	mutex_unlock(&n->v_lock);
	vfs_node_release(n);

	return err;
}

int vfs_stat(const char * path, struct vfs_stat_t * st)
{
	struct vfs_node_t * n;
	int err;

	if(!path || !st)
		return -1;

	if((err = vfs_node_acquire(path, &n)))
		return err;
	err = vfs_node_stat(n, st);
	vfs_node_release(n);

	return err;
}

void do_init_vfs(void)
{
	int i;

	init_list_head(&mnt_list);
	mutex_init(&mnt_list_lock);

	for(i = 0; i < VFS_MAX_FD; i++)
	{
		mutex_init(&fd_file[i].f_lock);
		fd_file[i].f_node = NULL;
		fd_file[i].f_offset = 0;
		fd_file[i].f_flags = 0;
	}
	mutex_init(&fd_file_lock);

	for(i = 0; i < VFS_NODE_HASH_SIZE; i++)
	{
		init_list_head(&node_list[i]);
		mutex_init(&node_list_lock[i]);
	}
}
