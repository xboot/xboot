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
#include <framebuffer/framebuffer.h>

struct window_manager_t {
	spinlock_t lock;
	struct list_head list;
	struct list_head window;
	struct framebuffer_t * fb;
	struct fifo_t * event;
	int wcount;
	int refresh;
	struct {
		struct surface_t * s;
		struct region_t ro;
		struct region_t rn;
		int dirty;
		int show;
	} cursor;
};

struct window_t {
	struct list_head list;
	struct window_manager_t * wm;
	struct surface_t * s;
	struct region_list_t * rl;
	struct hmap_t * map;
	int launcher;
	void * priv;
};

extern struct list_head __window_manager_list;

static inline int window_is_active(struct window_t * w)
{
	return list_is_last(&w->list, &w->wm->window);
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

static inline void window_set_launcher(struct window_t * w, int enable)
{
	if(w)
		w->launcher = enable ? 1 : 0;
}

static inline int window_get_launcher(struct window_t * w)
{
	return w ? w->launcher : 0;
}

struct window_t * window_alloc(const char * fb, const char * input, void * data);
void window_free(struct window_t * w);
void window_to_front(struct window_t * w);
void window_to_back(struct window_t * w);
void window_region_list_add(struct window_t * w, struct region_t * r);
void window_region_list_clear(struct window_t * w);
void window_present(struct window_t * w, struct color_t * c, void * o, void (*draw)(struct window_t *, void *));
int window_pump_event(struct window_t * w, struct event_t * e);
void push_event(struct event_t * e);

#ifdef __cplusplus
}
#endif

#endif /* __WINDOW_H__ */
