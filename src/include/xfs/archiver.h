#ifndef __XFS_ARCHIVER_H__
#define __XFS_ARCHIVER_H__

#include <xfs/xfsio.h>

/*
 * Function signature for callbacks that enumerate files
 */
typedef void (*xfs_enum_files_callback)(void * data, const char * dir, const char * file);

/*
 * Information on various archive
 */
struct xfs_archive_info_t
{
	const char * extension;
	const char * description;
};

struct xfs_archiver_t
{
    const struct xfs_archive_info_t info;

    void *(*open_archive)(struct xfs_io_t * io, const char * name, int forWrite);
	struct xfs_io_t *(*open_read)(void * opaque, const char * fnm, int * exists);
	struct xfs_io_t *(*open_write)(void * opaque, const char * filename);
	struct xfs_io_t *(*open_append)(void * opaque, const char * filename);
	int (*remove)(void * opaque, const char * filename);
	int (*mkdir)(void * opaque, const char * filename);
	int (*stat)(void * opaque, const char * fn, int * exists, struct xfs_stat_t * stat);
	void (*enumerate_files)(void * opaque, const char * dirname, int omitSymLinks, xfs_enum_files_callback cb, const char * origdir, void * callbackdata);
	void (*close_archive)(void * opaque);
};


extern const struct xfs_archiver_t __xfs_archiver_direct;


#endif /* __XFS_ARCHIVER_H__ */
