#ifndef __SANDBOXLINUX_H__
#define __SANDBOXLINUX_H__

#include <stdint.h>

/*
 * Common interface
 */
uint64_t sandbox_linux_get_time(void);
int sandbox_linux_read(int fd, void * buf, size_t count);
int sandbox_linux_read_nonblock(int fd, void * buf, size_t count);
ssize_t sandbox_linux_write(int fd, const void * buf, size_t count);
off_t sandbox_linux_lseek(int fd, off_t offset);
int sandbox_linux_execve(const char * filename, char * const argv[], char * const envp[]);

void sandbox_linux_init(int argc, char * argv[]);
void sandbox_linux_exit(void);

/*
 * Timer interface
 */
void sandbox_linux_timer_start(void (*cb)(void));
void sandbox_linux_timer_stop(void);

/*
 * PM interface
 */
int sandbox_linux_pm_sleep(void);
int sandbox_linux_pm_halt(void);
int sandbox_linux_pm_reset(void);

/*
 * Console interface
 */
ssize_t sandbox_linux_console_read(void * buf, size_t count);
ssize_t sandbox_linux_console_write(void * buf, size_t count);

/*
 * Framebuffer interface
 */
struct linux_fb_surface_t {
	int width;
	int height;
	int pitch;
	void * pixels;
	void * surface;
};

int sandbox_linux_sdl_fb_open(void);
int sandbox_linux_sdl_fb_close(void);
int sandbox_linux_sdl_surface_create(struct linux_fb_surface_t * surface);
int sandbox_linux_sdl_surface_destroy(struct linux_fb_surface_t * surface);
int sandbox_linux_sdl_present(struct linux_fb_surface_t * surface);

#endif /* __SANDBOXLINUX_H__ */
