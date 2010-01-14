#ifndef __VFS_H__
#define __VFS_H__

#include <configs.h>
#include <default.h>
#include <xboot/list.h>
#include <fs/vfs/stat.h>
#include <fs/vfs/fcntl.h>
#include <fs/fs.h>

#define MAX_PATH			(256)
#define	MAX_NAME			(64)

/*
 * declare structure
 */
struct file;
struct dirent;
struct vnode;
struct vnops;
struct mount;
struct vfsops;

/*
 * file structure
 */
struct file {
	x_u32 f_flags;				/* open flag */
	x_s32 f_count;				/* reference count */
	x_off f_offset;				/* current position in file */
	struct vnode * f_vnode;		/* vnode */
};

/*
 * dirent types.
 */
enum dirent_type {
	DT_UNKNOWN,
	DT_DIR,
	DT_REG,
	DT_BLK,
	DT_CHR,
	DT_FIFO,
	DT_LNK,
	DT_SOCK,
	DT_WHT,
};

/*
 * the dirent structure defines the format of directory entries.
 */
struct dirent {
	x_u32 d_fileno;				/* file number of entry */
	x_u16 d_reclen;				/* length of this record */
	x_u16 d_namlen;				/* length of string in d_name */
	enum dirent_type d_type; 	/* file type, see below */
	char d_name[MAX_NAME];		/* name must be no longer than this */
};

/*
 * vnode types.
 */
enum vnode_type {
	VREG,	   					/* regular file  */
	VDIR,	    				/* directory */
	VBLK,	    				/* block device */
	VCHR,	    				/* character device */
	VLNK,	    				/* symbolic link */
	VSOCK,	    				/* socks */
	VFIFO,	    				/* fifo */
};

/*
 * vnode flags.
 */
enum vnode_flag {
	VNONE,						/* default vnode flag */
	VROOT,	   					/* root of its file system */
};

/*
 * vnode attribute
 */
struct vattr {
	enum vnode_type	va_type;	/* vnode type */
	x_u32 va_mode;				/* file access mode */
};

/*
 * vnode structure
 */
struct vnode {
	struct list_head v_link;	/* link for hash list */
	struct mount * v_mount;		/* mounted vfs pointer */
	struct vnops * v_op;		/* vnode operations */
	x_size v_size;				/* file size */
	x_u32 v_mode;				/* file mode permissions */
	enum vnode_type v_type;		/* vnode type */
	enum vnode_flag v_flags;	/* vnode flag */
	x_s32 v_refcnt;				/* reference count */
	x_s32 v_nrlocks;			/* lock count (for debug) */
	x_s32 v_blkno;				/* block number */
	char * v_path;				/* pointer to path in fs */
	void * v_data;				/* private data for fs */
};

/*
 * vnode operations
 */
struct vnops {
	x_s32 (*vop_open)(struct vnode *, x_s32);
	x_s32 (*vop_close)(struct vnode *, struct file *);
	x_s32 (*vop_read)(struct vnode *, struct file *, void *, x_size, x_size *);
	x_s32 (*vop_write)(struct vnode *, struct file *, void *, x_size, x_size *);
	x_s32 (*vop_seek)(struct vnode *, struct file *, x_off, x_off);
	x_s32 (*vop_ioctl)(struct vnode *, struct file *, x_u32, void *);
	x_s32 (*vop_fsync)(struct vnode *, struct file *);
	x_s32 (*vop_readdir)(struct vnode *, struct file *, struct dirent *);
	x_s32 (*vop_lookup)(struct vnode *, char *, struct vnode *);
	x_s32 (*vop_create)(struct vnode *, char *, x_u32);
	x_s32 (*vop_remove)(struct vnode *, struct vnode *, char *);
	x_s32 (*vop_rename)(struct vnode *, struct vnode *, char *, struct vnode *, struct vnode *, char *);
	x_s32 (*vop_mkdir)(struct vnode *, char *, x_u32);
	x_s32 (*vop_rmdir)(struct vnode *, struct vnode *, char *);
	x_s32 (*vop_getattr)(struct vnode *, struct vattr *);
	x_s32 (*vop_setattr)(struct vnode *, struct vattr *);
	x_s32 (*vop_inactive)(struct vnode *);
	x_s32 (*vop_truncate)(struct vnode *, x_off);
};

/*
 * file system id type
 */
struct fsid {
	x_s32 val[2];
};

/*
 * directory description
 */
struct dir {
	x_s32 fd;
	struct dirent entry;
};

/*
 * file system statistics
 */
struct statfs {
	x_s16 f_type;				/* filesystem type number */
	x_s16 f_flags;				/* copy of mount flags */
	x_s32 f_bsize;				/* fundamental file system block size */
	x_s32 f_blocks;				/* total data blocks in file system */
	x_s32 f_bfree;				/* free blocks in fs */
	x_s32 f_bavail;				/* free blocks avail to non-superuser */
	x_s32 f_files;				/* total file nodes in file system */
	x_s32 f_ffree;				/* free file nodes in fs */
	struct fsid f_fsid;			/* file system id */
	x_s32 f_namelen;			/* maximum filename length */
};

