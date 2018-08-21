/*
 * kernel/fs/vfs/vfs.c
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
#include <fs/fs.h>
#include <fs/vfs/fcntl.h>
#include <fs/vfs/stat.h>
#include <fs/vfs/vfs.h>

/*
 * list for vfs mount points.
 */
extern struct list_head mount_list;

/*
 * system mount
 */
s32_t sys_mount(char * dev, char * dir, char * fsname, u32_t flags)
{
	struct block_t * device;
	struct filesystem_t * fs;
	struct list_head * pos;
	struct mount_t * m;
	struct vnode_t *vp, *vp_covered;
	s32_t err;

	if(!dir || *dir == '\0')
		return ENOENT;

	/* find a file system. */
	if( !(fs = filesystem_search(fsname)) )
		return EACCES;

	/*
	 * null can be specified as a device.
	 */
	if(dev != NULL)
	{
		if( (device = search_block(dev)) == NULL)
			return EACCES;
	}
	else
	{
		device = NULL;
	}

	/*
	 * check if device or directory has already been mounted.
	 */
	list_for_each(pos, &mount_list)
	{
		m = list_entry(pos, struct mount_t, m_link);
		if( !strcmp(m->m_path, dir) )
		{
			return EBUSY;
		}

		if((device != NULL) && (m->m_dev == (void *)device))
		{
			return EBUSY;
		}
	}

	/*
	 * open block device
	 */
	if(device != NULL)
	{
	}

	/*
	 * create vfs mount entry.
	 */
	if( !(m = malloc(sizeof(struct mount_t))) )
	{
		return ENOMEM;
	}

	m->m_fs = fs;
	m->m_flags = flags & MOUNT_MASK;
	m->m_count = 0;
	strlcpy(m->m_path, dir, sizeof(m->m_path));
	m->m_dev = (void *)device;

	/*
	 * get vnode to be covered in the upper file system.
	 */
	if(*dir == '/' && *(dir + 1) == '\0')
	{
		/*
		 * ignore if it mounts to global root directory.
		 */
		vp_covered = NULL;
	}
	else
	{
		if(vfs_namei(dir, &vp_covered) != 0)
		{
			free(m);
			return ENOENT;
		}
		if(vp_covered->v_type != VDIR)
		{
			if(vp_covered)
				vput(vp_covered);
			free(m);
			return ENOTDIR;
		}
	}
	m->m_covered = vp_covered;

	/*
	 * create a root vnode for this file system.
	 */
	if((vp = vget(m, "/")) == NULL)
	{
		if(vp_covered)
			vput(vp_covered);
		free(m);
		return ENOMEM;
	}
	vp->v_type = VDIR;
	vp->v_flags = VROOT;
	if( !S_ISDIR(vp->v_mode) || (vp->v_mode & (S_IRWXU | S_IRWXG | S_IRWXO)) )
		vp->v_mode = S_IFDIR | S_IRWXU | S_IRWXG | S_IRWXO;
	m->m_root = vp;

	/*
	 * call a file system specific routine.
	 */
	err = m->m_fs->vfsops->vfs_mount(m, dev, flags);
	if( err != 0 )
	{
		vput(vp);
		if(vp_covered)
			vput(vp_covered);
		free(m);
		return err;
	}

	if(m->m_flags & MOUNT_RDONLY)
		vp->v_mode &= ~(S_IWUSR|S_IWGRP|S_IWOTH);

	/*
	 * add to mount list
	 */
	list_add(&m->m_link, &mount_list);

	/*
	 * success
	 */
	return 0;
}

/*
 * system umount
 */
s32_t sys_umount(char * path)
{
	struct list_head * pos;
	struct mount_t * m;
	s32_t err;

	/*
	 * check if device or directory has already been mounted.
	 */
	list_for_each(pos, &mount_list)
	{
		m = list_entry(pos, struct mount_t, m_link);
		if( !strcmp(path, m->m_path) )
		{
			/*
			 * root fs can not be unmounted.
			 */
			if(m->m_covered == NULL)
				return EINVAL;

			err = m->m_fs->vfsops->vfs_unmount(m);
			if(err != 0)
				return err;

			list_del(&m->m_link);

			/* decrement referece count of root vnode */
			vrele(m->m_covered);

			/* release all vnodes */
			vflush(m);

			if(m->m_dev)
			{
			}

			free(m);
			return 0;
		}
	}

	return EINVAL;
}

