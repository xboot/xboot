#ifndef __XFS_PLATFORM_H__
#define __XFS_PLATFORM_H__

#include <xfs/xfs.h>

struct xfs_context_t * __xfs_platform_init(void);
void __xfs_platform_exit(struct xfs_context_t * ctx);
void __xfs_platform_lock(void);
void __xfs_platform_unlock(void);
struct xfs_context_t * __xfs_platform_get_context(void);
const char * __xfs_platform_directory_separator(void);
char * __xfs_platform_cvt_to_dependent(const char * prepend, const char * dirname, const char * append);

void * __xfs_platform_open_read(const char * filename);
void * __xfs_platform_open_write(const char * filename);
void * __xfs_platform_open_append(const char * filename);
s64_t __xfs_platform_read(void * handle, void * buf, u32_t size, u32_t count);
s64_t __xfs_platform_write(void * handle, const void * buf, u32_t size, u32_t count);
s64_t __xfs_platform_get_last_modtime(const char * filename);
s64_t __xfs_platform_tell(void * handle);
s64_t __xfs_platform_length(void * handle);
bool_t __xfs_platform_seek(void * handle, u64_t pos);
bool_t __xfs_platform_eof(void * handle);
bool_t __xfs_platform_flush(void * handle);
bool_t __xfs_platform_close(void * handle);
bool_t __xfs_platform_exists(const char * filename);
bool_t __xfs_platform_is_link(const char * filename);
bool_t __xfs_platform_is_directory(const char * filename);
bool_t __xfs_platform_mkdir(const char * path);
bool_t __xfs_platform_delete(const char * path);
void __xfs_platform_enumerate(const char * path, xfs_enumerate_callback cb, const char * odir, void * cbdata);

#endif /* __XFS_PLATFORM_H__ */
