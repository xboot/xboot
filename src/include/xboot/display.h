#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <cairo-xboot.h>

struct display_t
{
	struct framebuffer_t * fb;
	cairo_surface_t * cs;
	cairo_t * cr;

	int showobj;
	int showfps;
	double fps;
	uint64_t frame;
	ktime_t stamp;
};

struct display_t * display_alloc(const char * fb);
void display_free(struct display_t * d);

#ifdef __cplusplus
}
#endif

#endif /* __DISPLAY_H__ */