/*
 * system sync
 */
s32_t sys_sync(void)
{
	struct list_head * pos;
	struct mount_t * m;

	/* call each mounted file system. */
	list_for_each(pos, &mount_list)
	{
		m = list_entry(pos, struct mount_t, m_link);
		if(m && m->m_fs->vfsops->vfs_sync)
			m->m_fs->vfsops->vfs_sync(m);
	}

	return 0;
}

/*
 * system open
 */
s32_t sys_open(char * path, u32_t flags, u32_t mode, struct file_t ** pfp)
{
	struct vnode_t *vp, *dvp;
	struct file_t * fp;
	char *filename;
	s32_t err;

	if((flags & O_ACCMODE) == 0)
		return EINVAL;

	if(flags & O_CREAT)
	{
		err = vfs_namei(path, &vp);
		if(err == ENOENT)
		{
			/* create new file. */
			if((err = vfs_lookup(path, &dvp, &filename)) != 0)
				return err;
			if((err = vn_access(dvp, W_OK)) != 0)
			{
				vput(dvp);
				return err;
			}
			mode &= ~S_IFMT;
			mode |= S_IFREG;
			err = dvp->v_op->vop_create(dvp, filename, mode);
			vput(dvp);
			if(err)
				return err;
			if((err = vfs_namei(path, &vp)) != 0)
				return err;
			flags &= ~O_TRUNC;
		}
		else if(err)
		{
			return err;
		}
		else
		{
			/* file already exits */
			if(flags & O_EXCL)
			{
				vput(vp);
				return EEXIST;
			}
			flags &= ~O_CREAT;
		}
	}
	else
	{
		if ((err = vfs_namei(path, &vp)) != 0)
			return err;
	}

	if((flags & O_CREAT) == 0)
	{
		if( (flags & O_WRONLY) || (flags & O_TRUNC) )
		{
			if ((err = vn_access(vp, W_OK)) != 0)
			{
				vput(vp);
				return err;
			}
			if(vp->v_type == VDIR)
			{
				/* open directory with writable. */
				vput(vp);
				return EISDIR;
			}
		}
	}

	/* process truncate request */
	if(flags & O_TRUNC)
	{
		if (!(flags & O_WRONLY) || (vp->v_type == VDIR))
		{
			vput(vp);
			return EINVAL;
		}
		if((err = vp->v_op->vop_truncate(vp, 0)) != 0)
		{
			vput(vp);
			return err;
		}
	}

	/* setup file structure */
	if(!(fp = malloc(sizeof(struct file_t))))
	{
		vput(vp);
		return ENOMEM;
	}

	/* request to file system */
	if((err = vp->v_op->vop_open(vp, flags)) != 0)
	{
		free(fp);
		vput(vp);
		return err;
	}
	memset(fp, 0, sizeof(struct file_t));
	fp->f_vnode = vp;
	fp->f_flags = flags;
	fp->f_offset = 0;
	fp->f_count = 1;
	*pfp = fp;

	return 0;
}

/*
 * system close
 */
s32_t sys_close(struct file_t * fp)
{
	struct vnode_t * vp;
	s32_t err;

	if(fp->f_count <= 0)
		return -1;

	vp = fp->f_vnode;
	if(--fp->f_count > 0)
	{
		vrele(vp);
		return 0;
	}

	if ((err = vp->v_op->vop_close(vp, fp)) != 0)
		return err;

	vput(vp);
	free(fp);

	return 0;
}

/*
 * system read
 */
s32_t sys_read(struct file_t * fp, void * buf, loff_t size, loff_t * count)
{
	struct vnode_t * vp;
	s32_t err;

	if((fp->f_flags & O_RDONLY) == 0)
		return EBADF;

	if(size == 0)
	{
		*count = 0;
		return 0;
	}

	vp = fp->f_vnode;
	err = vp->v_op->vop_read(vp, fp, buf, size, count);

	return err;
}

