#ifndef __FB_RENDER_H__
#define __FB_RENDER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <fb/rect.h>
#include <fb/color.h>

enum pixel_format_t
{
	PIXEL_FORMAT_ARGB32		= 0,
	PIXEL_FORMAT_RGB24		= 1,
	PIXEL_FORMAT_A8			= 2,
	PIXEL_FORMAT_A1			= 3,
	PIXEL_FORMAT_RGB16_565	= 4,
	PIXEL_FORMAT_RGB30		= 5,
};

struct texture_t {
	/* the width of render */
	u32_t width;

	/* the height of render */
	u32_t height;

	/* pitch of one scan line */
	u32_t pitch;

	/* pixel format */
	enum pixel_format_t format;

	/* pixel data */
	void * pixels;

	/* private data */
	void * priv;
};

struct render_t {
	/* the width of render */
	u32_t width;

	/* the height of render */
	u32_t height;

	/* pitch of one scan line */
	u32_t pitch;

	/* pixel format */
	enum pixel_format_t format;

	/* pixel data */
	void * pixels;

	/* functions ... */
	struct texture_t * (*alloc)(struct render_t * render, u32_t w, u32_t h);
	void (*free)(struct texture_t * texture);
	void (*fill)(struct render_t * render, struct rect_t * rect, u32_t c);
	void (*blit)(struct render_t * render, struct rect_t * drect, struct texture_t * texture, struct rect_t * srect);
	struct texture_t * (*scale)(struct render_t * render, struct texture_t * texture, u32_t w, u32_t h);
	struct texture_t * (*rotate)(struct render_t * render, struct rect_t * rect, u32_t angle);
};

u32_t render_map_color(struct render_t * render , struct color_t * col);
void render_unmap_color(struct render_t * render, u32_t c, struct color_t * col);
void render_fill(struct render_t * render, struct rect_t * rect, u32_t c);
void render_blit(struct render_t * render, struct rect_t * drect, struct texture_t * texture, struct rect_t * srect);
struct texture_t * render_scale(struct render_t * render, struct texture_t * texture, u32_t w, u32_t h);
struct texture_t * render_rotate(struct render_t * render, struct rect_t * rect, u32_t angle);

#ifdef __cplusplus
}
#endif

#endif /* __FB_RENDER_H__ */
