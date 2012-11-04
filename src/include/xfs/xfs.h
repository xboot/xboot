#ifndef __XFS_H__
#define __XFS_H__

#include <xboot.h>

/*
 * Function signature for callback that enumerate files
 */
typedef void (*xfs_enumerate_callback)(void * cbdata, const char * odir, const char * filename);

/*
 * A xfs context
 */
struct xfs_dir_handle_t
{
    void * handle;
    char * dname;
    char * mpoint;
    struct xfs_archiver_t * archiver;
    struct xfs_dir_handle_t * next;
};

struct xfs_file_handle_t
{
    u8_t forReading;
    u8_t * buffer;
    u32_t bufsize;
    u32_t buffill;
    u32_t bufpos;
    struct xfs_io_t * io;
    struct xfs_dir_handle_t * dhandle;
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

/*
 * Initial xfs functions
 */
bool_t xfs_init(struct xfs_context_t * ctx);
bool_t xfs_exit(struct xfs_context_t * ctx);

#if 0

/*
 * A xfs file handle.
 */
struct xfs_file_t
{
	void * handle;
};

#endif

#endif /* __XFS_H__ */