/*
 * system write
 */
s32_t sys_write(struct file_t * fp, void * buf, loff_t size, loff_t * count)
{
	struct vnode_t * vp;
	s32_t err;

	if((fp->f_flags & O_WRONLY) == 0)
		return EBADF;

	if(size == 0)
	{
		*count = 0;
		return 0;
	}

	vp = fp->f_vnode;
	err = vp->v_op->vop_write(vp, fp, buf, size, count);

	return err;
}

/*
 * system lseek
 */
s32_t sys_lseek(struct file_t * fp, loff_t off, u32_t type, loff_t * origin)
{
	struct vnode_t * vp;

	vp = fp->f_vnode;

	switch(type)
	{
	case VFS_SEEK_SET:
		if(off < 0)
			off = 0;
		if(off > (loff_t)vp->v_size)
			off = vp->v_size;
		break;

	case VFS_SEEK_CUR:
		if(fp->f_offset + off > (loff_t)vp->v_size)
			off = vp->v_size;
		else if(fp->f_offset + off < 0)
			off = 0;
		else
			off = fp->f_offset + off;
		break;

	case VFS_SEEK_END:
		if(off > 0)
			off = vp->v_size;
		else if(vp->v_size + off < 0)
			off = 0;
		else
			off = vp->v_size + off;
		break;

	default:
		return EINVAL;
	}

	/* request to check the file offset */
	if(vp->v_op->vop_seek(vp, fp, fp->f_offset, off) != 0)
		return EINVAL;

	*origin = off;
	fp->f_offset = off;

	return 0;
}

/*
 * system ioctl
 */
s32_t sys_ioctl(struct file_t * fp, int cmd, void * arg)
{
	struct vnode_t * vp;
	s32_t err;

	if((fp->f_flags & O_ACCMODE) == 0)
		return EBADF;

	vp = fp->f_vnode;
	err = vp->v_op->vop_ioctl(vp, fp, cmd, arg);

	return err;
}

/*
 * system fsync
 */
s32_t sys_fsync(struct file_t * fp)
{
	struct vnode_t * vp;
	s32_t err;

	if((fp->f_flags & O_WRONLY) == 0)
		return EBADF;

	vp = fp->f_vnode;

	err = ((vp)->v_op->vop_fsync)(vp, fp);

	return err;
}

/*
 * system fstat
 */
s32_t sys_fstat(struct file_t * fp, struct stat * st)
{
	struct vnode_t * vp;
	s32_t err;

	vp = fp->f_vnode;

	err = vn_stat(vp, st);

	return err;
}

/*
 * system opendir
 */
s32_t sys_opendir(char * path, struct file_t ** file)
{
	struct vnode_t * dvp;
	struct file_t * fp;
	s32_t err;

	if((err = sys_open(path, O_RDONLY, 0, &fp)) != 0)
		return err;

	dvp = fp->f_vnode;
	if(dvp->v_type != VDIR)
	{
		sys_close(fp);
		return ENOTDIR;
	}
	*file = fp;

	return 0;
}

/*
 * system closedir
 */
s32_t sys_closedir(struct file_t * fp)
{
	struct vnode_t * dvp;
	s32_t err;

	dvp = fp->f_vnode;

	if(dvp->v_type != VDIR)
		return EBADF;

	err = sys_close(fp);
	return err;
}

/*
 * system readdir
 */
s32_t sys_readdir(struct file_t * fp, struct dirent_t * dir)
{
	struct vnode_t * dvp;
	s32_t err;

	dvp = fp->f_vnode;

	if(dvp->v_type != VDIR)
		return EBADF;

	err = dvp->v_op->vop_readdir(dvp, fp, dir);

	return err;
}

/*
 * system rewinddir
 */
s32_t sys_rewinddir(struct file_t * fp)
{
	struct vnode_t * dvp;

	dvp = fp->f_vnode;

	if(dvp->v_type != VDIR)
		return EBADF;

	fp->f_offset = 0;

	return 0;
}

/*
 * system seekdir
 */
