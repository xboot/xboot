#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <cairo-xboot.h>

struct region_t {
	double x, y;
	double w, h;
};

struct dirty_region_t {
	struct region_t * region;
	int count;
	int size;
};

struct display_t
{
	struct framebuffer_t * fb;
	struct dirty_region_t * dr;
	cairo_surface_t * cursor;
	cairo_surface_t * cs;
	cairo_t * cr;

	int xpos, ypos;
	int showcur;
	int showobj;
	int showfps;
	double fps;
	uint64_t frame;
	ktime_t stamp;
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

static inline int display_get_bpp(struct display_t * disp)
{
	if(disp)
		return framebuffer_get_bpp(disp->fb);
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

static inline void display_set_cursor(struct display_t * disp, int x, int y)
{
	if(disp)
	{
		disp->xpos = x;
		disp->ypos = y;
	}
}

static inline void display_get_cursor(struct display_t * disp, int * x, int * y)
{
	if(disp)
	{
		if(x)
			*x = disp->xpos;
		if(y)
			*y = disp->ypos;
	}
}

static inline void display_set_showcur(struct display_t * disp, int show)
{
	if(disp)
		disp->showcur = show ? 1 : 0;
}

static inline int display_get_showcur(struct display_t * disp)
{
	return disp ? disp->showcur : 0;
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

static inline void display_set_showfps(struct display_t * disp, int show)
{
	if(disp)
	{
		if(show && !disp->showfps)
			disp->stamp = ktime_get();
		disp->showfps = show ? 1 : 0;
	}
}

static inline int display_get_showfps(struct display_t * disp)
{
	return disp ? disp->showfps : 0;
}

struct display_t * display_alloc(const char * fb);
void display_free(struct display_t * disp);
void display_present(struct display_t * disp, void * o, void (*draw)(void *, struct display_t *));

#ifdef __cplusplus
}
#endif

#endif /* __DISPLAY_H__ */
