#ifndef __STAT_H__
#define __STAT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <types.h>

struct stat
{
    u32_t	st_ino;				/* file serial number */
    loff_t	st_size;     		/* file size */
    u32_t	st_mode;			/* file mode */
    u32_t	st_dev;				/* id of device containing file */
    u32_t	st_uid;				/* user ID of the file owner */
    u32_t	st_gid;				/* group ID of the file's group */
    u32_t	st_ctime;			/* file create time */
    u32_t	st_atime;			/* last access time */
    u32_t	st_mtime;			/* last data modification time */
};

/*
 * file type bits
 */
#define	S_IFDIR					(1<<0)
#define	S_IFCHR					(1<<1)
#define	S_IFBLK					(1<<2)
#define	S_IFREG					(1<<3)
#define	S_IFLNK					(1<<4)
#define	S_IFIFO					(1<<5)
#define	S_IFSOCK				(1<<6)
#define	S_IFMT					(S_IFDIR|S_IFCHR|S_IFBLK|S_IFREG|S_IFLNK|S_IFIFO|S_IFSOCK)

#define S_ISDIR(mode)			((mode) & S_IFDIR )
#define S_ISCHR(mode)			((mode) & S_IFCHR )
#define S_ISBLK(mode)			((mode) & S_IFBLK )
#define S_ISREG(mode)			((mode) & S_IFREG )
#define S_ISLNK(mode)			((mode) & S_IFLNK )
#define S_ISFIFO(mode)			((mode) & S_IFIFO )
#define S_ISSOCK(mode)			((mode) & S_IFSOCK )

/*
 * permission bits
 */
#define S_IXUSR					(1<<16)
#define S_IWUSR					(1<<17)
#define S_IRUSR					(1<<18)
#define S_IRWXU					(S_IRUSR|S_IWUSR|S_IXUSR)

#define S_IXGRP					(1<<19)
#define S_IWGRP					(1<<20)
#define S_IRGRP					(1<<21)
#define S_IRWXG					(S_IRGRP|S_IWGRP|S_IXGRP)

#define S_IXOTH					(1<<22)
#define S_IWOTH					(1<<23)
#define S_IROTH					(1<<24)
#define S_IRWXO					(S_IROTH|S_IWOTH|S_IXOTH)

/*
 * access permission
 */
#define	R_OK					(0x04)
#define	W_OK					(0x02)
#define	X_OK					(0x01)

#ifdef __cplusplus
}
#endif

#endif /* __STAT_H__ */
