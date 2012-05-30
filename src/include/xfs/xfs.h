#ifndef __XFS_H__
#define __XFS_H__

#include <xboot.h>

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

/*
 * A xfs file handle.
 */
typedef struct xfs_file_t
{
	void * handle;
} xfs_file_t;

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

/*
 * An abstract i/o interface.
 */
typedef struct xfs_io_t
{
	/*
	 * Instance data for this struct.
	 */
	void * priv;

	s64_t (*read)(struct xfs_io_t * io, void * buf, u64_t len);
	s64_t (*write)(struct xfs_io_t *io, const void * buf, u64_t len);
	int (*seek)(struct xfs_io_t * io, u64_t offset);
	s64_t (*tell)(struct xfs_io_t * io);
	s64_t (*length)(struct xfs_io_t * io);

	struct xfs_io_t * (*duplicate)(struct xfs_io_t * io);

	int (*flush)(struct xfs_io_t * io);
	void (*destroy)(struct xfs_io_t * io);
} xfs_io_t;


/*
 * Information on various xfs
 */
struct xfs_archive_info_t
{
	const char * extension;
	const char * description;
};

struct xfs_archiver_t
{
#if 0
    /*
     * Basic info about this archiver...
     */
    const PHYSFS_ArchiveInfo info;


    /*
     * DIRECTORY ROUTINES:
     * These functions are for dir handles. Generate a handle with the
     *  openArchive() method, then pass it as the "opaque" PHYSFS_Dir to the
     *  others.
     *
     * Symlinks should always be followed (except in stat()); PhysicsFS will
     *  use the stat() method to check for symlinks and make a judgement on
     *  whether to continue to call other methods based on that.
     */

        /*
         * Open a dirhandle for dir/archive data provided by (io).
         *  (name) is a filename associated with (io), but doesn't necessarily
         *  map to anything, let alone a real filename. This possibly-
         *  meaningless name is in platform-dependent notation.
         * (forWrite) is non-zero if this is to be used for
         *  the write directory, and zero if this is to be used for an
         *  element of the search path.
         * Returns NULL on failure. We ignore any error code you set here.
         *  Returns non-NULL on success. The pointer returned will be
         *  passed as the "opaque" parameter for later calls.
         */
    PHYSFS_Dir *(*openArchive)(PHYSFS_Io *io, const char *name, int forWrite);

        /*
         * List all files in (dirname). Each file is passed to (cb),
         *  where a copy is made if appropriate, so you should dispose of
         *  it properly upon return from the callback.
         * You should omit symlinks if (omitSymLinks) is non-zero.
         * If you have a failure, report as much as you can.
         *  (dirname) is in platform-independent notation.
         */
    void (*enumerateFiles)(PHYSFS_Dir *opaque, const char *dirname,
                           int omitSymLinks, PHYSFS_EnumFilesCallback cb,
                           const char *origdir, void *callbackdata);

        /*
         * Open file for reading.
         *  This filename, (fnm), is in platform-independent notation.
         * If you can't handle multiple opens of the same file,
         *  you can opt to fail for the second call.
         * Fail if the file does not exist.
         * Returns NULL on failure, and calls __PHYSFS_setError().
         *  Returns non-NULL on success. The pointer returned will be
         *  passed as the "opaque" parameter for later file calls.
         *
         * Regardless of success or failure, please set *exists to
         *  non-zero if the file existed (even if it's a broken symlink!),
         *  zero if it did not.
         */
    PHYSFS_Io *(*openRead)(PHYSFS_Dir *opaque, const char *fnm, int *exists);

        /*
         * Open file for writing.
         * If the file does not exist, it should be created. If it exists,
         *  it should be truncated to zero bytes. The writing
         *  offset should be the start of the file.
         * This filename is in platform-independent notation.
         * If you can't handle multiple opens of the same file,
         *  you can opt to fail for the second call.
         * Returns NULL on failure, and calls __PHYSFS_setError().
         *  Returns non-NULL on success. The pointer returned will be
         *  passed as the "opaque" parameter for later file calls.
         */
    PHYSFS_Io *(*openWrite)(PHYSFS_Dir *opaque, const char *filename);

        /*
         * Open file for appending.
         * If the file does not exist, it should be created. The writing
         *  offset should be the end of the file.
         * This filename is in platform-independent notation.
         * If you can't handle multiple opens of the same file,
         *  you can opt to fail for the second call.
         * Returns NULL on failure, and calls __PHYSFS_setError().
         *  Returns non-NULL on success. The pointer returned will be
         *  passed as the "opaque" parameter for later file calls.
         */
    PHYSFS_Io *(*openAppend)(PHYSFS_Dir *opaque, const char *filename);

        /*
         * Delete a file in the archive/directory.
         *  Return non-zero on success, zero on failure.
         *  This filename is in platform-independent notation.
         *  This method may be NULL.
         * On failure, call __PHYSFS_setError().
         */
    int (*remove)(PHYSFS_Dir *opaque, const char *filename);

        /*
         * Create a directory in the archive/directory.
         *  If the application is trying to make multiple dirs, PhysicsFS
         *  will split them up into multiple calls before passing them to
         *  your driver.
         *  Return non-zero on success, zero on failure.
         *  This filename is in platform-independent notation.
         *  This method may be NULL.
         * On failure, call __PHYSFS_setError().
         */
    int (*mkdir)(PHYSFS_Dir *opaque, const char *filename);

        /*
         * Close directories/archives, and free any associated memory,
         *  including the original PHYSFS_Io and (opaque) itself, if
         *  applicable. Implementation can assume that it won't be called if
         *  there are still files open from this archive.
         */
    void (*closeArchive)(PHYSFS_Dir *opaque);

        /*
         * Obtain basic file metadata.
         *  Returns non-zero on success, zero on failure.
         *  On failure, call __PHYSFS_setError().
         */
    int (*stat)(PHYSFS_Dir *opaque, const char *fn,
                int *exists, PHYSFS_Stat *stat);
#endif
};


#endif /* __XFS_H__ */
