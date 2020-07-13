#ifndef __SANDBOX_H__
#define __SANDBOX_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*
 * Sandbox macro
 */
#define sandbox_min(a, b)	({typeof(a) _amin = (a); typeof(b) _bmin = (b); (void)(&_amin == &_bmin); _amin < _bmin ? _amin : _bmin;})
#define sandbox_max(a, b)	({typeof(a) _amax = (a); typeof(b) _bmax = (b); (void)(&_amax == &_bmax); _amax > _bmax ? _amax : _bmax;})

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
 * Sandbox region
 */
struct sandbox_region_t {
	int x, y;
	int w, h;
};

static inline void sandbox_region_init(struct sandbox_region_t * r, int x, int y, int w, int h)
{
	r->x = x;
	r->y = y;
	r->w = w;
	r->h = h;
}

static inline void sandbox_region_clone(struct sandbox_region_t * r, struct sandbox_region_t * o)
{
	r->x = o->x;
	r->y = o->y;
	r->w = o->w;
	r->h = o->h;
}

static inline int sandbox_region_isempty(struct sandbox_region_t * r)
{
	if((r->w > 0) && (r->h > 0))
		return 0;
	return 1;
}

static inline int sandbox_region_hit(struct sandbox_region_t * r, int x, int y)
{
	if((x >= r->x) && (x < r->x + r->w) && (y >= r->y) && (y < r->y + r->h))
		return 1;
	return 0;
}

static inline int sandbox_region_contains(struct sandbox_region_t * r, struct sandbox_region_t * o)
{
	int rr = r->x + r->w;
	int rb = r->y + r->h;
	int or = o->x + o->w;
	int ob = o->y + o->h;
	if((o->x >= r->x) && (o->x < rr) && (o->y >= r->y) && (o->y < rb) && (or > r->x) && (or <= rr) && (ob > r->y) && (ob <= rb))
		return 1;
	return 0;
}

static inline int sandbox_region_overlap(struct sandbox_region_t * r, struct sandbox_region_t * o)
{
	if((o->x + o->w >= r->x) && (o->x <= r->x + r->w) && (o->y + o->h >= r->y) && (o->y <= r->y + r->h))
		return 1;
	return 0;
}

static inline void sandbox_region_expand(struct sandbox_region_t * r, struct sandbox_region_t * o, int n)
{
	r->x = o->x - n;
	r->y = o->y - n;
	r->w = o->w + n * 2;
	r->h = o->h + n * 2;
}

static inline int sandbox_region_intersect(struct sandbox_region_t * r, struct sandbox_region_t * a, struct sandbox_region_t * b)
{
	int x0 = sandbox_max(a->x, b->x);
	int x1 = sandbox_min(a->x + a->w, b->x + b->w);
	if(x0 <= x1)
	{
		int y0 = sandbox_max(a->y, b->y);
		int y1 = sandbox_min(a->y + a->h, b->y + b->h);
		if(y0 <= y1)
		{
			r->x = x0;
			r->y = y0;
			r->w = x1 - x0;
			r->h = y1 - y0;
			return 1;
		}
	}
	return 0;
}

static inline int sandbox_region_union(struct sandbox_region_t * r, struct sandbox_region_t * a, struct sandbox_region_t * b)
{
	int ar = a->x + a->w;
	int ab = a->y + a->h;
	int br = b->x + b->w;
	int bb = b->y + b->h;
	r->x = sandbox_min(a->x, b->x);
	r->y = sandbox_min(a->y, b->y);
	r->w = sandbox_max(ar, br) - r->x;
	r->h = sandbox_max(ab, bb) - r->y;
	return 1;
}

struct sandbox_region_list_t {
	struct sandbox_region_t * region;
	unsigned int size;
	unsigned int count;
};

struct sandbox_region_list_t * sandbox_region_list_alloc(unsigned int size);
void sandbox_region_list_free(struct sandbox_region_list_t * rl);
void sandbox_region_list_clone(struct sandbox_region_list_t * rl, struct sandbox_region_list_t * o);
void sandbox_region_list_merge(struct sandbox_region_list_t * rl, struct sandbox_region_list_t * o);
void sandbox_region_list_add(struct sandbox_region_list_t * rl, struct sandbox_region_t * r);
void sandbox_region_list_clear(struct sandbox_region_list_t * rl);

/*
 * Audio interface
 */
void * sandbox_audio_playback_start(int rate, int fmt, int ch, int(*cb)(void *, void *, int), void * data);
void sandbox_audio_playback_stop(void * context);
void * sandbox_audio_capture_start(int rate, int fmt, int ch, int(*cb)(void *, void *, int), void * data);
void sandbox_audio_capture_stop(void * context);
int sandbox_audio_ioctl(const char * cmd, void * arg);

/*
 * Event interface
 */
/* Input device */
void * sandbox_event_open(void);
void sandbox_event_close(void * context);
void sandbox_event_mouse_set_range(void * context, int xmax, int ymax);
void sandbox_event_mouse_get_range(void * context, int * xmax, int * ymax);
void sandbox_event_mouse_set_sensitivity(void * context, int s);
void sandbox_event_mouse_get_sensitivity(void * context, int * s);
void sandbox_event_set_key_callback(void * context, void * device,
		void (*down)(void * device, unsigned int key),
		void (*up)(void * device, unsigned int key));
void sandbox_event_set_mouse_callback(void * context, void * device,
		void (*down)(void * device, int x, int y, unsigned int button),
		void (*move)(void * device, int x, int y),
		void (*up)(void * device, int x, int y, unsigned int button),
		void (*wheel)(void * device, int dx, int dy));
void sandbox_event_set_touch_callback(void * context, void * device,
		void (*begin)(void * device, int x, int y, unsigned int id),
		void (*move)(void * device, int x, int y, unsigned int id),
		void (*end)(void * device, int x, int y, unsigned int id));
void sandbox_event_set_joystick_callback(void * context, void * device,
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
	int stride;
	int pixlen;
	void * pixels;
	void * priv;
};

/* Framebuffer device */
void * sandbox_fb_open(const char * dev);
void sandbox_fb_close(void * context);
int sandbox_fb_get_width(void * context);
int sandbox_fb_get_height(void * context);
int sandbox_fb_get_pwidth(void * context);
int sandbox_fb_get_pheight(void * context);
int sandbox_fb_surface_create(void * context, struct sandbox_fb_surface_t * surface);
int sandbox_fb_surface_destroy(void * context, struct sandbox_fb_surface_t * surface);
int sandbox_fb_surface_present(void * context, struct sandbox_fb_surface_t * surface, struct sandbox_region_list_t * rl);
void sandbox_fb_set_backlight(void * context, int brightness);
int sandbox_fb_get_backlight(void * context);

/* DRM device */
void * sandbox_fb_drm_open(const char * dev);
void sandbox_fb_drm_close(void * context);
int sandbox_fb_drm_get_width(void * context);
int sandbox_fb_drm_get_height(void * context);
int sandbox_fb_drm_get_pwidth(void * context);
int sandbox_fb_drm_get_pheight(void * context);
int sandbox_fb_drm_surface_create(void * context, struct sandbox_fb_surface_t * surface);
int sandbox_fb_drm_surface_destroy(void * context, struct sandbox_fb_surface_t * surface);
int sandbox_fb_drm_surface_present(void * context, struct sandbox_fb_surface_t * surface, struct sandbox_region_list_t * rl);
void sandbox_fb_drm_set_backlight(void * context, int brightness);
int sandbox_fb_drm_get_backlight(void * context);

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
