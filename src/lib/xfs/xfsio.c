/*
 * xfs/xfsio.c
 */

#include <xfs/xfs.h>

struct xfs_dir_handle_t
{
    void * pirv;
    char * dir_name;
    char * mount_point;
    const struct xfs_archiver_t * funcs;
    struct xfs_dir_handle_t *next;
};

struct xfs_file_handle_t
{
	struct xfs_io_t * io;
    u8_t forReading;
    const struct xfs_dir_handle_t * dir_handle;
    u8_t * buffer;
    u32_t bufsize;
    u32_t buffill;
    u32_t bufpos;
    struct xfs_file_handle_t * next;
};

struct xfs_err_state_t
{
    void * tid;
    enum xfs_err code;
    struct xfs_err_state_t * next;
};

//-----------------------
struct xfs {
	struct xfs_err_state_t * errorStates;
	struct xfs_dir_handle_t * searchPath;
	struct xfs_dir_handle_t * writeDir;
	struct xfs_file_handle_t * openWriteList;
	struct xfs_file_handle_t * openReadList;
	char * baseDir;
	char * userDir;
	char * prefDir;
	int allowSymLinks;
	const struct xfs_archiver_t **archivers;
	const struct xfs_archive_info_t ** archiveInfo;

	void * errorLock;
	void * stateLock;
};

//===================================

struct nativeio_info_t
{
	void * handle;
	const char * path;
	int mode;
};

static s64_t nativeio_read(struct xfs_io_t * io, void * buf, u64_t len)
{
	struct nativeio_info_t * info = (struct nativeio_info_t *)io->priv;

	//xxx return __PHYSFS_platformRead(info->handle, buf, len);
	return 0;
}

static s64_t nativeio_write(struct xfs_io_t * io, const void *buffer, u64_t len)
{
	struct nativeio_info_t * info = (struct nativeio_info_t *)io->priv;

	//xxx return __PHYSFS_platformWrite(info->handle, buffer, len);
	return 0;
}

static int nativeio_seek(struct xfs_io_t *io, u64_t offset)
{
	struct nativeio_info_t * info = (struct nativeio_info_t *)io->priv;

	//xxx return __PHYSFS_platformSeek(info->handle, offset);
	return 0;
}

static s64_t nativeio_tell(struct xfs_io_t * io)
{
	struct nativeio_info_t * info = (struct nativeio_info_t *)io->priv;

	//xxx return __PHYSFS_platformTell(info->handle);
	return 0;
}

static s64_t nativeio_length(struct xfs_io_t * io)
{
	struct nativeio_info_t * info = (struct nativeio_info_t *)io->priv;

	//xxx return __PHYSFS_platformFileLength(info->handle);
	return 0;
}

static struct xfs_io_t * nativeio_duplicate(struct xfs_io_t * io)
{
	struct nativeio_info_t * info = (struct nativeio_info_t *)io->priv;

	//xxx return __PHYSFS_createNativeIo(info->path, info->mode);
	return 0;
}

static int nativeio_flush(struct xfs_io_t * io)
{
	struct nativeio_info_t * info = (struct nativeio_info_t *)io->priv;

	//xxx return __PHYSFS_platformFlush(io->opaque);
	return 0;
}

static void nativeio_destroy(struct xfs_io_t * io)
{
	struct nativeio_info_t * info = (struct nativeio_info_t *)io->priv;

/*
	xxx
	__PHYSFS_platformClose(info->handle);
	allocator.Free((void *) info->path);
	allocator.Free(info);
	allocator.Free(io);
*/
}

