#ifndef __XFS_H__
#define __XFS_H__

#include <xboot.h>

/*
 * Function signature for callback that enumerate files
 */
typedef void (*xfs_enumerate_callback)(void * data, const char * dir, const char * file);

/*
 * A xfs file type
 */
enum xfs_filetype_t
{
	XFS_FILETYPE_OTHER 		= 0,
	XFS_FILETYPE_REGULAR 	= 1,
	XFS_FILETYPE_DIRECTORY 	= 2,
};

/*
 * Meta data for a file or directory
 */
struct xfs_stat_t
{
	u64_t size;
	u32_t ctime;
	u32_t atime;
	u32_t mtime;
	enum xfs_filetype_t type;
};

/*
 * An abstract i/o interface.
 */
struct xfs_io_t
{
	u64_t (*read)(struct xfs_io_t * io, void * buf, u64_t len);
	u64_t (*write)(struct xfs_io_t * io, const void * buf, u64_t len);
	u64_t (*tell)(struct xfs_io_t * io);
	u64_t (*length)(struct xfs_io_t * io);
	bool_t (*seek)(struct xfs_io_t * io, u64_t off);
	bool_t (*flush)(struct xfs_io_t * io);
	struct xfs_io_t * (*duplicate)(struct xfs_io_t * io);
	void (*destroy)(struct xfs_io_t * io);

	void * priv;
};

/*
 * An abstract archiver interface.
 */
struct xfs_archiver_t
{
	const char * extension;
	const char * description;

    void * (*open_archive)(struct xfs_io_t * io, const char * name, int forWrite);
	struct xfs_io_t * (*open_read)(void * handle, const char * name);
	struct xfs_io_t * (*open_write)(void * handle, const char * name);
	struct xfs_io_t * (*open_append)(void * handle, const char * name);
	bool_t (*remove)(void * handle, const char * name);
	bool_t (*mkdir)(void * handle, const char * name);
	bool_t (*stat)(void * handle, const char * name, struct xfs_stat_t * stat);
	void (*enumerate)(void * handle, const char * dname, xfs_enumerate_callback cb, const char * odir, void * cbdata);
	void (*close_archive)(void * handle);
};

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
//	struct xfs_archiver_t ** archivers;

	char * baseDir;
	char * userDir;
//	char * prefDir;

	void * lock;
};

/*
 * Various archiver
 */
extern struct xfs_archiver_t __xfs_archiver_direct;
extern struct xfs_archiver_t ** __xfs_archiver_supported;

/*
 * Platform functions
 */
struct xfs_context_t * __xfs_platform_init(void);
void __xfs_platform_exit(struct xfs_context_t * ctx);
inline struct xfs_context_t * __xfs_platform_get_context(void);
inline char __xfs_platform_directory_separator(void);
inline void __xfs_platform_lock(void);
inline void __xfs_platform_unlock(void);

void * __xfs_platform_open_read(const char * path);
void * __xfs_platform_open_write(const char * path);
void * __xfs_platform_open_append(const char * path);
u64_t __xfs_platform_read(void * handle, void * buf, u64_t len);
u64_t __xfs_platform_write(void * handle, const void * buf, u64_t len);
u64_t __xfs_platform_tell(void * handle);
u64_t __xfs_platform_length(void * handle);
bool_t __xfs_platform_seek(void * handle, u64_t pos);
bool_t __xfs_platform_flush(void * handle);
bool_t __xfs_platform_close(void * handle);
bool_t __xfs_platform_mkdir(const char * path);
bool_t __xfs_platform_delete(const char * path);
bool_t __xfs_platform_stat(const char * path, struct xfs_stat_t * st);
void __xfs_platform_enumerate(const char * path, xfs_enumerate_callback cb, const char * odir, void * cbdata);

/*
 * Nativeio functions
 */
struct xfs_io_t * __xfs_create_nativeio(const char * path, const char mode);

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
