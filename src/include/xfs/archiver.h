#ifndef __XFS_ARCHIVER_H__
#define __XFS_ARCHIVER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <stdint.h>
#include <list.h>
#include <shash.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <path.h>
#include <malloc.h>
#include <irqflags.h>
#include <spinlock.h>
#include <xboot/initcall.h>

typedef void (*xfs_walk_callback_t)(const char * dir, const char * name, void * data);

enum {
	XFS_OPEN_MODE_READ		= 0,
	XFS_OPEN_MODE_WRITE		= 1,
	XFS_OPEN_MODE_APPEND	= 2,
};

struct xfs_archiver_t
{
	char * name;
	struct list_head list;

	void * (*mount)(const char * path, int * writable);
	void (*umount)(void * m);
	void (*walk)(void * m, const char * name, xfs_walk_callback_t cb, void * data);
	bool_t (*isdir)(void * m, const char * name);
	bool_t (*isfile)(void * m, const char * name);
	bool_t (*mkdir)(void * m, const char * name);
	bool_t (*remove)(void * m, const char * name);
	void * (*open)(void * m, const char * name, int mode);
	s64_t (*read)(void * f, void * buf, s64_t size);
	s64_t (*write)(void * f, void * buf, s64_t size);
	s64_t (*seek)(void * f, s64_t offset);
	s64_t (*tell)(void * f);
	s64_t (*length)(void * f);
	void (*close)(void * f);
};

bool_t register_archiver(struct xfs_archiver_t * archiver);
bool_t unregister_archiver(struct xfs_archiver_t * archiver);
void * mount_archiver(const char * path, struct xfs_archiver_t ** archiver, int * writable);

#ifdef __cplusplus
}
#endif

#endif /* __XFS_ARCHIVER_H__ */
