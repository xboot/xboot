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

/*
 * The GIMP c-source image
 */
struct gimage_t
{
	/* The width of image */
	u32_t width;

	/* The height of image */
	u32_t height;

	/*
	 * Bytes per pixel
	 *
	 * 3:RGB  - PIXEL_FORMAT_RGB24
	 * 4:RGBA - PIXEL_FORMAT_ARGB32
	 */
	u32_t bytes_per_pixel;

	/* pixel data */
	u8_t * pixels;
};

struct texture_t {
	/* The width of render */
	u32_t width;

	/* The height of render */
	u32_t height;

	/* The pitch of one scan line */
	u32_t pitch;

	/* Pixel format */
	enum pixel_format_t format;

	/* Pixel data */
	void * pixels;

	/* Private data */
	void * priv;
};

struct render_t {
	/* The width of render */
	u32_t width;

	/* The height of render */
	u32_t height;

	/* The pitch of one scan line */
	u32_t pitch;

	/* Pixel format */
	enum pixel_format_t format;

	/* Pixel data */
	void * pixels;

	/* Pixel data length */
	size_t pixlen;

	/* Private data */
	void * priv;

	/* Low level functions, just for console */
	void (*clear)(struct render_t * render, struct rect_t * r, struct color_t * c);
	struct texture_t * (*snapshot)(struct render_t * render);
	struct texture_t * (*alloc_texture)(struct render_t * render, void * pixels, u32_t w, u32_t h, enum pixel_format_t format);
	struct texture_t * (*alloc_texture_similar)(struct render_t * render, u32_t w, u32_t h);
	void (*free_texture)(struct render_t * render, struct texture_t * texture);
	void (*fill_texture)(struct render_t * render, struct texture_t * texture, struct rect_t * r, struct color_t * c);
	void (*blit_texture)(struct render_t * render, struct rect_t * drect, struct texture_t * texture, struct rect_t * srect);
	void * data;
};

void render_clear(struct render_t * render, struct rect_t * r, struct color_t * c);
struct texture_t * render_snapshot(struct render_t * render);
struct texture_t * render_alloc_texture(struct render_t * render, void * pixels, u32_t w, u32_t h, enum pixel_format_t format);
struct texture_t * render_alloc_texture_similar(struct render_t * render, u32_t w, u32_t h);
struct texture_t * render_alloc_texture_from_gimage(struct render_t * render, const struct gimage_t * image);
void render_free_texture(struct render_t * render, struct texture_t * texture);
void render_fill_texture(struct render_t * render, struct texture_t * texture, struct rect_t * r, struct color_t * c);
void render_blit_texture(struct render_t * render, struct rect_t * drect, struct texture_t * texture, struct rect_t * srect);

#ifdef __cplusplus
}
#endif

#endif /* __FB_RENDER_H__ */
