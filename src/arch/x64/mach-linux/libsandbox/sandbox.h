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
 * Framebuffer interface
 */
struct sandbox_fb_surface_t {
	int width;
	int height;
	int pitch;
	void * pixels;
	void * priv;
};

/* Framebuffer device */
void * sandbox_fb_open(const char * dev);
void sandbox_fb_close(void * handle);
int sandbox_fb_get_width(void * handle);
int sandbox_fb_get_height(void * handle);
int sandbox_fb_get_pwidth(void * handle);
int sandbox_fb_get_pheight(void * handle);
int sandbox_fb_surface_create(void * handle, struct sandbox_fb_surface_t * surface);
int sandbox_fb_surface_destroy(void * handle, struct sandbox_fb_surface_t * surface);
int sandbox_fb_surface_present(void * handle, struct sandbox_fb_surface_t * surface);
void sandbox_fb_set_backlight(void * handle, int brightness);
int sandbox_fb_get_backlight(void * handle);

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
void sandbox_timer_init(void);
void sandbox_timer_exit(void);
void sandbox_timer_next(uint64_t time, void (*cb)(void *), void * data);
uint64_t sandbox_timer_count(void);
uint64_t sandbox_timer_frequency(void);

/*
 * Uart interface
 */
int sandbox_uart_open(const char * dev);
int sandbox_uart_close(int fd);
int sandbox_uart_set(int fd, int baud, int data, int parity, int stop);
ssize_t sandbox_uart_read(int fd, void * buf, size_t len);
ssize_t sandbox_uart_write(int fd, const void * buf, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* __SANDBOX_H__ */
