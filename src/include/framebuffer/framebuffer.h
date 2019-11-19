#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <graphic/surface.h>

struct framebuffer_t
{
	/* Framebuffer name */
	char * name;

	/* The width and height in pixel */
	int width, height;

	/* The physical size in millimeter */
	int pwidth, pheight;

	/* Set backlight brightness */
	void (*setbl)(struct framebuffer_t * fb, int brightness);

	/* Get backlight brightness */
	int (*getbl)(struct framebuffer_t * fb);

	/* Create a surface */
	struct surface_t * (*create)(struct framebuffer_t * fb);

	/* Destroy a surface */
	void (*destroy)(struct framebuffer_t * fb, struct surface_t * s);

	/* Present a surface */
	void (*present)(struct framebuffer_t * fb, struct surface_t * s, struct region_list_t * rl);

	/* Private data */
	void * priv;
};

static inline void present_surface(void * vram, struct surface_t * s, struct region_list_t * rl)
{
	struct region_t * r;
	unsigned char * p, * q;
	int count = rl->count;
	int stride = s->stride;
	int offset, line, height;
	int i, j;

	for(i = 0; i < count; i++)
	{
		r = &rl->region[i];
		offset = r->y * stride + (r->x << 2);
		line = r->w << 2;
		height = r->h;

		p = (unsigned char *)vram + offset;
		q = (unsigned char *)s->pixels + offset;
		for(j = 0; j < height; j++, p += stride, q += stride)
			memcpy(p, q, line);
	}
}

static inline int framebuffer_get_width(struct framebuffer_t * fb)
{
	return fb->width;
}

static inline int framebuffer_get_height(struct framebuffer_t * fb)
{
	return fb->height;
}

static inline int framebuffer_get_pwidth(struct framebuffer_t * fb)
{
	return fb->pwidth;
}

static inline int framebuffer_get_pheight(struct framebuffer_t * fb)
{
	return fb->pheight;
}

static inline struct surface_t * framebuffer_create_surface(struct framebuffer_t * fb)
{
	return fb->create(fb);
}

static inline void framebuffer_destroy_surface(struct framebuffer_t * fb, struct surface_t * s)
{
	fb->destroy(fb, s);
}

static inline void framebuffer_present_surface(struct framebuffer_t * fb, struct surface_t * s, struct region_list_t * rl)
{
	fb->present(fb, s, rl);
}

struct framebuffer_t * search_framebuffer(const char * name);
struct framebuffer_t * search_first_framebuffer(void);
struct device_t * register_framebuffer(struct framebuffer_t * fb, struct driver_t * drv);
void unregister_framebuffer(struct framebuffer_t * fb);

void framebuffer_set_backlight(struct framebuffer_t * fb, int brightness);
int framebuffer_get_backlight(struct framebuffer_t * fb);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEBUFFER_H__ */
