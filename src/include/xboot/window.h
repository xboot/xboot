#ifndef __WINDOW_H__
#define __WINDOW_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <cairo-xboot.h>

struct window_manager_t {
	struct framebuffer_t * fb;
	struct list_head entry;
	struct list_head list;
	spinlock_t lock;
	struct {
		cairo_surface_t * cs;
		int width, height;
		int ox, oy;
		int nx, ny;
		int dirty;
		int show;
	} cursor;
};

struct window_t {
	struct window_manager_t * wm;
	struct region_list_t * rl;
	cairo_surface_t * cs;
	cairo_t * cr;
	int width, height;
	struct list_head entry;
};

static inline int window_manager_get_width(struct window_manager_t * wm)
{
	if(wm)
		return framebuffer_get_width(wm->fb);
	return 0;
}

static inline int window_manager_get_height(struct window_manager_t * wm)
{
	if(wm)
		return framebuffer_get_height(wm->fb);
	return 0;
}

static inline int window_manager_get_pwidth(struct window_manager_t * wm)
{
	if(wm)
		return framebuffer_get_pwidth(wm->fb);
	return 0;
}

static inline int window_manager_get_pheight(struct window_manager_t * wm)
{
	if(wm)
		return framebuffer_get_pheight(wm->fb);
	return 0;
}

static inline int window_manager_get_bytes(struct window_manager_t * wm)
{
	if(wm)
		return framebuffer_get_bytes(wm->fb);
	return 0;
}

static inline void window_manager_set_backlight(struct window_manager_t * wm, int brightness)
{
	if(wm)
		framebuffer_set_backlight(wm->fb, brightness);
}

static inline int window_manager_get_backlight(struct window_manager_t * wm)
{
	if(wm)
		return framebuffer_get_backlight(wm->fb);
	return 0;
}

static inline void window_manager_cursor_set_position(struct window_manager_t * wm, int x, int y)
{
	if(wm)
	{
		if(!wm->cursor.dirty)
		{
			wm->cursor.ox = wm->cursor.nx;
			wm->cursor.oy = wm->cursor.ny;
			wm->cursor.dirty = 1;
		}
		wm->cursor.nx = x;
		wm->cursor.ny = y;
	}
}

static inline void window_manager_cursor_get_position(struct window_manager_t * wm, int * x, int * y)
{
	if(wm)
	{
		if(x)
			*x = wm->cursor.nx;
		if(y)
			*y = wm->cursor.ny;
	}
}

static inline void window_manager_cursor_set_show(struct window_manager_t * wm, int show)
{
	if(wm)
		wm->cursor.show = show ? 1 : 0;
}

static inline int window_manager_cursor_get_show(struct window_manager_t * wm)
{
	return wm ? wm->cursor.show : 0;
}

struct window_manager_t * window_manager_alloc(const char * fb);
void window_manager_free(struct window_manager_t * wm);
struct window_t * window_alloc(struct window_manager_t * wm);
void window_free(struct window_t * w);
void window_to_front(struct window_t * w);
void window_to_back(struct window_t * w);
void window_region_list_add(struct window_t * w, struct region_t * r);
void window_region_list_clear(struct window_t * w);
void window_present(struct window_t * w, void * o, void (*draw)(struct window_t *, void *));

#ifdef __cplusplus
}
#endif

#endif /* __WINDOW_H__ */
