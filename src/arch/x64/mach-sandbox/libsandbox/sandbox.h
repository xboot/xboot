#ifndef __SANDBOX_H__
#define __SANDBOX_H__

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
 * Stdio interface
 */
void sandbox_stdio_init(void);
void sandbox_stdio_exit(void);
ssize_t sandbox_stdio_read(void * buf, size_t count);
ssize_t sandbox_stdio_write(void * buf, size_t count);

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
 * PM interface
 */
void sandbox_pm_shutdown(void);
void sandbox_pm_reboot(void);
void sandbox_pm_sleep(void);

/*
 * Audio interface
 */
void sandbox_sdl_audio_init(void);
void sandbox_sdl_audio_exit(void);
void sandbox_sdl_audio_open(int fmt, int rate, int ch, int sample, void(*cb)(void *, void *, int), void * data);
void sandbox_sdl_audio_close(void);
void sandbox_sdl_audio_start(void);
void sandbox_sdl_audio_stop(void);

/*
 * Event interface
 */
void sandbox_sdl_event_init(void);
void sandbox_sdl_event_exit(void);
void sandbox_sdl_event_set_key_callback(void * device,
		void (*down)(void * device, unsigned int key),
		void (*up)(void * device, unsigned int key));
void sandbox_sdl_event_set_mouse_callback(void * device,
		void (*down)(void * device, int x, int y, unsigned int button),
		void (*move)(void * device, int x, int y),
		void (*up)(void * device, int x, int y, unsigned int button),
		void (*wheel)(void * device, int dx, int dy));
void sandbox_sdl_event_set_touch_callback(void * device,
		void (*begin)(void * device, int x, int y, unsigned int id),
		void (*move)(void * device, int x, int y, unsigned int id),
		void (*end)(void * device, int x, int y, unsigned int id));
void sandbox_sdl_event_set_joystick_callback(void * device,
		void (*left_stick)(void * device, int x, int y),
		void (*right_stick)(void * device, int x, int y),
		void (*left_trigger)(void * device, int v),
		void (*right_trigger)(void * device, int v),
		void (*button_down)(void * device, unsigned int button),
		void (*button_up)(void * device, unsigned int button));

/*
 * Framebuffer interface
 */
struct sandbox_fb_surface_t {
	int width;
	int height;
	int pitch;
	void * pixels;
	void * surface;
};

void * sandbox_sdl_fb_init(const char * title, int width, int height, int fullscreen);
void sandbox_sdl_fb_exit(void * handle);
int sandbox_sdl_fb_get_width(void * handle);
int sandbox_sdl_fb_get_height(void * handle);
int sandbox_sdl_fb_surface_create(void * handle, struct sandbox_fb_surface_t * surface);
int sandbox_sdl_fb_surface_destroy(void * handle, struct sandbox_fb_surface_t * surface);
int sandbox_sdl_fb_surface_present(void * handle, struct sandbox_fb_surface_t * surface);
void sandbox_sdl_fb_set_backlight(void * handle, int brightness);
int sandbox_sdl_fb_get_backlight(void * handle);

/*
 * Timer interface
 */
void sandbox_sdl_timer_init(void);
void sandbox_sdl_timer_exit(void);
void sandbox_sdl_timer_set_next(int delay, void (*cb)(void *), void * data);
uint64_t sandbox_get_time_counter(void);
uint64_t sandbox_get_time_frequency(void);

/*
 * Sysfs interface
 */
int sandbox_sysfs_access(const char * path, const char * mode);
int sandbox_sysfs_read_value(const char * path, int * v);
int sandbox_sysfs_write_value(const char * path, const int v);
int sandbox_sysfs_read_string(const char * path, char * s);
int sandbox_sysfs_write_string(const char * path, const char * s);

#endif /* __SANDBOX_H__ */
