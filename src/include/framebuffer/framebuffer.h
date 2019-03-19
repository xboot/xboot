#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

enum pixel_format_t
{
	PIXEL_FORMAT_ARGB32		= 0,
	PIXEL_FORMAT_RGB24		= 1,
	PIXEL_FORMAT_A8			= 2,
	PIXEL_FORMAT_A1			= 3,
	PIXEL_FORMAT_RGB16_565	= 4,
	PIXEL_FORMAT_RGB30		= 5,
};

struct dirty_rect_t {
	uint32_t x, y;
	uint32_t w, h;
};

struct render_t {
	/* The width of render */
	uint32_t width;

	/* The height of render */
	uint32_t height;

	/* The pitch of one scan line */
	uint32_t pitch;

	/* Pixel format */
	enum pixel_format_t format;

	/* Pixel data */
	void * pixels;

	/* Pixel data length */
	size_t pixlen;

	/* Private data */
	void * priv;
};

struct framebuffer_t
{
	/* Framebuffer name */
	char * name;

	/* The width and height in pixel */
	int width, height;

	/* The physical size in millimeter */
	int pwidth, pheight;

	/* The bit per pixel */
	int bpp;

	/* Set backlight brightness */
	void (*setbl)(struct framebuffer_t * fb, int brightness);

	/* Get backlight brightness */
	int (*getbl)(struct framebuffer_t * fb);

	/* Create a render */
	struct render_t * (*create)(struct framebuffer_t * fb);

	/* Destroy a render */
	void (*destroy)(struct framebuffer_t * fb, struct render_t * render);

	/* Present a render */
	void (*present)(struct framebuffer_t * fb, struct render_t * render, struct dirty_rect_t * rect, int nrect);

	/* Private data */
	void * priv;
};

struct framebuffer_t * search_framebuffer(const char * name);
struct framebuffer_t * search_first_framebuffer(void);
bool_t register_framebuffer(struct device_t ** device, struct framebuffer_t * fb);
bool_t unregister_framebuffer(struct framebuffer_t * fb);

int framebuffer_get_width(struct framebuffer_t * fb);
int framebuffer_get_height(struct framebuffer_t * fb);
int framebuffer_get_pwidth(struct framebuffer_t * fb);
int framebuffer_get_pheight(struct framebuffer_t * fb);
int framebuffer_get_bpp(struct framebuffer_t * fb);
struct render_t * framebuffer_create_render(struct framebuffer_t * fb);
void framebuffer_destroy_render(struct framebuffer_t * fb, struct render_t * render);
void framebuffer_present_render(struct framebuffer_t * fb, struct render_t * render, struct dirty_rect_t * rect, int nrect);
void framebuffer_set_backlight(struct framebuffer_t * fb, int brightness);
int framebuffer_get_backlight(struct framebuffer_t * fb);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEBUFFER_H__ */
