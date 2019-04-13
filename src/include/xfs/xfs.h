#ifndef __XFS_H__
#define __XFS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xfs/archiver.h>

struct xfs_path_t {
	char * path;
	void * mhandle;
	int writable;
	struct xfs_archiver_t * archiver;
	struct list_head list;
};

struct xfs_context_t {
	struct xfs_path_t mounts;
	spinlock_t lock;
};

struct xfs_file_t {
	struct xfs_context_t * ctx;
	struct xfs_path_t * path;
	void * fhandle;
};

bool_t xfs_mount(struct xfs_context_t * ctx, const char * path, int writable);
bool_t xfs_umount(struct xfs_context_t * ctx, const char * path);
void xfs_walk(struct xfs_context_t * ctx, const char * name, xfs_walk_callback_t cb, void * data);
bool_t xfs_isdir(struct xfs_context_t * ctx, const char * name);
bool_t xfs_isfile(struct xfs_context_t * ctx, const char * name);
bool_t xfs_mkdir(struct xfs_context_t * ctx, const char * name);
bool_t xfs_remove(struct xfs_context_t * ctx, const char * name);
struct xfs_file_t * xfs_open_read(struct xfs_context_t * ctx, const char * name);
struct xfs_file_t * xfs_open_write(struct xfs_context_t * ctx, const char * name);
struct xfs_file_t * xfs_open_append(struct xfs_context_t * ctx, const char * name);
s64_t xfs_read(struct xfs_file_t * file, void * buf, s64_t size);
s64_t xfs_write(struct xfs_file_t * file, void * buf, s64_t size);
s64_t xfs_seek(struct xfs_file_t * file, s64_t offset);
s64_t xfs_tell(struct xfs_file_t * file);
s64_t xfs_length(struct xfs_file_t * file);
void xfs_close(struct xfs_file_t * file);

struct xfs_context_t * xfs_alloc(const char * path, int userdata);
void xfs_free(struct xfs_context_t * ctx);

#ifdef __cplusplus
}
#endif

#endif /* __XFS_H__ */
