#ifndef __WINDOW_H__
#define __WINDOW_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <stdint.h>
#include <list.h>
#include <fifo.h>
#include <irqflags.h>
#include <spinlock.h>
#include <xboot/event.h>
#include <framebuffer/framebuffer.h>

struct window_manager_t {
	spinlock_t lock;
	struct list_head list;
	struct list_head window;
	struct framebuffer_t * fb;
	int wcount;
	int refresh;
	struct {
		struct surface_t * s;
		struct region_t r;
	} watermark;
	struct {
		struct surface_t * s;
		struct region_t ro;
		struct region_t rn;
		int dirty;
		int show;
	} cursor;
};

struct window_t {
	struct task_t * task;
	struct list_head list;
	struct window_manager_t * wm;
	struct surface_t * s;
	struct region_list_t * rl;
	struct fifo_t * event;
	struct hmap_t * map;
	int launcher;
};

extern struct list_head __window_manager_list;

static inline void window_manager_set_watermark(struct window_manager_t * wm, const void * buf, int len)
{
	if(wm)
	{
		if(wm->watermark.s)
			surface_free(wm->watermark.s);
		struct surface_t * s = surface_alloc_from_buf(buf, len);
		wm->watermark.s = s;
		if(s)
			region_init(&wm->watermark.r, (framebuffer_get_width(wm->fb) - surface_get_width(s)) / 2, (framebuffer_get_height(wm->fb) - surface_get_height(s)) / 2, surface_get_width(s), surface_get_height(s));
		else
			region_init(&wm->watermark.r, 0, 0, 0, 0);
	}
}

static inline void window_manager_set_cursor(struct window_manager_t * wm, const void * buf, int len)
{
	if(wm)
	{
		struct surface_t * s = surface_alloc_from_buf(buf, len);
		if(s)
		{
			if(wm->cursor.s)
				surface_free(wm->cursor.s);
			wm->cursor.s = s;
			wm->cursor.dirty = 1;
			region_init(&wm->cursor.ro, 0, 0, surface_get_width(s), surface_get_height(s));
			region_init(&wm->cursor.rn, 0, 0, surface_get_width(s), surface_get_height(s));
		}
	}
}

static inline int window_is_active(struct window_t * w)
{
	return list_is_first(&w->list, &w->wm->window);
}

static inline int window_get_width(struct window_t * w)
{
	if(w)
		return framebuffer_get_width(w->wm->fb);
	return 0;
}

static inline int window_get_height(struct window_t * w)
{
	if(w)
		return framebuffer_get_height(w->wm->fb);
	return 0;
}

static inline int window_get_pwidth(struct window_t * w)
{
	if(w)
		return framebuffer_get_pwidth(w->wm->fb);
	return 0;
}

static inline int window_get_pheight(struct window_t * w)
{
	if(w)
		return framebuffer_get_pheight(w->wm->fb);
	return 0;
}

static inline void window_set_backlight(struct window_t * w, int brightness)
{
	if(w)
		framebuffer_set_backlight(w->wm->fb, brightness);
}

static inline int window_get_backlight(struct window_t * w)
{
	if(w)
		return framebuffer_get_backlight(w->wm->fb);
	return 0;
}

struct window_t * window_alloc(const char * fb, const char * input);
void window_free(struct window_t * w);
void window_to_front(struct window_t * w);
void window_to_back(struct window_t * w);
void window_region_list_add(struct window_t * w, struct region_t * r);
void window_region_list_clear(struct window_t * w);
void window_present(struct window_t * w, void * o, void (*draw)(struct window_t *, void *));
void window_exit(struct window_t * w);
int window_pump_event(struct window_t * w, struct event_t * e);
void push_event(struct event_t * e);

#ifdef __cplusplus
}
#endif

#endif /* __WINDOW_H__ */
