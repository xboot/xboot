#ifndef __SANDBOXLINUX_H__
#define __SANDBOXLINUX_H__

#include <stdint.h>

/*
 * Common interface
 */
uint64_t sandbox_linux_get_time(void);
ssize_t sandbox_linux_read(int fd, void * buf, size_t count);
ssize_t sandbox_linux_read_nonblock(int fd, void * buf, size_t count);
ssize_t sandbox_linux_write(int fd, const void * buf, size_t count);
off_t sandbox_linux_lseek(int fd, off_t offset);
int sandbox_linux_execve(const char * filename, char * const argv[], char * const envp[]);

/*
 * Sandbox interface
 */
struct sandbox_config_t {
	struct {
		char * name;
	} file;

	struct {
		void * mem;
		size_t size;
	} memory;

	struct {
		int width;
		int height;
		int xdpi;
		int ydpi;
	} framebuffer;
};
struct sandbox_config_t * sandbox_linux_get_config(void);
void sandbox_linux_init(int argc, char * argv[]);
void sandbox_linux_exit(void);

/*
 * Console interface
 */
ssize_t sandbox_linux_console_read(void * buf, size_t count);
ssize_t sandbox_linux_console_write(void * buf, size_t count);

/*
 * PM interface
 */
int sandbox_linux_pm_shutdown(void);
int sandbox_linux_pm_reset(void);
int sandbox_linux_pm_sleep(void);

/*
 * Event interface
 */
void sandbox_linux_sdl_event_init(void);
void sandbox_linux_sdl_event_exit(void);
void sandbox_linux_sdl_event_set_key_callback(void * device,
		void (*down)(void * device, unsigned int key),
		void (*up)(void * device, unsigned int key));
void sandbox_linux_sdl_event_set_mouse_callback(void * device,
		void (*down)(void * device, int x, int y, unsigned int btn),
		void (*move)(void * device, int x, int y),
		void (*up)(void * device, int x, int y, unsigned int btn),
		void (*wheel)(void * device, int dx, int dy));
void sandbox_linux_sdl_event_set_touch_callback(void * device,
		void (*begin)(void * device, int x, int y, unsigned int id),
		void (*move)(void * device, int x, int y, unsigned int id),
		void (*end)(void * device, int x, int y, unsigned int id));

/*
 * Framebuffer interface
 */
struct sandbox_fb_surface_t {
	int width;
	int height;
	int pitch;
	void * pixels;
	void * texture;
};

void sandbox_linux_sdl_fb_init(int width, int height);
void sandbox_linux_sdl_fb_exit(void);
int sandbox_linux_sdl_fb_surface_create(struct sandbox_fb_surface_t * surface, int width, int height);
int sandbox_linux_sdl_fb_surface_destroy(struct sandbox_fb_surface_t * surface);
int sandbox_linux_sdl_fb_surface_present(struct sandbox_fb_surface_t * surface);
void sandbox_linux_sdl_fb_set_backlight(int brightness);
int sandbox_linux_sdl_fb_get_backlight(void);

/*
 * Timer interface
 */
void sandbox_linux_sdl_timer_init(int delay, void (*cb)(void));
void sandbox_linux_sdl_timer_exit(void);

#endif /* __SANDBOXLINUX_H__ */
