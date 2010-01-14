/*
 * kernel/fs/vfs/vfs.c
 *
 * Copyright (c) 2007-2010 jianjun jiang <jerryjianjun@gmail.com>
 * website: http://xboot.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <configs.h>
#include <default.h>
#include <error.h>
#include <malloc.h>
#include <xboot/list.h>
#include <xboot/panic.h>
#include <xboot/blkdev.h>
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
x_s32 sys_mount(char * dev, char * dir, char * fsname, x_u32 flags)
{
	struct blkdev * device;
	struct filesystem * fs;
	struct list_head * pos;
	struct mount * m;
	struct vnode *vp, *vp_covered;
	x_s8 * p;
	x_s32 err;

	if(!dir || *dir == '\0')
		return ENOENT;

	/* find a file system. */
	if( !(fs = filesystem_search(fsname)) )
		return ENODEV;

	/*
	 * null can be specified as a device.
	 */
	if(dev != NULL)
	{
		/* search for the last directory separator in dev */
		if( (p = strrchr((const x_s8 *)dev, '/')) != NULL )
			p = p+1;
		else
			p = (x_s8 *)dev;

		if( (device = search_blkdev((const char *)p)) == NULL)
			return ENODEV;
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
		m = list_entry(pos, struct mount, m_link);
		if( !strcmp((const x_s8 *)m->m_path, (const x_s8 *)dir) )
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
		if( device->open(device) != 0 )
			return EINTR;
	}

	/*
	 * create vfs mount entry.
	 */
	if( !(m = malloc(sizeof(struct mount))) )
	{
		if(device != NULL)
			device->close(device);
		return ENOMEM;
	}

	m->m_fs = fs;
	m->m_flags = flags & MOUNT_MASK;
	m->m_count = 0;
	strlcpy((x_s8 *)m->m_path, (const x_s8 *)dir, sizeof(m->m_path));
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
		if(namei(dir, &vp_covered) != 0)
		{
			if(device != NULL)
				device->close(device);
			free(m);
			return ENOENT;
		}
		if(vp_covered->v_type != VDIR)
		{
			if(vp_covered)
				vput(vp_covered);
			if(device != NULL)
				device->close(device);
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
		if(device != NULL)
			device->close(device);
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
		if(device != NULL)
			device->close(device);
		free(m);
		return err;
	}

	if(m->m_flags & MOUNT_RDONLY)
		vp->v_mode &= ~(S_IWUSR|S_IWGRP|S_IWOTH);

	/*
	 * keep reference count for root or covered vnode.
	 */
	vn_unlock(vp);
	if(vp_covered)
		vn_unlock(vp_covered);

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
x_s32 sys_umount(char * path)
{
	struct blkdev * device;
	struct list_head * pos;
	struct mount * m;
	x_s32 err;

	/*
	 * check if device or directory has already been mounted.
	 */
	list_for_each(pos, &mount_list)
	{
		m = list_entry(pos, struct mount, m_link);
		if( !strcmp((const x_s8 *)path, (const x_s8 *)m->m_path) )
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
				device = (struct blkdev *)(m->m_dev);
				device->close(device);
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
x_s32 sys_sync(void)
{
	struct list_head * pos;
	struct mount * m;

	/* call each mounted file system. */
	list_for_each(pos, &mount_list)
	{
		m = list_entry(pos, struct mount, m_link);
		if(m && m->m_fs->vfsops->vfs_sync)
			m->m_fs->vfsops->vfs_sync(m);
	}

	return 0;
}

/*
 * system open
 */
x_s32 sys_open(char * path, x_u32 flags, x_u32 mode, struct file ** pfp)
{
	struct vnode *vp, *dvp;
	struct file * fp;
	char *filename;
	x_s32 err;

	if((flags & O_ACCMODE) == 0)
		return EINVAL;

	if(flags & O_CREAT)
	{
		err = namei(path, &vp);
		if(err == ENOENT)
		{
			/* create new file. */
			if((err = lookup(path, &dvp, &filename)) != 0)
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
			if((err = namei(path, &vp)) != 0)
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
		if ((err = namei(path, &vp)) != 0)
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
	if(!(fp = malloc(sizeof(struct file))))
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
	memset(fp, 0, sizeof(struct file));
	fp->f_vnode = vp;
	fp->f_flags = flags;
	fp->f_offset = 0;
	fp->f_count = 1;
	*pfp = fp;
	vn_unlock(vp);

	return 0;
}

/*
 * system close
 */
x_s32 sys_close(struct file * fp)
{
	struct vnode * vp;
	x_s32 err;

	if(fp->f_count <= 0)
		panic("sys_close");

	vp = fp->f_vnode;
	if(--fp->f_count > 0)
	{
		vrele(vp);
		return 0;
	}

	vn_lock(vp);
	if ((err = vp->v_op->vop_close(vp, fp)) != 0)
	{
		vn_unlock(vp);
		return err;
	}
	vput(vp);
	free(fp);

	return 0;
}

/*
 * system read
 */
x_s32 sys_read(struct file * fp, void * buf, x_size size, x_size * count)
{
	struct vnode * vp;
	x_s32 err;

	if((fp->f_flags & O_RDONLY) == 0)
		return EBADF;

	if(size == 0)
	{
		*count = 0;
		return 0;
	}

	vp = fp->f_vnode;
	vn_lock(vp);
	err = vp->v_op->vop_read(vp, fp, buf, size, count);
	vn_unlock(vp);

	return err;
}

/*
 * system write
 */
x_s32 sys_write(struct file * fp, void * buf, x_size size, x_size * count)
{
	struct vnode * vp;
	x_s32 err;

	if((fp->f_flags & O_WRONLY) == 0)
		return EBADF;

	if(size == 0)
	{
		*count = 0;
		return 0;
	}

	vp = fp->f_vnode;
	vn_lock(vp);
	err = vp->v_op->vop_write(vp, fp, buf, size, count);
	vn_unlock(vp);

	return err;
}

/*
 * system lseek
 */
x_s32 sys_lseek(struct file * fp, x_off off, x_u32 type, x_off * origin)
{
	struct vnode * vp;

	vp = fp->f_vnode;
	vn_lock(vp);

	switch(type)
	{
	case SEEK_SET:
		if(off < 0)
			off = 0;
		if(off > (x_off)vp->v_size)
			off = vp->v_size;
		break;

	case SEEK_CUR:
		if(fp->f_offset + off > (x_off)vp->v_size)
			off = vp->v_size;
		else if(fp->f_offset + off < 0)
			off = 0;
		else
			off = fp->f_offset + off;
		break;

	case SEEK_END:
		if(off > 0)
			off = vp->v_size;
		else if(vp->v_size + off < 0)
			off = 0;
		else
			off = vp->v_size + off;
		break;

	default:
		vn_unlock(vp);
		return EINVAL;
	}

	/* request to check the file offset */
	if(vp->v_op->vop_seek(vp, fp, fp->f_offset, off) != 0)
	{
		vn_unlock(vp);
		return EINVAL;
	}

	*origin = off;
	fp->f_offset = off;
	vn_unlock(vp);

	return 0;
}

/*
 * system ioctl
 */
x_s32 sys_ioctl(struct file * fp, x_u32 cmd, void * arg)
{
	struct vnode * vp;
	x_s32 err;

	if((fp->f_flags & O_ACCMODE) == 0)
		return EBADF;

	vp = fp->f_vnode;
	vn_lock(vp);
	err = vp->v_op->vop_ioctl(vp, fp, cmd, arg);
	vn_unlock(vp);

	return err;
}

/*
 * system fsync
 */
x_s32 sys_fsync(struct file * fp)
{
	struct vnode * vp;
	x_s32 err;

	if((fp->f_flags & O_WRONLY) == 0)
		return EBADF;

	vp = fp->f_vnode;

	vn_lock(vp);
	err = ((vp)->v_op->vop_fsync)(vp, fp);
	vn_unlock(vp);

	return err;
}

/*
 * system fstat
 */
x_s32 sys_fstat(struct file * fp, struct stat * st)
{
	struct vnode * vp;
	x_s32 err;

	vp = fp->f_vnode;

	vn_lock(vp);
	err = vn_stat(vp, st);
	vn_unlock(vp);

	return err;
}

/*
 * system opendir
 */
x_s32 sys_opendir(char * path, struct file ** file)
{
	struct vnode * dvp;
	struct file * fp;
	x_s32 err;

	if((err = sys_open(path, O_RDONLY, 0, &fp)) != 0)
		return err;

	dvp = fp->f_vnode;
	vn_lock(dvp);
	if(dvp->v_type != VDIR)
	{
		vn_unlock(dvp);
		sys_close(fp);
		return ENOTDIR;
	}
	vn_unlock(dvp);
	*file = fp;

	return 0;
}

/*
 * system closedir
 */
x_s32 sys_closedir(struct file * fp)
{
	struct vnode * dvp;
	x_s32 err;

	dvp = fp->f_vnode;

	vn_lock(dvp);
	if(dvp->v_type != VDIR)
	{
		vn_unlock(dvp);
		return EBADF;
	}
	vn_unlock(dvp);

	err = sys_close(fp);
	return err;
}

/*
 * system readdir
 */
x_s32 sys_readdir(struct file * fp, struct dirent * dir)
{
	struct vnode * dvp;
	x_s32 err;

	dvp = fp->f_vnode;

	vn_lock(dvp);
	if(dvp->v_type != VDIR)
	{
		vn_unlock(dvp);
		return EBADF;
	}
	err = dvp->v_op->vop_readdir(dvp, fp, dir);
	vn_unlock(dvp);

	return err;
}

/*
 * system rewinddir
 */
x_s32 sys_rewinddir(struct file * fp)
{
	struct vnode * dvp;

	dvp = fp->f_vnode;

	vn_lock(dvp);
	if(dvp->v_type != VDIR)
	{
		vn_unlock(dvp);
		return EBADF;
	}
	fp->f_offset = 0;
	vn_unlock(dvp);

	return 0;
}

/*
 * system seekdir
 */
x_s32 sys_seekdir(struct file * fp, x_off loc)
{
	struct vnode * dvp;

	dvp = fp->f_vnode;
	vn_lock(dvp);
	if(dvp->v_type != VDIR)
	{
		vn_unlock(dvp);
		return EBADF;
	}
	fp->f_offset = loc;
	vn_unlock(dvp);

	return 0;
}

/*
 * system telldir
 */
x_s32 sys_telldir(struct file * fp, x_off * loc)
{
	struct vnode * dvp;

	dvp = fp->f_vnode;

	vn_lock(dvp);
	if(dvp->v_type != VDIR)
	{
		vn_unlock(dvp);
		return EBADF;
	}
	*loc = fp->f_offset;
	vn_unlock(dvp);

	return 0;
}

/*
 * system mkdir
 */
x_s32 sys_mkdir(char * path, x_u32 mode)
{
	char * name;
	struct vnode *vp, *dvp;
	x_s32 err;

	if((err = namei(path, &vp)) == 0)
	{
		/* file already exists */
		vput(vp);
		return EEXIST;
	}

	/* notice: vp is invalid here */
	if((err = lookup(path, &dvp, &name)) != 0)
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
static x_s32 check_dir_empty(char * path)
{
	struct file * fp;
	struct dirent dir;
	x_s32 err;

	if((err = sys_opendir(path, &fp)) != 0)
		return err;

	do {
		err = sys_readdir(fp, &dir);
		if(err != 0 && err != EACCES)
			break;
	} while(!strcmp((const x_s8 *)dir.d_name, (const x_s8 *)".") || !strcmp((const x_s8 *)dir.d_name, (const x_s8 *)".."));

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
x_s32 sys_rmdir(char * path)
{
	struct vnode *vp, *dvp;
	char *name;
	x_s32 err;

	if((err = check_dir_empty(path)) != 0)
		return err;
	if((err = namei(path, &vp)) != 0)
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
	if((err = lookup(path, &dvp, &name)) != 0)
	{
		vput(vp);
		return err;
	}

	err = dvp->v_op->vop_rmdir(dvp, vp, name);
	vn_unlock(vp);
	vgone(vp);
	vput(dvp);

	return err;
}

/*
 * system mknod
 */
x_s32 sys_mknod(char * path, x_u32 mode)
{
	struct vnode *vp, *dvp;
	char * name;
	x_s32 err;

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

	if((err = namei(path, &vp)) == 0)
	{
		vput(vp);
		return EEXIST;
	}

	if((err = lookup(path, &dvp, &name)) != 0)
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
x_s32 sys_rename(char * src, char * dest)
{
	struct vnode *vp1, *vp2 = 0, *dvp1, *dvp2;
	char * sname, * dname;
	char root[] = "/";
	x_s32 len;
	x_s32 err;

	if((err = namei(src, &vp1)) != 0)
		return err;
	if((err = vn_access(vp1, W_OK)) != 0)
		goto err1;

	/* if source and dest are the same, do nothing */
	if (!strncmp((const x_s8 *)src, (const x_s8 *)dest, MAX_PATH))
		goto err1;

	/* check if target is directory of source */
	len = strlen((const x_s8 *)dest);
	if(!strncmp((const x_s8 *)src, (const x_s8 *)dest, len))
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
	err = namei(dest, &vp2);
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

	dname = (char *)strrchr((const x_s8 *)dest, '/');
	if(dname == NULL)
	{
		err = ENOTDIR;
		goto err2;
	}
	if(dname == dest)
		dest = root;

	*dname = 0;
	dname++;

	if((err = lookup(src, &dvp1, &sname)) != 0)
		goto err2;

	if((err = namei(dest, &dvp2)) != 0)
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
x_s32 sys_unlink(char * path)
{
	char *name;
	struct vnode *vp, *dvp;
	x_s32 err;

	if((err = namei(path, &vp)) != 0)
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

	if((err = lookup(path, &dvp, &name)) != 0)
	{
		vput(vp);
		return err;
	}

	err = dvp->v_op->vop_remove(dvp, vp, name);
	vn_unlock(vp);
	vgone(vp);
	vput(dvp);

	return err;
}

/*
 * system access
 */
x_s32 sys_access(char * path, x_u32 mode)
{
	struct vnode * vp;
	x_s32 err;

	if((err = namei(path, &vp)) != 0)
		return err;

	err = vn_access(vp, mode);
	vput(vp);

	return err;
}

/*
 * system stat
 */
x_s32 sys_stat(char * path, struct stat * st)
{
	struct vnode * vp;
	x_s32 err;

	if((err = namei(path, &vp)) != 0)
		return err;

	err = vn_stat(vp, st);
	vput(vp);

	return err;
}

/*
 * system truncate
 */
x_s32 sys_truncate(char * path, x_off length)
{
	return 0;
}

/*
 * system ftruncate
 */
x_s32 sys_ftruncate(struct file * fp, x_off length)
{
	return 0;
}

/*
 * system fchdir
 */
x_s32 sys_fchdir(struct file * fp, char * cwd)
{
	struct vnode * dvp;

	dvp = fp->f_vnode;

	vn_lock(dvp);
	if(dvp->v_type != VDIR)
	{
		vn_unlock(dvp);
		return EBADF;
	}
	strlcpy((x_s8 *)cwd, (const x_s8 *)dvp->v_path, MAX_PATH);
	vn_unlock(dvp);

	return 0;
}
