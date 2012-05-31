#ifndef __XFS_PLATFORM_H__
#define __XFS_PLATFORM_H__

#include <xboot.h>

/*
 * A xfs file type
 */
typedef enum xfs_filetype_t
{
	XFS_FILETYPE_OTHER 		= 0,
	XFS_FILETYPE_REGULAR 	= 1,
	XFS_FILETYPE_DIRECTORY 	= 2,
	XFS_FILETYPE_SYMLINK 	= 3,
} archiver_filetype_t;

/*
 * Meta data for a file or directory
 */
typedef struct xfs_stat_t
{
	s64_t filesize; 	/* size in bytes, -1 for non-files and unknown */
	s64_t mtime; 		/* last modification time */
	s64_t ctime; 		/* like mtime, but for file creation time */
	s64_t atime; 		/* like mtime, but for file access time */
	s64_t filetype; 	/* file or directory or symlink */
	int readonly; 		/* non-zero if read only, zero if writable. */
} xfs_stat_t;


bool_t __xfs_platform_init(void);
bool_t __xfs_platform_exit(void);

void * __xfs_platform_create_mutex(void);
void __xfs_platform_destory_mutex(void * mutex);
void __xfs_platform_lock_mutex(void * mutex);
void __xfs_platform_unlock_mutex(void * mutex);

char __xfs_platform_directory_separator(void);

void * __xfs_platform_open_read(const char * path);
void * __xfs_platform_open_write(const char * path);
void * __xfs_platform_open_append(const char * path);
s64_t __xfs_platform_read(void * handle, void * buf, u64_t len);
s64_t __xfs_platform_write(void * handle, const void * buf, u64_t len);
int __xfs_platform_seek(void * handle, u64_t pos);
s64_t __xfs_platform_tell(void * handle);
s64_t __xfs_platform_length(void * handle);
int __xfs_platform_flush(void * handle);
void __xfs_platform_close(void * handle);
int __xfs_platform_mkdir(const char * path);
int __xfs_platform_delete(const char * path);
int __xfs_platform_stat(const char * path, int * exists, struct xfs_stat_t * st);

#endif /* __XFS_PLATFORM_H__ */
