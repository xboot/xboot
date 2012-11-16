#ifndef __FRAMEWORK_XFS_H__
#define __FRAMEWORK_XFS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

/*
 * Function signature for callback that enumerate files
 */
typedef void (*xfs_enumerate_callback)(void * cbdata, const char * odir, const char * filename);

struct xfs_file_t
{
	void * handle;
};

struct xfs_dir_handle_t
{
    void * handle;
    char * dname;
    char * mpoint;
	const struct xfs_archiver_t * archiver;
    struct xfs_dir_handle_t * next;
};

struct xfs_file_handle_t
{
	void * handle;
    u8_t forReading;
    struct xfs_dir_handle_t * dhandle;
    struct xfs_archiver_t * archiver;
    u8_t * buffer;
    u32_t bufsize;
    u32_t buffill;
    u32_t bufpos;
    struct xfs_file_handle_t * next;
};

struct xfs_context_t {
	struct xfs_dir_handle_t * search_path;
	struct xfs_dir_handle_t * write_dir;
	struct xfs_file_handle_t * open_write_list;
	struct xfs_file_handle_t * open_read_list;

	char * base_dir;
	char * user_dir;

	void * lock;
};

struct xfs_context_t * __xfs_init(const char * argv0);
void __xfs_exit(struct xfs_context_t * ctx);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_XFS_H__ */