s32_t sys_seekdir(struct file_t * fp, loff_t loc)
{
	struct vnode_t * dvp;

	dvp = fp->f_vnode;
	if(dvp->v_type != VDIR)
		return EBADF;

	fp->f_offset = loc;

	return 0;
}

/*
 * system telldir
 */
s32_t sys_telldir(struct file_t * fp, loff_t * loc)
{
	struct vnode_t * dvp;

	dvp = fp->f_vnode;

	if(dvp->v_type != VDIR)
		return EBADF;

	*loc = fp->f_offset;

	return 0;
}

/*
 * system mkdir
 */
s32_t sys_mkdir(char * path, u32_t mode)
{
	char * name;
	struct vnode_t *vp, *dvp;
	s32_t err;

	if((err = vfs_namei(path, &vp)) == 0)
	{
		/* file already exists */
		vput(vp);
		return EEXIST;
	}

	/* notice: vp is invalid here */
	if((err = vfs_lookup(path, &dvp, &name)) != 0)
	{
		/* directory already exists */
		return err;
	}

	if((err = vn_access(dvp, W_OK)) != 0)
	{
		vput(dvp);
		return err;
	}

	mode &= ~S_IFMT;
	mode |= S_IFDIR;
	err = dvp->v_op->vop_mkdir(dvp, name, mode);
	vput(dvp);

	return err;
}

/*
 * return 0 if directory is empty
 */
static s32_t check_dir_empty(char * path)
{
	struct file_t * fp;
	struct dirent_t dir;
	s32_t err;

	if((err = sys_opendir(path, &fp)) != 0)
		return err;

	do {
		err = sys_readdir(fp, &dir);
		if(err != 0 && err != EACCES)
			break;
	} while(!strcmp(dir.d_name, ".") || !strcmp(dir.d_name, ".."));

	sys_closedir(fp);

	if(err == ENOENT)
		return 0;
	else if (err == 0)
		return EEXIST;

	return err;
}

/*
 * system rmdir
 */
s32_t sys_rmdir(char * path)
{
	struct vnode_t *vp, *dvp;
	char *name;
	s32_t err;

	if((err = check_dir_empty(path)) != 0)
		return err;
	if((err = vfs_namei(path, &vp)) != 0)
		return err;
	if((err = vn_access(vp, W_OK)) != 0)
	{
		vput(vp);
		return err;
	}
	if(vp->v_type != VDIR)
	{
		vput(vp);
		return ENOTDIR;
	}

	if( (vp->v_flags == VROOT) || (vcount(vp) >= 2) )
	{
		vput(vp);
		return EBUSY;
	}
	if((err = vfs_lookup(path, &dvp, &name)) != 0)
	{
		vput(vp);
		return err;
	}

	err = dvp->v_op->vop_rmdir(dvp, vp, name);
	vgone(vp);
	vput(dvp);

	return err;
}

/*
 * system mknod
 */
s32_t sys_mknod(char * path, u32_t mode)
{
	struct vnode_t *vp, *dvp;
	char * name;
	s32_t err;

	switch(mode & S_IFMT)
	{
	case S_IFDIR:
	case S_IFCHR:
	case S_IFBLK:
	case S_IFREG:
	case S_IFLNK:
	case S_IFIFO:
	case S_IFSOCK:
		break;
	default:
		return EINVAL;
	}

	if((err = vfs_namei(path, &vp)) == 0)
	{
		vput(vp);
		return EEXIST;
	}

	if((err = vfs_lookup(path, &dvp, &name)) != 0)
		return err;

	if((err = vn_access(dvp, W_OK)) != 0)
	{
		vput(dvp);
		return err;
	}

	if(S_ISDIR(mode))
		err = dvp->v_op->vop_mkdir(dvp, name, mode);
	else
		err = dvp->v_op->vop_create(dvp, name, mode);

	vput(dvp);
	return err;
}

/*
 * system rename
 */
