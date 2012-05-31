#ifndef __XFSIO_H__
#define __XFSIO_H__

#include <xfs/platform.h>

/*
 * An abstract i/o interface.
 */
typedef struct xfs_io_t
{
	s64_t (*read)(struct xfs_io_t * io, void * buf, u64_t len);
	s64_t (*write)(struct xfs_io_t * io, const void * buf, u64_t len);
	int (*seek)(struct xfs_io_t * io, u64_t offset);
	s64_t (*tell)(struct xfs_io_t * io);
	s64_t (*length)(struct xfs_io_t * io);
	struct xfs_io_t * (*duplicate)(struct xfs_io_t * io);
	int (*flush)(struct xfs_io_t * io);
	void (*destroy)(struct xfs_io_t * io);

	void * priv;
} xfs_io_t;

struct xfs_io_t * __xfs_create_nativeio(const char * path, const char mode);

#endif /* __XFSIO_H__ */
