#ifndef __SANDBOX_H__
#define __SANDBOX_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*
 * Sandbox interface
 */
struct sandbox_t {
	struct {
		char * buffer;
		size_t size;
	} json;

	char * app;
};
struct sandbox_t * sandbox_get(void);
void sandbox_init(int argc, char * argv[]);
void sandbox_exit(void);

/*
 * File interface
 */
int sandbox_file_open(const char * path, const char * mode);
int sandbox_file_close(int fd);
int sandbox_file_isdir(const char * path);
int sandbox_file_isfile(const char * path);
int sandbox_file_mkdir(const char * path);
int sandbox_file_remove(const char * path);
int sandbox_file_access(const char * path, const char * mode);
void sandbox_file_walk(const char * path, void (*cb)(const char * dir, const char * name, void * data), const char * dir, void * data);
ssize_t sandbox_file_read(int fd, void * buf, size_t count);
ssize_t sandbox_file_read_nonblock(int fd, void * buf, size_t count);
ssize_t sandbox_file_write(int fd, const void * buf, size_t count);
int64_t sandbox_file_seek(int fd, int64_t offset);
int64_t sandbox_file_tell(int fd);
int64_t sandbox_file_length(int fd);

/*
 * Keygen interface
 */
const char * sandbox_uniqueid(void);
int sandbox_keygen(const char * msg, void * key);

/*
 * PM interface
 */
void sandbox_pm_shutdown(void);
void sandbox_pm_reboot(void);
void sandbox_pm_sleep(void);

/*
 * Stdio interface
 */
void sandbox_stdio_init(void);
void sandbox_stdio_exit(void);
ssize_t sandbox_stdio_read(void * buf, size_t count);
ssize_t sandbox_stdio_write(void * buf, size_t count);

/*
 * Sysfs interface
 */
int sandbox_sysfs_access(const char * path, const char * mode);
int sandbox_sysfs_read_value(const char * path, int * v);
int sandbox_sysfs_write_value(const char * path, const int v);
int sandbox_sysfs_read_string(const char * path, char * s);
int sandbox_sysfs_write_string(const char * path, const char * s);

/*
 * Timer interface
 */
uint64_t sandbox_timer_count(void);
uint64_t sandbox_timer_frequency(void);

#ifdef __cplusplus
}
#endif

#endif /* __SANDBOX_H__ */
