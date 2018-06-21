#ifndef __VFS_H__
#define __VFS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <block/block.h>
#include <fs/vfs/stat.h>
#include <fs/vfs/fcntl.h>
#include <fs/fs.h>

#define MAX_PATH			(256)
#define	MAX_NAME			(64)

/*
 * declare structure
 */
struct file_t;
struct dirent_t;
struct vnode_t;
struct vnops_t;
struct mount_t;
struct vfsops_t;

/*
 * file structure
 */
struct file_t {
	u32_t f_flags;				/* open flag */
	s32_t f_count;				/* reference count */
	loff_t f_offset;			/* current position in file */
	struct vnode_t * f_vnode;	/* vnode */
};

/*
 * dirent types.
 */
enum dirent_type_t {
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
struct dirent_t {
	u32_t d_fileno;				/* file number of entry */
	u16_t d_reclen;				/* length of this record */
	u16_t d_namlen;				/* length of string in d_name */
	enum dirent_type_t d_type; 	/* file type, see below */
	char d_name[MAX_NAME];		/* name must be no longer than this */
};

/*
 * vnode types.
 */
enum vnode_type_t {
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
enum vnode_flag_t {
	VNONE,						/* default vnode flag */
	VROOT,	   					/* root of its file system */
};

/*
 * vnode attribute
 */
struct vattr_t {
	enum vnode_type_t va_type;	/* vnode type */
	u32_t va_mode;				/* file access mode */
};

/*
 * vnode structure
 */
struct vnode_t {
	struct list_head v_link;	/* link for hash list */
	struct mount_t * v_mount;	/* mounted vfs pointer */
	struct vnops_t * v_op;		/* vnode operations */
	loff_t v_size;				/* file size */
	u32_t v_mode;				/* file mode permissions */
	enum vnode_type_t v_type;	/* vnode type */
	enum vnode_flag_t v_flags;	/* vnode flag */
	s32_t v_refcnt;				/* reference count */
	u32_t v_blkno;				/* block number */
	char * v_path;				/* pointer to path in fs */
	void * v_data;				/* private data for fs */
};

/*
 * vnode operations
 */
struct vnops_t {
	s32_t (*vop_open)(struct vnode_t *, s32_t);
	s32_t (*vop_close)(struct vnode_t *, struct file_t *);
	s32_t (*vop_read)(struct vnode_t *, struct file_t *, void *, loff_t, loff_t *);
	s32_t (*vop_write)(struct vnode_t *, struct file_t *, void *, loff_t, loff_t *);
	s32_t (*vop_seek)(struct vnode_t *, struct file_t *, loff_t, loff_t);
	s32_t (*vop_ioctl)(struct vnode_t *, struct file_t *, int, void *);
	s32_t (*vop_fsync)(struct vnode_t *, struct file_t *);
	s32_t (*vop_readdir)(struct vnode_t *, struct file_t *, struct dirent_t *);
	s32_t (*vop_lookup)(struct vnode_t *, char *, struct vnode_t *);
	s32_t (*vop_create)(struct vnode_t *, char *, u32_t);
	s32_t (*vop_remove)(struct vnode_t *, struct vnode_t *, char *);
	s32_t (*vop_rename)(struct vnode_t *, struct vnode_t *, char *, struct vnode_t *, struct vnode_t *, char *);
	s32_t (*vop_mkdir)(struct vnode_t *, char *, u32_t);
	s32_t (*vop_rmdir)(struct vnode_t *, struct vnode_t *, char *);
	s32_t (*vop_getattr)(struct vnode_t *, struct vattr_t *);
	s32_t (*vop_setattr)(struct vnode_t *, struct vattr_t *);
	s32_t (*vop_inactive)(struct vnode_t *);
	s32_t (*vop_truncate)(struct vnode_t *, loff_t);
};

/*
 * file system id type
 */
struct fsid {
	s32_t val[2];
};

/*
 * directory description
 */
struct dir {
	s32_t fd;
	struct dirent_t entry;
};

/*
 * file system statistics
 */
struct statfs {
	s16_t f_type;				/* filesystem type number */
	s16_t f_flags;				/* copy of mount flags */
	s32_t f_bsize;				/* fundamental file system block size */
	s32_t f_blocks;				/* total data blocks in file system */
	s32_t f_bfree;				/* free blocks in fs */
	s32_t f_bavail;				/* free blocks avail to non-superuser */
	s32_t f_files;				/* total file nodes in file system */
	s32_t f_ffree;				/* free file nodes in fs */
	struct fsid f_fsid;			/* file system id */
	s32_t f_namelen;			/* maximum filename length */
};

/*
 * mount flags.
 */
#define	MOUNT_RDONLY			(0x00000001)	/* read only filesystem */
#define	MOUNT_MASK				(0x00000001)	/* mount flag mask value */

/*
 * mount data
 */
struct mount_t {
	struct list_head m_link;	/* link to next mount point */
	struct filesystem_t * m_fs;	/* pointer to fs */
	u32_t m_flags;				/* mount flag */
	s32_t m_count;				/* reference count */
	char m_path[MAX_PATH];		/* mounted path */
	void * m_dev;				/* mounted device */
	struct vnode_t * m_root;	/* root vnode */
	struct vnode_t * m_covered;	/* vnode covered on parent fs */
	void * m_data;				/* private data for fs */
};

/*
 * operations supported on virtual file system.
 */
struct vfsops_t {
	s32_t(*vfs_mount)(struct mount_t *, char *, s32_t);
	s32_t(*vfs_unmount)(struct mount_t *);
	s32_t(*vfs_sync)(struct mount_t *);
	s32_t(*vfs_vget)(struct mount_t *, struct vnode_t *);
	s32_t(*vfs_statfs)(struct mount_t *, struct statfs *);
	struct vnops_t * vfs_vnops;
};

/*
 * declare for vfs_mount
 */
void vfs_busy(struct mount_t * m);
void vfs_unbusy(struct mount_t * m);
s32_t vfs_findroot(char * path, struct mount_t ** mp, char ** root);

/*
 * declare for vfs_vnode
 */
struct vnode_t * vn_lookup(struct mount_t * mp, char * path);
struct vnode_t * vget(struct mount_t * mp, char * path);
void vput(struct vnode_t * vp);
s32_t vcount(struct vnode_t * vp);
void vref(struct vnode_t * vp);
void vrele(struct vnode_t * vp);
void vgone(struct vnode_t * vp);
void vflush(struct mount_t * mp);
s32_t vn_stat(struct vnode_t * vp, struct stat * st);
s32_t vn_access(struct vnode_t * vp, u32_t mode);

/*
 * declare for vfs_path
 */
int fd_alloc(int low);
int fd_free(int fd);
struct file_t * get_fp(int fd);
int set_fp(int fd, struct file_t *fp);
int vfs_path_conv(const char * path, char * full);
void vfs_setcwd(const char * path);
char * vfs_getcwd(char * buf, size_t size);
void vfs_setcwdfp(struct file_t * fp);
struct file_t * vfs_getcwdfp(void);

/*
 * declare for vfs_lookup
 */
s32_t vfs_namei(char * path, struct vnode_t ** vpp);
s32_t vfs_lookup(char * path, struct vnode_t ** vpp, char ** name);

/*
 * declare for vfs syscall
 */
s32_t sys_mount(char * dev, char * dir, char * fsname, u32_t flags);
s32_t sys_umount(char * path);
s32_t sys_sync(void);
s32_t sys_open(char * path, u32_t flags, u32_t mode, struct file_t ** pfp);
s32_t sys_close(struct file_t * fp);
s32_t sys_read(struct file_t * fp, void * buf, loff_t size, loff_t * count);
s32_t sys_write(struct file_t * fp, void * buf, loff_t size, loff_t * count);
s32_t sys_lseek(struct file_t * fp, loff_t off, u32_t type, loff_t * origin);
s32_t sys_ioctl(struct file_t * fp, int cmd, void * arg);
s32_t sys_fsync(struct file_t * fp);
s32_t sys_fstat(struct file_t * fp, struct stat * st);
s32_t sys_opendir(char * path, struct file_t ** file);
s32_t sys_closedir(struct file_t * fp);
s32_t sys_readdir(struct file_t * fp, struct dirent_t * dir);
s32_t sys_rewinddir(struct file_t * fp);
s32_t sys_seekdir(struct file_t * fp, loff_t loc);
s32_t sys_telldir(struct file_t * fp, loff_t * loc);
s32_t sys_mkdir(char * path, u32_t mode);
s32_t sys_rmdir(char * path);
s32_t sys_mknod(char * path, u32_t mode);
s32_t sys_rename(char * src, char * dest);
s32_t sys_unlink(char * path);
s32_t sys_access(char * path, u32_t mode);
s32_t sys_stat(char * path, struct stat * st);
s32_t sys_truncate(char * path, loff_t length);
s32_t sys_ftruncate(struct file_t * fp, loff_t length);
s32_t sys_fchdir(struct file_t * fp, char * cwd);

void do_init_vfs(void);

#ifdef __cplusplus
}
#endif

#endif /* __VFS_H__ */