/*
 * mount flags.
 */
#define	MOUNT_RDONLY			(0x00000001)	/* read only filesystem */
#define	MOUNT_NODEV				(0x00000002)	/* don't interpret special files */
#define	MOUNT_LOOP				(0x00000004)	/* mount loop device */
#define	MOUNT_MASK				(0x00000007)	/* mount flag mask value */

/*
 * mount data
 */
struct mount {
	struct list_head m_link;	/* link to next mount point */
	struct vfsops * m_op;		/* pointer to vfs operation */
	x_u32 m_flags;				/* mount flag */
	x_s32 m_count;				/* reference count */
	char m_path[MAX_PATH];		/* mounted path */
	void * m_dev;				/* mounted device */
	struct vnode * m_root;		/* root vnode */
	struct vnode * m_covered;	/* vnode covered on parent fs */
	void * m_data;				/* private data for fs */
};

/*
 * operations supported on virtual file system.
 */
struct vfsops {
	x_s32(*vfs_mount)(struct mount *, char *, x_s32);
	x_s32(*vfs_unmount)(struct mount *);
	x_s32(*vfs_sync)(struct mount *);
	x_s32(*vfs_vget)(struct mount *, struct vnode *);
	x_s32(*vfs_statfs)(struct mount *, struct statfs *);
	struct vnops * vfs_vnops;
};


/*
 * declare for vfs_mount
 */
void vfs_busy(struct mount * m);
void vfs_unbusy(struct mount * m);
x_s32 vfs_findroot(char * path, struct mount ** mp, char ** root);
x_s32 lookup(char * path, struct vnode ** vpp, char ** name);


/*
 * declare for vfs_vnode
 */
struct vnode * vn_lookup(struct mount * mp, char * path);
struct vnode * vget(struct mount * mp, char * path);
void vput(struct vnode * vp);
void vn_lock(struct vnode * vp);
void vn_unlock(struct vnode * vp);
x_s32 vcount(struct vnode * vp);
void vref(struct vnode * vp);
void vrele(struct vnode * vp);
void vgone(struct vnode * vp);
void vflush(struct mount * mp);
x_s32 vn_stat(struct vnode * vp, struct stat * st);
x_s32 vn_access(struct vnode * vp, x_u32 mode);


/*
 * declare for vfs_path
 */
x_s32 fd_alloc(x_s32 low);
x_s32 fd_free(x_s32 fd);
struct file * get_fp(x_s32 fd);
x_s32 set_fp(x_s32 fd, struct file *fp);
x_s32 vfs_path_conv(const char * path, char * full);
void vfs_setcwd(const char * path);
char * vfs_getcwd(char * buf, x_s32 size);
void vfs_setcwdfp(struct file * fp);
struct file * vfs_getcwdfp(void);


/*
 * declare for vfs_lookup
 */
x_s32 namei(char * path, struct vnode ** vpp);
x_s32 lookup(char * path, struct vnode ** vpp, char ** name);


/*
 * declare for vfs syscall
 */
x_s32 sys_mount(char * dev, char * dir, char * fsname, x_u32 flags);
x_s32 sys_umount(char * path);
x_s32 sys_sync(void);
x_s32 sys_open(char * path, x_u32 flags, x_u32 mode, struct file ** pfp);
x_s32 sys_close(struct file * fp);
x_s32 sys_read(struct file * fp, void * buf, x_size size, x_size * count);
x_s32 sys_write(struct file * fp, void * buf, x_size size, x_size * count);
x_s32 sys_lseek(struct file * fp, x_off off, x_u32 type, x_off * origin);
x_s32 sys_ioctl(struct file * fp, x_u32 cmd, void * arg);
x_s32 sys_fsync(struct file * fp);
x_s32 sys_fstat(struct file * fp, struct stat * st);
x_s32 sys_opendir(char * path, struct file ** file);
x_s32 sys_closedir(struct file * fp);
x_s32 sys_readdir(struct file * fp, struct dirent * dir);
x_s32 sys_rewinddir(struct file * fp);
x_s32 sys_seekdir(struct file * fp, x_off loc);
x_s32 sys_telldir(struct file * fp, x_off * loc);
x_s32 sys_mkdir(char * path, x_u32 mode);
x_s32 sys_rmdir(char * path);
x_s32 sys_mknod(char * path, x_u32 mode);
x_s32 sys_rename(char * src, char * dest);
x_s32 sys_unlink(char * path);
x_s32 sys_access(char * path, x_u32 mode);
x_s32 sys_stat(char * path, struct stat * st);
x_s32 sys_truncate(char * path, x_off length);
x_s32 sys_ftruncate(struct file * fp, x_off length);
x_s32 sys_fchdir(struct file * fp, char * cwd);

#endif /* __VFS_H__ */
