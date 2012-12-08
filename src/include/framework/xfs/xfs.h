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

struct xfs_context_t * __xfs_alloc(void);
void __xfs_free(struct xfs_context_t * ctx);

bool_t xfs_init(const char * path);
bool_t xfs_mount(const char * dir, const char * mpoint, int appendToPath);
bool_t xfs_add_to_search_path(const char * dir, int appendToPath);
bool_t xfs_remove_from_search_path(const char * dir);
const char * xfs_get_mount_point(const char * dir);
bool_t xfs_mkdir(const char * dir);
bool_t xfs_delete(const char * name);
const char * xfs_get_real_dir(const char * name);
bool_t xfs_exists(const char * name);
s64_t xfs_get_last_modtime(const char * name);
bool_t xfs_is_directory(const char * name);
bool_t xfs_is_symlink(const char * name);
struct xfs_file_t * xfs_open_write(const char * name);
struct xfs_file_t * xfs_open_append(const char * name);
struct xfs_file_t *xfs_open_read(const char * name);
bool_t xfs_close(struct xfs_file_t * h);
s64_t xfs_read(struct xfs_file_t * handle, void * buffer, u32_t objSize, u32_t objCount);
s64_t xfs_write(struct xfs_file_t * handle, const void * buffer, u32_t objSize, u32_t objCount);
bool_t xfs_eof(struct xfs_file_t * handle);
s64_t xfs_tell(struct xfs_file_t * handle);
bool_t xfs_seek(struct xfs_file_t * handle, u64_t pos);
s64_t xfs_length(struct xfs_file_t * handle);
bool_t xfs_flush(struct xfs_file_t * handle);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_XFS_H__ */
