#ifndef __XFS_H__
#define __XFS_H__

#include <types.h>

/*
 * Values that represent specific causes of failure.
 */
typedef enum PHYSFS_ErrorCode
{
    PHYSFS_ERR_OK,               /* Success; no error.                    */
    PHYSFS_ERR_OTHER_ERROR,      /* Error not otherwise covered here.     */
    PHYSFS_ERR_OUT_OF_MEMORY,    /* Memory allocation failed.             */
    PHYSFS_ERR_NOT_INITIALIZED,  /* PhysicsFS is not initialized.         */
    PHYSFS_ERR_IS_INITIALIZED,   /* PhysicsFS is already initialized.     */
    PHYSFS_ERR_ARGV0_IS_NULL,    /* Needed argv[0], but it is NULL.       */
    PHYSFS_ERR_UNSUPPORTED,      /* Operation or feature unsupported.     */
    PHYSFS_ERR_PAST_EOF,         /* Attempted to access past end of file. */
    PHYSFS_ERR_FILES_STILL_OPEN, /* Files still open.                     */
    PHYSFS_ERR_INVALID_ARGUMENT, /* Bad parameter passed to an function.  */
    PHYSFS_ERR_NOT_MOUNTED,      /* Requested archive/dir not mounted.    */
    PHYSFS_ERR_NO_SUCH_PATH,     /* No such file, directory, or parent.   */
    PHYSFS_ERR_SYMLINK_FORBIDDEN,/* Symlink seen when not permitted.      */
    PHYSFS_ERR_NO_WRITE_DIR,     /* No write dir has been specified.      */
    PHYSFS_ERR_OPEN_FOR_READING, /* Wrote to a file opened for reading.   */
    PHYSFS_ERR_OPEN_FOR_WRITING, /* Read from a file opened for writing.  */
    PHYSFS_ERR_NOT_A_FILE,       /* Needed a file, got a directory (etc). */
    PHYSFS_ERR_READ_ONLY,        /* Wrote to a read-only filesystem.      */
    PHYSFS_ERR_CORRUPT,          /* Corrupted data encountered.           */
    PHYSFS_ERR_SYMLINK_LOOP,     /* Infinite symbolic link loop.          */
    PHYSFS_ERR_IO,               /* i/o error (hardware failure, etc).    */
    PHYSFS_ERR_PERMISSION,       /* Permission denied.                    */
    PHYSFS_ERR_NO_SPACE,         /* No space (disk full, over quota, etc) */
    PHYSFS_ERR_BAD_FILENAME,     /* Filename is bogus/insecure.           */
    PHYSFS_ERR_BUSY,             /* Tried to modify a file the OS needs.  */
    PHYSFS_ERR_DIR_NOT_EMPTY,    /* Tried to delete dir with files in it. */
    PHYSFS_ERR_OS_ERROR          /* Unspecified OS-level error.           */
} PHYSFS_ErrorCode;

/*
 * A archiver file handle.
 */
typedef struct archiver_file_t
{
    void * handle;
} archiver_file_t;

/*
 * Information on various archives
 */
typedef struct archiver_info_t
{
    const char * extension;
    const char * description;
} archiver_info_t;

/*
 * Archiver file type
 */
typedef enum archiver_filetype_t
{
	XFS_FILETYPE_OTHER		= 0,
	XFS_FILETYPE_REGULAR	= 1,
	XFS_FILETYPE_DIRECTORY	= 2,
	XFS_FILETYPE_SYMLINK	= 3,
} archiver_filetype_t;

/*
 * Meta data for a file or directory
 */
typedef struct archiver_stat_t
{
	s64_t filesize;		/* size in bytes, -1 for non-files and unknown */
	s64_t mtime;		/* last modification time */
	s64_t ctime;		/* like mtime, but for file creation time */
	s64_t atime;		/* like mtime, but for file access time */
	s64_t filetype;		/* file or directory or symlink */
	int readonly;		/* non-zero if read only, zero if writable. */
} archiver_stat_t;

/*
 * An abstract i/o interface.
 */
typedef struct archiver_io_t
{
    /*
     * Instance data for this struct.
     */
    void * opaque;

    s64_t (*read)(struct archiver_io_t * io, void * buf, u64_t len);
    s64_t (*write)(struct archiver_io_t *io, const void * buf, u64_t len);
    int (*seek)(struct archiver_io_t * io, u64_t offset);
    s64_t (*tell)(struct archiver_io_t * io);
    s64_t (*length)(struct archiver_io_t * io);

    struct archiver_io_t * (*duplicate)(struct archiver_io_t * io);

    int (*flush)(struct archiver_io_t * io);
    void (*destroy)(struct archiver_io_t * io);
} archiver_io_t;


#endif /* __XFS_H__ */
