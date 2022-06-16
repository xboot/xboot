#ifndef __GRAPHIC_SURFACE_H__
#define __GRAPHIC_SURFACE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <stdint.h>
#include <graphic/point.h>
#include <graphic/region.h>
#include <graphic/color.h>
#include <graphic/matrix.h>
#include <graphic/expblur.h>
#include <graphic/text.h>
#include <graphic/icon.h>
#include <xfs/xfs.h>

struct surface_t;
struct render_t;

/*
 * Each pixel is a 32-bits, with alpha in the upper 8 bits, then red green and blue.
 * The 32-bit quantities are stored native-endian, Pre-multiplied alpha is used.
 * That is, 50% transparent red is 0x80800000 not 0x80ff0000.
 */
struct surface_t
{
	int width;
	int height;
	int stride;
	int pixlen;
	void * pixels;
	struct render_t * r;
	void * rctx;
	void * priv;
};

struct render_t
{
	char * name;
	struct list_head list;

	void * (*create)(struct surface_t * s);
	void (*destroy)(void * rctx);

	void (*blit)(struct surface_t * s, struct region_t * clip, struct matrix_t * m, struct surface_t * src);
	void (*fill)(struct surface_t * s, struct region_t * clip, struct matrix_t * m, int w, int h, struct color_t * c);

	void (*shape_save)(struct surface_t * s);
	void (*shape_restore)(struct surface_t * s);
	void (*shape_set_source_color)(struct surface_t * s, struct color_t * c);
	void (*shape_set_source_surface)(struct surface_t * s, struct surface_t * o, double x, double y);
	void (*shape_set_line_width)(struct surface_t * s, double w);
	void (*shape_set_matrix)(struct surface_t * s, struct matrix_t * m);
	void (*shape_new_path)(struct surface_t * s);
	void (*shape_close_path)(struct surface_t * s);
	void (*shape_move_to)(struct surface_t * s, double x, double y);
	void (*shape_line_to)(struct surface_t * s, double x, double y);
	void (*shape_curve_to)(struct surface_t * s, double x1, double y1, double x2, double y2, double x3, double y3);
	void (*shape_rectangle)(struct surface_t * s, double x, double y, double w, double h);
	void (*shape_arc)(struct surface_t * s, double cx, double cy, double r, double a0, double a1);
	void (*shape_arc_negative)(struct surface_t * s, double cx, double cy, double r, double a0, double a1);
	void (*shape_circle)(struct surface_t * s, double cx, double cy, double r);
	void (*shape_ellipse)(struct surface_t * s, double cx, double cy, double rx, double ry);
	void (*shape_clip)(struct surface_t * s);
	void (*shape_clip_preserve)(struct surface_t * s);
	void (*shape_fill)(struct surface_t * s);
	void (*shape_fill_preserve)(struct surface_t * s);
	void (*shape_stroke)(struct surface_t * s);
	void (*shape_stroke_preserve)(struct surface_t * s);
	void (*shape_paint)(struct surface_t * s);
};

static inline int surface_get_width(struct surface_t * s)
{
	return s->width;
}

static inline int surface_get_height(struct surface_t * s)
{
	return s->height;
}

static inline int surface_get_stride(struct surface_t * s)
{
	return s->stride;
}

static inline void * surface_get_pixels(struct surface_t * s)
{
	return s->pixels;
}

static inline void surface_blit(struct surface_t * s, struct region_t * clip, struct matrix_t * m, struct surface_t * src)
{
	s->r->blit(s, clip, m, src);
}

static inline void surface_fill(struct surface_t * s, struct region_t * clip, struct matrix_t * m, int w, int h, struct color_t * c)
{
	s->r->fill(s, clip, m, w, h, c);
}

static inline void surface_shape_save(struct surface_t * s)
{
	s->r->shape_save(s);
}

static inline void surface_shape_restore(struct surface_t * s)
{
	s->r->shape_restore(s);
}

static inline void surface_shape_set_source_color(struct surface_t * s, struct color_t * c)
{
	s->r->shape_set_source_color(s, c);
}

static inline void surface_shape_set_source_surface(struct surface_t * s, struct surface_t * o, double x, double y)
{
	s->r->shape_set_source_surface(s, o, x, y);
}

static inline void surface_shape_set_line_width(struct surface_t * s, double w)
{
	s->r->shape_set_line_width(s, w);
}

static inline void surface_shape_set_matrix(struct surface_t * s, struct matrix_t * m)
{
	s->r->shape_set_matrix(s, m);
}

static inline void surface_shape_new_path(struct surface_t * s)
{
	s->r->shape_new_path(s);
}

static inline void surface_shape_close_path(struct surface_t * s)
{
	s->r->shape_close_path(s);
}

