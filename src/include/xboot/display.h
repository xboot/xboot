#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <cairo-xboot.h>

struct display_t
{
	struct framebuffer_t * fb;
	struct region_list_t * rl;
	cairo_surface_t * cs;
	cairo_t * cr;

	struct {
		cairo_surface_t * cs;
		int width, height;
		int ox, oy;
		int nx, ny;
		int dirty;
		int show;
	} cursor;

	struct {
		double rate;
		uint64_t frame;
		ktime_t stamp;
		int show;
	} fps;

	int showobj;
};

static inline int display_get_width(struct display_t * disp)
{
	if(disp)
		return framebuffer_get_width(disp->fb);
	return 0;
}

static inline int display_get_height(struct display_t * disp)
{
	if(disp)
		return framebuffer_get_height(disp->fb);
	return 0;
}

static inline int display_get_pwidth(struct display_t * disp)
{
	if(disp)
		return framebuffer_get_pwidth(disp->fb);
	return 0;
}

static inline int display_get_pheight(struct display_t * disp)
{
	if(disp)
		return framebuffer_get_pheight(disp->fb);
	return 0;
}

static inline int display_get_bytes(struct display_t * disp)
{
	if(disp)
		return framebuffer_get_bytes(disp->fb);
	return 0;
}

static inline void display_set_backlight(struct display_t * disp, int brightness)
{
	if(disp)
		framebuffer_set_backlight(disp->fb, brightness);
}

static inline int display_get_backlight(struct display_t * disp)
{
	if(disp)
		return framebuffer_get_backlight(disp->fb);
	return 0;
}

static inline void display_cursor_set_position(struct display_t * disp, int x, int y)
{
	if(disp)
	{
		if(!disp->cursor.dirty)
		{
			disp->cursor.ox = disp->cursor.nx;
			disp->cursor.oy = disp->cursor.ny;
			disp->cursor.dirty = 1;
		}
		disp->cursor.nx = x;
		disp->cursor.ny = y;
	}
}

static inline void display_cursor_get_position(struct display_t * disp, int * x, int * y)
{
	if(disp)
	{
		if(x)
			*x = disp->cursor.nx;
		if(y)
			*y = disp->cursor.ny;
	}
}

static inline void display_cursor_set_show(struct display_t * disp, int show)
{
	if(disp)
		disp->cursor.show = show ? 1 : 0;
}

static inline int display_cursor_get_show(struct display_t * disp)
{
	return disp ? disp->cursor.show : 0;
}

static inline void display_fps_set_show(struct display_t * disp, int show)
{
	if(disp)
	{
		if(show && !disp->fps.show)
			disp->fps.stamp = ktime_get();
		disp->fps.show = show ? 1 : 0;
	}
}

static inline int display_fps_get_show(struct display_t * disp)
{
	return disp ? disp->fps.show : 0;
}

static inline void display_set_showobj(struct display_t * disp, int show)
{
	if(disp)
		disp->showobj = show ? 1 : 0;
}

static inline int display_get_showobj(struct display_t * disp)
{
	return disp ? disp->showobj : 0;
}

struct display_t * display_alloc(const char * fb);
void display_free(struct display_t * disp);
void display_region_list_add(struct display_t * disp, struct region_t * r);
void display_region_list_clear(struct display_t * disp);
void display_present(struct display_t * disp, void * o, void (*draw)(struct display_t *, void *));

#ifdef __cplusplus
}
#endif

#endif /* __DISPLAY_H__ */
