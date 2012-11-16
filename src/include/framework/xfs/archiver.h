#ifndef __FRAMEWORK_XFS_ARCHIVER_H__
#define __FRAMEWORK_XFS_ARCHIVER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <framework/xfs/xfs.h>

/*
 * An abstract archiver interface.
 */
struct xfs_archiver_t
{
	const char * extension;
	const char * description;

	bool_t (*is_archive)(const char * name, int forWriting);
	void *(*open_archive)(const char * name, int forWriting);
	void (*enumerate)(void * handle, const char * dname, xfs_enumerate_callback cb, const char * odir, void * cbdata);
	bool_t (*exists)(void * handle, const char * name);
	bool_t (*is_directory)(void * handle, const char * name, int * fileExists);
	bool_t (*is_symlink)(void * handle, const char * name, int * fileExists);
	s64_t (*get_last_modtime)(void * handle, const char * name, int * fileExists);
	void *(*open_read)(void * handle, const char * name, int * fileExists);
	void *(*open_write)(void * handle, const char * name);
	void *(*open_append)(void * handle, const char * name);
	bool_t (*remove)(void * handle, const char * name);
	bool_t (*mkdir)(void * handle, const char * name);
	void (*dir_close)(void * handle); // close_archive
    s64_t (*read)(void * handle, void * buf, u32_t size, u32_t count);
    s64_t (*write)(void * handle, const void * buf, u32_t size, u32_t count);
    bool_t (*eof)(void * handle);
    s64_t (*tell)(void * handle);
    bool_t (*seek)(void * handle, u64_t pos);
    s64_t (*length)(void * handle);
    bool_t (*file_close)(void * handle);
};

extern const struct xfs_archiver_t __xfs_archiver_direct;
extern const struct xfs_archiver_t __xfs_archiver_zip;

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_XFS_ARCHIVER_H__ */
