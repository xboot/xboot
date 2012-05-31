#ifndef __XFS_H__
#define __XFS_H__

#include <xfs/xfsio.h>

/*
 * A xfs file handle.
 */
typedef struct xfs_file_t
{
	void * handle;
} xfs_file_t;








/*
 * Values that represent specific causes of failure.
 */
typedef enum xfs_err
{
	XFS_ERR_OK,						/* Success; no error.                    */
	XFS_ERR_OTHER_ERROR,			/* Error not otherwise covered here.     */
	XFS_ERR_OUT_OF_MEMORY,			/* Memory allocation failed.             */
	XFS_ERR_NOT_INITIALIZED,		/* not initialized.         			 */
	XFS_ERR_IS_INITIALIZED,			/* already initialized.                  */
	XFS_ERR_ARGV0_IS_NULL,			/* Needed argv[0], but it is NULL.       */
	XFS_ERR_UNSUPPORTED,			/* Operation or feature unsupported.     */
	XFS_ERR_PAST_EOF,				/* Attempted to access past end of file. */
	XFS_ERR_FILES_STILL_OPEN,		/* Files still open.                     */
	XFS_ERR_INVALID_ARGUMENT,		/* Bad parameter passed to an function.  */
	XFS_ERR_NOT_MOUNTED,			/* Requested archive/dir not mounted.    */
	XFS_ERR_NO_SUCH_PATH,			/* No such file, directory, or parent.   */
	XFS_ERR_SYMLINK_FORBIDDEN,		/* Symlink seen when not permitted.      */
	XFS_ERR_NO_WRITE_DIR,			/* No write dir has been specified.      */
	XFS_ERR_OPEN_FOR_READING,		/* Wrote to a file opened for reading.   */
	XFS_ERR_OPEN_FOR_WRITING,		/* Read from a file opened for writing.  */
	XFS_ERR_NOT_A_FILE,				/* Needed a file, got a directory (etc). */
	XFS_ERR_READ_ONLY,				/* Wrote to a read-only filesystem.      */
	XFS_ERR_CORRUPT,				/* Corrupted data encountered.           */
	XFS_ERR_SYMLINK_LOOP,			/* Infinite symbolic link loop.          */
	XFS_ERR_IO,						/* i/o error (hardware failure, etc).    */
	XFS_ERR_PERMISSION,				/* Permission denied.                    */
	XFS_ERR_NO_SPACE,				/* No space (disk full, over quota, etc) */
	XFS_ERR_BAD_FILENAME,			/* Filename is bogus/insecure.           */
	XFS_ERR_BUSY,					/* Tried to modify a file the OS needs.  */
	XFS_ERR_DIR_NOT_EMPTY,			/* Tried to delete dir with files in it. */
	XFS_ERR_OS_ERROR,				/* Unspecified OS-level error.           */
} xfs_err;

#endif /* __XFS_H__ */