static inline void surface_shape_move_to(struct surface_t * s, double x, double y)
{
	s->r->shape_move_to(s, x, y);
}

static inline void surface_shape_line_to(struct surface_t * s, double x, double y)
{
	s->r->shape_line_to(s, x, y);
}

static inline void surface_shape_curve_to(struct surface_t * s, double x1, double y1, double x2, double y2, double x3, double y3)
{
	s->r->shape_curve_to(s, x1, y1, x2, y2, x3, y3);
}

static inline void surface_shape_rectangle(struct surface_t * s, double x, double y, double w, double h)
{
	s->r->shape_rectangle(s, x, y, w, h);
}

static inline void surface_shape_arc(struct surface_t * s, double cx, double cy, double r, double a0, double a1)
{
	s->r->shape_arc(s, cx, cy, r, a0, a1);
}

static inline void surface_shape_arc_negative(struct surface_t * s, double cx, double cy, double r, double a0, double a1)
{
	s->r->shape_arc_negative(s, cx, cy, r, a0, a1);
}

static inline void surface_shape_circle(struct surface_t * s, double cx, double cy, double r)
{
	s->r->shape_circle(s, cx, cy, r);
}

static inline void surface_shape_ellipse(struct surface_t * s, double cx, double cy, double rx, double ry)
{
	s->r->shape_ellipse(s, cx, cy, rx, ry);
}

static inline void surface_shape_clip(struct surface_t * s)
{
	s->r->shape_clip(s);
}

static inline void surface_shape_clip_preserve(struct surface_t * s)
{
	s->r->shape_clip_preserve(s);
}

static inline void surface_shape_fill(struct surface_t * s)
{
	s->r->shape_fill(s);
}

static inline void surface_shape_fill_preserve(struct surface_t * s)
{
	s->r->shape_fill_preserve(s);
}

static inline void surface_shape_stroke(struct surface_t * s)
{
	s->r->shape_stroke(s);
}

static inline void surface_shape_stroke_preserve(struct surface_t * s)
{
	s->r->shape_stroke_preserve(s);
}

static inline void surface_shape_paint(struct surface_t * s)
{
	s->r->shape_paint(s);
}

struct render_t * search_render(void);
bool_t register_render(struct render_t * r);
bool_t unregister_render(struct render_t * r);
struct surface_t * surface_alloc(int width, int height, void * priv);
struct surface_t * surface_alloc_from_xfs(struct xfs_context_t * ctx, const char * filename);
struct surface_t * surface_alloc_from_buf(const void * buf, int len);
struct surface_t * surface_alloc_qrcode(const char * txt, int pixsz);
void surface_free(struct surface_t * s);

struct surface_t * surface_clone(struct surface_t * s, int x, int y, int w, int h);
struct surface_t * surface_extend(struct surface_t * s, int width, int height, const char * type);
void surface_clear(struct surface_t * s, struct color_t * c, int x, int y, int w, int h);
void surface_set_pixel(struct surface_t * s, int x, int y, struct color_t * c);
void surface_get_pixel(struct surface_t * s, int x, int y, struct color_t * c);

void surface_text(struct surface_t * s, struct region_t * clip, struct matrix_t * m, struct text_t * txt);
void surface_icon(struct surface_t * s, struct region_t * clip, struct matrix_t * m, struct icon_t * ico);
void surface_effect_glass(struct surface_t * s, struct region_t * clip, int x, int y, int w, int h, int radius);
void surface_effect_shadow(struct surface_t * s, struct region_t * clip, int x, int y, int w, int h, int radius, struct color_t * c);
void surface_effect_gradient(struct surface_t * s, struct region_t * clip, int x, int y, int w, int h, struct color_t * lt, struct color_t * rt, struct color_t * rb, struct color_t * lb);
void surface_effect_checkerboard(struct surface_t * s, struct region_t * clip, int x, int y, int w, int h);
void surface_filter_gray(struct surface_t * s);
void surface_filter_sepia(struct surface_t * s);
void surface_filter_invert(struct surface_t * s);
void surface_filter_coloring(struct surface_t * s, struct color_t * c);
void surface_filter_hue(struct surface_t * s, int angle);
void surface_filter_saturate(struct surface_t * s, int saturate);
void surface_filter_brightness(struct surface_t * s, int brightness);
void surface_filter_contrast(struct surface_t * s, int contrast);
void surface_filter_opacity(struct surface_t * s, int alpha);
void surface_filter_haldclut(struct surface_t * s, struct surface_t * clut, const char * type);
void surface_filter_blur(struct surface_t * s, int radius);

#ifdef __cplusplus
}
#endif

#endif /* __GRAPHIC_SURFACE_H__ */