s32_t sys_rename(char * src, char * dest)
{
	struct vnode_t *vp1, *vp2 = 0, *dvp1, *dvp2;
	char * sname, * dname;
	char root[] = "/";
	s32_t len;
	s32_t err;

	if((err = vfs_namei(src, &vp1)) != 0)
		return err;
	if((err = vn_access(vp1, W_OK)) != 0)
		goto err1;

	/* if source and dest are the same, do nothing */
	if (!strncmp(src, dest, MAX_PATH))
		goto err1;

	/* check if target is directory of source */
	len = strlen(dest);
	if(!strncmp(src, dest, len))
	{
		err = EINVAL;
		goto err1;
	}

	/* is the source busy ? */
	if (vcount(vp1) >= 2)
	{
		err = EBUSY;
		goto err1;
	}

	/* check type of source & target */
	err = vfs_namei(dest, &vp2);
	if(err == 0)
	{
		/* target exists */
		if(vp1->v_type == VDIR && vp2->v_type != VDIR)
		{
			err = ENOTDIR;
			goto err2;
		}
		else if(vp1->v_type != VDIR && vp2->v_type == VDIR)
		{
			err = EISDIR;
			goto err2;
		}
		if(vp2->v_type == VDIR && check_dir_empty(dest))
		{
			err = EEXIST;
			goto err2;
		}

		if(vcount(vp2) >= 2)
		{
			err = EBUSY;
			goto err2;
		}
	}

	dname = (char *)strrchr(dest, '/');
	if(dname == NULL)
	{
		err = ENOTDIR;
		goto err2;
	}
	if(dname == dest)
		dest = root;

	*dname = 0;
	dname++;

	if((err = vfs_lookup(src, &dvp1, &sname)) != 0)
		goto err2;

	if((err = vfs_namei(dest, &dvp2)) != 0)
		goto err3;

	/* the source and dest must be same file system */
	if(dvp1->v_mount != dvp2->v_mount)
	{
		err = EIO;
		goto err4;
	}

	err = dvp1->v_op->vop_rename(dvp1, vp1, sname, dvp2, vp2, dname);

 err4:
	vput(dvp2);
 err3:
	vput(dvp1);
 err2:
	if(vp2)
		vput(vp2);
 err1:
	vput(vp1);

	return err;
}

/*
 * system unlink
 */
s32_t sys_unlink(char * path)
{
	char *name;
	struct vnode_t *vp, *dvp;
	s32_t err;

	if((err = vfs_namei(path, &vp)) != 0)
		return err;

	if((err = vn_access(vp, W_OK)) != 0)
	{
		vput(vp);
		return err;
	}

	if(vp->v_type == VDIR)
	{
		vput(vp);
		return EBUSY;
	}

	if( (vp->v_flags == VROOT) || (vcount(vp) >= 2) )
	{
		vput(vp);
		return EBUSY;
	}

	if((err = vfs_lookup(path, &dvp, &name)) != 0)
	{
		vput(vp);
		return err;
	}

	err = dvp->v_op->vop_remove(dvp, vp, name);
	vgone(vp);
	vput(dvp);

	return err;
}

/*
 * system access
 */
s32_t sys_access(char * path, u32_t mode)
{
	struct vnode_t * vp;
	s32_t err;

	if((err = vfs_namei(path, &vp)) != 0)
		return err;

	err = vn_access(vp, mode);
	vput(vp);

	return err;
}

/*
 * system stat
 */
s32_t sys_stat(char * path, struct stat * st)
{
	struct vnode_t * vp;
	s32_t err;

	if((err = vfs_namei(path, &vp)) != 0)
		return err;

	err = vn_stat(vp, st);
	vput(vp);

	return err;
}

/*
 * system truncate
 */
s32_t sys_truncate(char * path, loff_t length)
{
	return 0;
}

/*
 * system ftruncate
 */
s32_t sys_ftruncate(struct file_t * fp, loff_t length)
{
	return 0;
}

/*
 * system fchdir
 */
s32_t sys_fchdir(struct file_t * fp, char * cwd)
{
	struct vnode_t * dvp;

	dvp = fp->f_vnode;

	if(dvp->v_type != VDIR)
		return EBADF;

	strlcpy(cwd, dvp->v_path, MAX_PATH);

	return 0;
}

void do_init_vfs(void)
{
	extern void vfs_vnode_init(void);
	extern void vfs_fd_init(void);
	vfs_vnode_init();
	vfs_fd_init();
}
