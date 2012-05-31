/*
 * xfs/xfsio.c
 */

#include <xfs/xfsio.h>

struct nativeio_info_t
{
	void * handle;
	const char * path;
	char mode;	/* 'r', 'w', 'a' */
};

static s64_t nativeio_read(struct xfs_io_t * io, void * buf, u64_t len)
{
	struct nativeio_info_t * info = (struct nativeio_info_t *)io->priv;
	return __xfs_platform_read(info->handle, buf, len);
}

static s64_t nativeio_write(struct xfs_io_t * io, const void * buf, u64_t len)
{
	struct nativeio_info_t * info = (struct nativeio_info_t *)io->priv;
	return __xfs_platform_write(info->handle, buf, len);
}

static int nativeio_seek(struct xfs_io_t *io, u64_t offset)
{
	struct nativeio_info_t * info = (struct nativeio_info_t *)io->priv;
	return __xfs_platform_seek(info->handle, offset);
}

static s64_t nativeio_tell(struct xfs_io_t * io)
{
	struct nativeio_info_t * info = (struct nativeio_info_t *)io->priv;
	return __xfs_platform_tell(info->handle);
}

static s64_t nativeio_length(struct xfs_io_t * io)
{
	struct nativeio_info_t * info = (struct nativeio_info_t *)io->priv;
	return __xfs_platform_length(info->handle);
}

static struct xfs_io_t * nativeio_duplicate(struct xfs_io_t * io)
{
	struct nativeio_info_t * info = (struct nativeio_info_t *)io->priv;
	return __xfs_create_nativeio(info->path, info->mode);
}

static int nativeio_flush(struct xfs_io_t * io)
{
	struct nativeio_info_t * info = (struct nativeio_info_t *)io->priv;
	return __xfs_platform_flush(info->handle);
}

static void nativeio_destroy(struct xfs_io_t * io)
{
	struct nativeio_info_t * info = (struct nativeio_info_t *)io->priv;

	__xfs_platform_close(info->handle);
	free((void *) info->path);
	free(info);
	free(io);
}

struct xfs_io_t * __xfs_create_nativeio(const char * path, const char mode)
{
	struct xfs_io_t * io;
	struct nativeio_info_t * info;
    void * handle;
    char * pathdup;

	if(mode == 'r')
		handle = __xfs_platform_open_read(path);
	else if(mode == 'w')
		handle = __xfs_platform_open_write(path);
	else if(mode == 'a')
		handle = __xfs_platform_open_append(path);
	else
		handle = NULL;

	if(!handle)
		return NULL;

    io = malloc(sizeof(struct xfs_io_t));
    info = malloc(sizeof(struct nativeio_info_t));
	pathdup = malloc(strlen(path) + 1);

	if(!io || !info || !pathdup)
	{
		if(handle)
			__xfs_platform_close(handle);
		if(io)
			free(io);
		if(info)
			free(info);
		if(pathdup)
			free(pathdup);

		return NULL;
	}

    strcpy(pathdup, path);
    info->handle = handle;
    info->path = pathdup;
    info->mode = mode;

    io->read = nativeio_read;
    io->write = nativeio_write;
    io->seek = nativeio_seek;
    io->tell = nativeio_tell;
    io->length = nativeio_length;
    io->duplicate = nativeio_duplicate;
    io->flush = nativeio_flush;
    io->destroy = nativeio_destroy;
    io->priv = info;

    return io;
}
