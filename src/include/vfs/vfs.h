#ifndef __NEW_VFS_H__
#define __NEW_VFS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <list.h>
#include <string.h>
#include <atomic.h>
#include <spinlock.h>
#include <xboot/mutex.h>

#define VFS_MAX_PATH		(256)
#define	VFS_MAX_NAME		(64)
#define VFS_MAX_FD			(32)
#define VFS_NODE_HASH_SIZE	(32)

struct vfs_stat_t;
struct vfs_dirent_t;
struct vfs_node_t;
struct vfs_mount_t;
struct vfs_filesystem_t;

struct vfs_stat_t {
	u64_t st_ino;
	s64_t st_size;
	u32_t st_mode;
	u64_t st_dev;
	u32_t st_uid;
	u32_t st_gid;
	u64_t st_ctime;
	u64_t st_atime;
	u64_t st_mtime;
};

enum vfs_dirent_type_t {
	VDT_UNK,
	VDT_DIR,
	VDT_REG,
	VDT_BLK,
	VDT_CHR,
	VDT_FIFO,
	VDT_LNK,
	VDT_SOCK,
	VDT_WHT,
};

struct vfs_dirent_t {
	u64_t d_off;
	u32_t d_reclen;
	enum vfs_dirent_type_t d_type;
	char d_name[VFS_MAX_NAME];
};

enum vfs_node_flag_t {
	VNF_NONE,
	VNF_ROOT,
};

enum vfs_node_type_t {
	VNT_REG,
	VNT_DIR,
	VNT_BLK,
	VNT_CHR,
	VNT_LNK,
	VNT_SOCK,
	VNT_FIFO,
	VNT_UNK,
};

struct vfs_node_t {
	struct list_head v_link;
	struct vfs_mount_t * v_mount;
	atomic_t v_refcnt;
	char v_path[VFS_MAX_PATH];
	enum vfs_node_flag_t v_flags;
	enum vfs_node_type_t v_type;
	struct mutex_t v_lock;
	u64_t v_ctime;
	u64_t v_atime;
	u64_t v_mtime;
	u32_t v_mode;
	s64_t v_size;
	void * v_data;
};

enum {
	MOUNT_RDONLY	= 0x1,
	MOUNT_RW		= 0x2,
	MOUNT_MASK		= 0x3,
};

struct vfs_mount_t {
	struct list_head m_link;
	struct vfs_filesystem_t * m_fs;
	void * m_dev;
	char m_path[VFS_MAX_PATH];
	u32_t m_flags;
	atomic_t m_refcnt;
	struct vfs_node_t * m_root;
	struct vfs_node_t * m_covered;
	struct mutex_t m_lock;
	void * m_data;
};

struct vfs_filesystem_t {
	struct list_head list;
	const char * name;

	int (*mount)(struct vfs_mount_t *, const char *, u32_t);
	int (*unmount)(struct vfs_mount_t *);
	int (*msync)(struct vfs_mount_t *);
	int (*vget)(struct vfs_mount_t *, struct vfs_node_t *);
	int (*vput)(struct vfs_mount_t *, struct vfs_node_t *);

	u64_t (*read)(struct vfs_node_t *, s64_t, void *, u64_t);
	u64_t (*write)(struct vfs_node_t *, s64_t, void *, u64_t);
	int (*truncate)(struct vfs_node_t *, s64_t);
	int (*sync)(struct vfs_node_t *);
	int (*readdir)(struct vfs_node_t *, s64_t, struct vfs_dirent_t *);
	int (*lookup)(struct vfs_node_t *, const char *, struct vfs_node_t *);
	int (*create)(struct vfs_node_t *, const char *, u32_t);
	int (*remove)(struct vfs_node_t *, struct vfs_node_t *, const char *);
	int (*rename)(struct vfs_node_t *, const char *, struct vfs_node_t *, struct vfs_node_t *, const char *);
	int (*mkdir)(struct vfs_node_t *, const char *, u32_t);
	int (*rmdir)(struct vfs_node_t *, struct vfs_node_t *, const char *);
	int (*chmod)(struct vfs_node_t *, u32_t);
};

bool_t register_filesystem(struct vfs_filesystem_t * fs);
bool_t unregister_filesystem(struct vfs_filesystem_t * fs);

#ifdef __cplusplus
}
#endif

#endif /* __NEW_VFS_H__ */
