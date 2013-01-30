#ifndef __GRAPHIC_SURFACE_H__
#define __GRAPHIC_SURFACE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <graphic/pixel.h>
#include <graphic/image.h>
#include <graphic/maps.h>
#include <graphic/maps/software.h>

struct surface_t;
struct surface_maps;

enum surface_pixels {
	SURFACE_PIXELS_NEEDFREE	= 0x0,
	SURFACE_PIXELS_DONTFREE	= 0x1,
};

/*
 * The GIMP c-source image
 */
struct gimage
{
	/* the width of image */
	u32_t width;

	/* the height of image */
	u32_t height;

	/*
	 * bytes per pixel
	 * 3:RGB  - PIXEL_FORMAT_BGR_888
	 * 4:RGBA - PIXEL_FORMAT_ABGR_8888
	 */
	u32_t bytes_per_pixel;

	/* pixel data */
	u8_t * pixels;
};

struct surface_t {
	/* surface pixel information */
	struct pixel_info info;

	/* surface's width and height */
	u32_t w, h;

	/* pitch of one scan line */
	u32_t pitch;

	/* surface pixels flag */
	enum surface_pixels flag;

	/* pointer to pixel data */
	void * pixels;

    /* clipping information */
	struct rect_t clip;

	/* surface map functions */
	struct surface_maps maps;

	/* private data */
	void * priv;
};

struct surface_t * surface_alloc(void * pixels, u32_t w, u32_t h, enum pixel_format fmt);
struct surface_t * surface_alloc_from_gimage(const struct gimage * image);
void surface_free(struct surface_t * surface);

bool_t surface_set_clip_rect(struct surface_t * surface, struct rect_t * rect);
bool_t surface_get_clip_rect(struct surface_t * surface, struct rect_t * rect);
u32_t surface_map_color(struct surface_t * surface, struct color_t * col);
void surface_unmap_color(struct surface_t * surface, u32_t c, struct color_t * col);
bool_t surface_fill(struct surface_t * surface, struct rect_t * rect, u32_t c, enum blend_mode mode);
bool_t surface_blit(struct surface_t * dst, struct rect_t * dst_rect, struct surface_t * src, struct rect_t * src_rect, enum blend_mode mode);
struct surface_t * surface_scale(struct surface_t * surface, struct rect_t * rect, u32_t w, u32_t h);
struct surface_t * surface_rotate(struct surface_t * surface, struct rect_t * rect, enum rotate_type type);

#ifdef __cplusplus
}
#endif

#endif /* __GRAPHIC_SURFACE_H__ */
