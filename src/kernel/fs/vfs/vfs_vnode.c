/*
 * kernel/fs/vfs/vfs_vnode.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
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

#include <xboot.h>
#include <errno.h>
#include <malloc.h>
#include <xboot/list.h>
#include <xboot/initcall.h>
#include <fs/fs.h>
#include <fs/vfs/fcntl.h>
#include <fs/vfs/stat.h>
#include <fs/vfs/vfs.h>

/* size of vnode hash table, must power 2 */
#define VNODE_HASH_SIZE				(32)

/*
 * vnode hash table.
 *
 * all opened vnodes are stored on this hash table.
 * they can be accessed by its path name.
 */
static struct list_head vnode_table[VNODE_HASH_SIZE];

/*
 * get the hash value from the mount point and path name.
 */
static u32_t vn_hash(struct mount * mp, char * path)
{
	u32_t val = 0;

	if(path)
	{
		while(*path)
			val = ((val << 5) + val) + *path++;
	}

	return (val ^ (u32_t)mp) & (VNODE_HASH_SIZE - 1);
}

/*
 * returns locked vnode for specified mount point and path.
 */
struct vnode * vn_lookup(struct mount * mp, char * path)
{
	struct list_head * pos;
	struct list_head * head;
	struct vnode * vp;

	head = &vnode_table[vn_hash(mp, path)];
	list_for_each(pos, head)
	{
		vp = list_entry(pos, struct vnode, v_link);
		if( (vp->v_mount == mp) && (!strncmp(vp->v_path, path, MAX_PATH)) )
		{
			vp->v_refcnt++;
			return vp;
		}
	}

	return NULL;
}

/*
 * allocate new vnode for specified path.
 */
struct vnode * vget(struct mount * mp, char * path)
{
	struct vnode * vp;
	s32_t len;

	if( !(vp = malloc(sizeof(struct vnode))) )
		return NULL;
	memset(vp, 0, sizeof(struct vnode));

	len = strlen(path) + 1;
	if( !(vp->v_path = malloc(len)) )
	{
		free(vp);
		return NULL;
	}

	vp->v_mount = mp;
	vp->v_op = mp->m_fs->vfsops->vfs_vnops;
	vp->v_refcnt = 1;
	strlcpy(vp->v_path, path, len);

	/*
	 * request to allocate fs specific data for vnode.
	 */
	if( mp->m_fs->vfsops->vfs_vget(mp, vp) != 0 )
	{
		free(vp->v_path);
		free(vp);

		return NULL;
	}

	vfs_busy(vp->v_mount);

	list_add(&vp->v_link, &vnode_table[vn_hash(mp, path)]);

	return vp;
}

/*
 * unlock vnode and decrement its reference count.
 */
void vput(struct vnode * vp)
{
	vp->v_refcnt--;

	if(vp->v_refcnt > 0)
		return;

	list_del(&vp->v_link);

	/*
	 * deallocate fs specific vnode data
	 */
	vp->v_op->vop_inactive(vp);
	vfs_unbusy(vp->v_mount);

	free(vp->v_path);
	free(vp);
}

/*
 * return reference count.
 */
s32_t vcount(struct vnode * vp)
{
	s32_t count;

	count = vp->v_refcnt;

	return count;
}

/*
 * increment the reference count on an active vnode.
 */
void vref(struct vnode * vp)
{
	vp->v_refcnt++;
}

/*
 * decrement the reference count of the vnode.
 */
void vrele(struct vnode * vp)
{
	vp->v_refcnt--;

	if(vp->v_refcnt > 0)
		return;

	list_del(&vp->v_link);

	/*
	 * deallocate fs specific vnode data
	 */
	vp->v_op->vop_inactive(vp);
	vfs_unbusy(vp->v_mount);

	free(vp->v_path);
	free(vp);
}

/*
 * vgone() is called when unlocked vnode is no longer valid.
 */
void vgone(struct vnode * vp)
{
	list_del(&vp->v_link);
	vfs_unbusy(vp->v_mount);

	free(vp->v_path);
	free(vp);
}

/*
 * remove all vnode in the vnode table for unmount.
 */
void vflush(struct mount * mp)
{
	struct list_head * pos;
	struct vnode * vp;
	s32_t i;

	for(i = 0; i < VNODE_HASH_SIZE; i++)
	{
		list_for_each(pos, &vnode_table[i])
		{
			vp = list_entry(pos, struct vnode, v_link);
			if(vp->v_mount == mp)
			{
				/* TODO */
			}
		}
	}
}

/*
 * get stat on vnode pointer.
 */
s32_t vn_stat(struct vnode * vp, struct stat * st)
{
	u32_t mode;

	memset(st, 0, sizeof(struct stat));

	st->st_ino = (u32_t)vp;
	st->st_size = vp->v_size;

	mode = vp->v_mode & (S_IRWXU|S_IRWXG|S_IRWXO);
	switch (vp->v_type)
	{
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
		return EBADF;
	};
	st->st_mode = mode;

	if(vp->v_type == VCHR || vp->v_type == VBLK)
		st->st_dev = (u32_t)vp->v_data;

	st->st_uid = 0;
	st->st_gid = 0;

	st->st_ctime = 0;
	st->st_atime = 0;
	st->st_mtime = 0;

	return 0;
}

/*
 * check permission on vnode pointer.
 */
s32_t vn_access(struct vnode * vp, u32_t mode)
{
	if((mode & R_OK) && (vp->v_mode & (S_IRUSR|S_IRGRP|S_IROTH)) == 0)
	{
		return EACCES;
	}

	if(mode & W_OK)
	{
		if(vp->v_mount->m_flags & MOUNT_RDONLY)
		{
			return EACCES;
		}

		if((vp->v_mode & (S_IWUSR|S_IWGRP|S_IWOTH)) == 0)
		{
			return EACCES;
		}
	}

	if((mode & X_OK) && (vp->v_mode & (S_IXUSR|S_IXGRP|S_IXOTH)) == 0)
	{
		return EACCES;
	}

	return 0;
}

/*
 * vnode pure init
 */
static __init void vnode_pure_init(void)
{
	s32_t i;

    for( i = 0; i < VNODE_HASH_SIZE; i++ )
    	init_list_head(&vnode_table[i]);
}
pure_initcall(vnode_pure_init);
