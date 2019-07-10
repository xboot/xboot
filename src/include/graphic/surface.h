#ifndef __GRAPHIC_SURFACE_H__
#define __GRAPHIC_SURFACE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <stdint.h>
#include <graphic/color.h>
#include <graphic/region.h>
#include <graphic/matrix.h>
#include <graphic/font.h>
#include <xfs/xfs.h>

struct surface_t;
struct surface_operate_t;

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
	struct surface_operate_t * op;
	void * pctx;
	void * priv;
};

struct surface_operate_t
{
	void * (*create)(struct surface_t * s);
	void (*destroy)(void * pctx);

	void (*blit)(struct surface_t * s, struct matrix_t * m, struct surface_t * src, double alpha);
	void (*mask)(struct surface_t * s, struct matrix_t * m, struct surface_t * src, struct surface_t * mask);
	void (*fill)(struct surface_t * s, struct matrix_t * m, struct region_t * r, struct color_t * c);
	void (*text)(struct surface_t * s, struct matrix_t * m, const char * utf8, struct color_t * c, void * sfont, int size);
	void (*extent)(struct surface_t * s, const char * utf8, void * sfont, int size, struct region_t * e);

	void (*filter_grayscale)(struct surface_t * s);
	void (*filter_sepia)(struct surface_t * s);
	void (*filter_invert)(struct surface_t * s);
	void (*filter_threshold)(struct surface_t * s, const char * type, int threshold, int value);
	void (*filter_colorize)(struct surface_t * s, const char * type);
	void (*filter_hue)(struct surface_t * s, int angle);
	void (*filter_saturate)(struct surface_t * s, int saturate);
	void (*filter_brightness)(struct surface_t * s, int brightness);
	void (*filter_contrast)(struct surface_t * s, int contrast);
	void (*filter_blur)(struct surface_t * s, int radius);

	void (*shape_save)(struct surface_t * s);
	void (*shape_restore)(struct surface_t * s);
	void (*shape_push_group)(struct surface_t * s);
	void (*shape_pop_group)(struct surface_t * s);
	void (*shape_pop_group_to_source)(struct surface_t * s);
	void (*shape_new_path)(struct surface_t * s);
	void (*shape_new_sub_path)(struct surface_t * s);
	void (*shape_close_path)(struct surface_t * s);
	void (*shape_set_operator)(struct surface_t * s, const char * type);
	void (*shape_set_source)(struct surface_t * s, void * pattern);
	void * (*shape_get_source)(struct surface_t * s);
	void (*shape_set_source_color)(struct surface_t * s, double r, double g, double b, double a);
	void (*shape_set_source_surface)(struct surface_t * s, struct surface_t * o, double x, double y);
	void (*shape_set_tolerance)(struct surface_t * s, double tolerance);
	void (*shape_set_miter_limit)(struct surface_t * s, double limit);
	void (*shape_set_antialias)(struct surface_t * s, const char * type);
	void (*shape_set_fill_rule)(struct surface_t * s, const char * type);
	void (*shape_set_line_width)(struct surface_t * s, double width);
	void (*shape_set_line_cap)(struct surface_t * s, const char * type);
	void (*shape_set_line_join)(struct surface_t * s, const char * type);
	void (*shape_set_dash)(struct surface_t * s, const double * dashes, int ndashes, double offset);
	void (*shape_identity)(struct surface_t * s);
	void (*shape_translate)(struct surface_t * s, double tx, double ty);
	void (*shape_scale)(struct surface_t * s, double sx, double sy);
	void (*shape_rotate)(struct surface_t * s, double angle);
	void (*shape_transform)(struct surface_t * s, struct matrix_t * m);
	void (*shape_set_matrix)(struct surface_t * s, struct matrix_t * m);
	void (*shape_get_matrix)(struct surface_t * s, struct matrix_t * m);
	void (*shape_move_to)(struct surface_t * s, double x, double y);
	void (*shape_rel_move_to)(struct surface_t * s, double dx, double dy);
	void (*shape_line_to)(struct surface_t * s, double x, double y);
	void (*shape_rel_line_to)(struct surface_t * s, double dx, double dy);
	void (*shape_curve_to)(struct surface_t * s, double x1, double y1, double x2, double y2, double x3, double y3);
	void (*shape_rel_curve_to)(struct surface_t * s, double dx1, double dy1, double dx2, double dy2, double dx3, double dy3);
	void (*shape_rectangle)(struct surface_t * s, double x, double y, double w, double h);
	void (*shape_rounded_rectangle)(struct surface_t * s, double x, double y, double w, double h, double r);
	void (*shape_arc)(struct surface_t * s, double xc, double yc, double r, double a1, double a2);
	void (*shape_arc_negative)(struct surface_t * s, double xc, double yc, double r, double a1, double a2);
	void (*shape_stroke)(struct surface_t * s);
	void (*shape_stroke_preserve)(struct surface_t * s);
	void (*shape_fill)(struct surface_t * s);
	void (*shape_fill_preserve)(struct surface_t * s);
	void (*shape_reset_clip)(struct surface_t * s);
	void (*shape_clip)(struct surface_t * s);
	void (*shape_clip_preserve)(struct surface_t * s);
	void (*shape_mask)(struct surface_t * s, void * pattern);
	void (*shape_mask_surface)(struct surface_t * s, struct surface_t * o, double x, double y);
	void (*shape_paint)(struct surface_t * s, double alpha);

	void * (*font_create)(void * font);
	void (*font_destroy)(void * sfont);

	void * (*pattern_create)(struct surface_t * s);
	void * (*pattern_create_color)(double r, double g, double b, double a);
	void * (*pattern_create_linear)(double x0, double y0, double x1, double y1);
	void * (*pattern_create_radial)(double x0, double y0, double r0, double x1, double y1, double r1);
	void (*pattern_destroy)(void * pattern);
	void (*pattern_add_color_stop)(void * pattern, double o, double r, double g, double b, double a);
	void (*pattern_set_extend)(void * pattern, const char * type);
	void (*pattern_set_filter)(void * pattern, const char * type);
	void (*pattern_set_matrix)(void * pattern, struct matrix_t * m);
};

struct surface_operate_t * surface_operate_get(void);

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

static inline void surface_blit(struct surface_t * s, struct matrix_t * m, struct surface_t * src, double alpha)
{
	s->op->blit(s, m, src, alpha);
}

static inline void surface_mask(struct surface_t * s, struct matrix_t * m, struct surface_t * src, struct surface_t * mask)
{
	s->op->mask(s, m, src, mask);
}

static inline void surface_fill(struct surface_t * s, struct matrix_t * m, struct region_t * r, struct color_t * c)
{
	s->op->fill(s, m, r, c);
}

static inline void surface_text(struct surface_t * s, struct matrix_t * m, const char * utf8, struct color_t * c, void * sfont, int size)
{
	s->op->text(s, m, utf8, c, sfont, size);
}

static inline void surface_extent(struct surface_t * s, const char * utf8, void * sfont, int size, struct region_t * e)
{
	s->op->extent(s, utf8, sfont, size, e);
}

static inline void surface_filter_grayscale(struct surface_t * s)
{
	s->op->filter_grayscale(s);
}

static inline void surface_filter_sepia(struct surface_t * s)
{
	s->op->filter_sepia(s);
}

static inline void surface_filter_invert(struct surface_t * s)
{
	s->op->filter_invert(s);
}

static inline void surface_filter_threshold(struct surface_t * s, const char * type, int threshold, int value)
{
	s->op->filter_threshold(s, type, threshold, value);
}

static inline void surface_filter_colorize(struct surface_t * s, const char * type)
{
	s->op->filter_colorize(s, type);
}

static inline void surface_filter_hue(struct surface_t * s, int angle)
{
	s->op->filter_hue(s, angle);
}

static inline void surface_filter_saturate(struct surface_t * s, int saturate)
{
	s->op->filter_saturate(s, saturate);
}

static inline void surface_filter_brightness(struct surface_t * s, int brightness)
{
	s->op->filter_brightness(s, brightness);
}

static inline void surface_filter_contrast(struct surface_t * s, int contrast)
{
	s->op->filter_contrast(s, contrast);
}

static inline void surface_filter_blur(struct surface_t * s, int radius)
{
	s->op->filter_blur(s, radius);
}

static inline void surface_shape_save(struct surface_t * s)
{
	s->op->shape_save(s);
}

static inline void surface_shape_restore(struct surface_t * s)
{
	s->op->shape_restore(s);
}

static inline void surface_shape_push_group(struct surface_t * s)
{
	s->op->shape_push_group(s);
}

static inline void surface_shape_pop_group(struct surface_t * s)
{
	s->op->shape_pop_group(s);
}

static inline void surface_shape_pop_group_to_source(struct surface_t * s)
{
	s->op->shape_pop_group_to_source(s);
}

static inline void surface_shape_new_path(struct surface_t * s)
{
	s->op->shape_new_path(s);
}

static inline void surface_shape_new_sub_path(struct surface_t * s)
{
	s->op->shape_new_sub_path(s);
}

static inline void surface_shape_close_path(struct surface_t * s)
{
	s->op->shape_close_path(s);
}

static inline void surface_shape_set_operator(struct surface_t * s, const char * type)
{
	s->op->shape_set_operator(s, type);
}

static inline void surface_shape_set_source(struct surface_t * s, void * pattern)
{
	s->op->shape_set_source(s, pattern);
}

static inline void * surface_shape_get_source(struct surface_t * s)
{
	return s->op->shape_get_source(s);
}

static inline void surface_shape_set_source_color(struct surface_t * s, double r, double g, double b, double a)
{
	s->op->shape_set_source_color(s, r, g, b, a);
}

static inline void surface_shape_set_source_surface(struct surface_t * s, struct surface_t * o, double x, double y)
{
	s->op->shape_set_source_surface(s, o, x, y);
}

static inline void surface_shape_set_tolerance(struct surface_t * s, double tolerance)
{
	s->op->shape_set_tolerance(s, tolerance);
}

static inline void surface_shape_set_miter_limit(struct surface_t * s, double limit)
{
	s->op->shape_set_miter_limit(s, limit);
}

static inline void surface_shape_set_antialias(struct surface_t * s, const char * type)
{
	s->op->shape_set_antialias(s, type);
}

static inline void surface_shape_set_fill_rule(struct surface_t * s, const char * type)
{
	s->op->shape_set_fill_rule(s, type);
}

static inline void surface_shape_set_line_width(struct surface_t * s, double width)
{
	s->op->shape_set_line_width(s, width);
}

static inline void surface_shape_set_line_cap(struct surface_t * s, const char * type)
{
	s->op->shape_set_line_cap(s, type);
}

static inline void surface_shape_set_line_join(struct surface_t * s, const char * type)
{
	s->op->shape_set_line_join(s, type);
}

static inline void surface_shape_set_dash(struct surface_t * s, const double * dashes, int ndashes, double offset)
{
	s->op->shape_set_dash(s, dashes, ndashes, offset);
}

static inline void surface_shape_identity(struct surface_t * s)
{
	s->op->shape_identity(s);
}
static inline void surface_shape_translate(struct surface_t * s, double tx, double ty)
{
	s->op->shape_translate(s, tx, ty);
}
static inline void surface_shape_scale(struct surface_t * s, double sx, double sy)
{
	s->op->shape_scale(s, sx, sy);
}
static inline void surface_shape_rotate(struct surface_t * s, double angle)
{
	s->op->shape_rotate(s, angle);
}
static inline void surface_shape_transform(struct surface_t * s, struct matrix_t * m)
{
	s->op->shape_transform(s, m);
}

static inline void surface_shape_set_matrix(struct surface_t * s, struct matrix_t * m)
{
	s->op->shape_set_matrix(s, m);
}

static inline void surface_shape_get_matrix(struct surface_t * s, struct matrix_t * m)
{
	s->op->shape_get_matrix(s, m);
}

static inline void surface_shape_move_to(struct surface_t * s, double x, double y)
{
	s->op->shape_move_to(s, x, y);
}

static inline void surface_shape_rel_move_to(struct surface_t * s, double dx, double dy)
{
	s->op->shape_rel_move_to(s, dx, dy);
}

static inline void surface_shape_line_to(struct surface_t * s, double x, double y)
{
	s->op->shape_line_to(s, x, y);
}

static inline void surface_shape_rel_line_to(struct surface_t * s, double dx, double dy)
{
	s->op->shape_rel_line_to(s, dx, dy);
}

static inline void surface_shape_curve_to(struct surface_t * s, double x1, double y1, double x2, double y2, double x3, double y3)
{
	s->op->shape_curve_to(s, x1, y1, x2, y2, x3, y3);
}

static inline void surface_shape_rel_curve_to(struct surface_t * s, double dx1, double dy1, double dx2, double dy2, double dx3, double dy3)
{
	s->op->shape_rel_curve_to(s, dx1, dy1, dx2, dy2, dx3, dy3);
}

static inline void surface_shape_rectangle(struct surface_t * s, double x, double y, double w, double h)
{
	s->op->shape_rectangle(s, x, y, w, h);
}

static inline void surface_shape_rounded_rectangle(struct surface_t * s, double x, double y, double w, double h, double r)
{
	s->op->shape_rounded_rectangle(s, x, y, w, h, r);
}

static inline void surface_shape_arc(struct surface_t * s, double xc, double yc, double r, double a1, double a2)
{
	s->op->shape_arc(s, xc, yc, r, a1, a2);
}

static inline void surface_shape_arc_negative(struct surface_t * s, double xc, double yc, double r, double a1, double a2)
{
	s->op->shape_arc_negative(s, xc, yc, r, a1, a2);
}

static inline void surface_shape_stroke(struct surface_t * s)
{
	s->op->shape_stroke(s);
}

static inline void surface_shape_stroke_preserve(struct surface_t * s)
{
	s->op->shape_stroke_preserve(s);
}

static inline void surface_shape_fill(struct surface_t * s)
{
	s->op->shape_fill(s);
}

static inline void surface_shape_fill_preserve(struct surface_t * s)
{
	s->op->shape_fill_preserve(s);
}

static inline void surface_shape_reset_clip(struct surface_t * s)
{
	s->op->shape_reset_clip(s);
}

static inline void surface_shape_clip(struct surface_t * s)
{
	s->op->shape_clip(s);
}

static inline void surface_shape_clip_preserve(struct surface_t * s)
{
	s->op->shape_clip_preserve(s);
}

static inline void surface_shape_mask(struct surface_t * s, void * pattern)
{
	s->op->shape_mask(s, pattern);
}

static inline void surface_shape_mask_surface(struct surface_t * s, struct surface_t * o, double x, double y)
{
	s->op->shape_mask_surface(s, o, x, y);
}

static inline void surface_shape_paint(struct surface_t * s, double alpha)
{
	s->op->shape_paint(s, alpha);
}

static inline void * surface_font_create(void * font)
{
	return surface_operate_get()->font_create(font);
}

static inline void surface_font_destroy(void * sfont)
{
	surface_operate_get()->font_destroy(sfont);
}

static inline void * surface_pattern_create(struct surface_t * s)
{
	return surface_operate_get()->pattern_create(s);
}

static inline void * surface_pattern_create_color(double r, double g, double b, double a)
{
	return surface_operate_get()->pattern_create_color(r, g, b, a);
}

static inline void * surface_pattern_create_linear(double x0, double y0, double x1, double y1)
{
	return surface_operate_get()->pattern_create_linear(x0, y0, x1, y1);
}

static inline void * surface_pattern_create_radial(double x0, double y0, double r0, double x1, double y1, double r1)
{
	return surface_operate_get()->pattern_create_radial(x0, y0, r0, x1, y1, r1);
}

static inline void surface_pattern_destroy(void * pattern)
{
	surface_operate_get()->pattern_destroy(pattern);
}

static inline void surface_pattern_add_color_stop(void * pattern, double o, double r, double g, double b, double a)
{
	surface_operate_get()->pattern_add_color_stop(pattern, o, r, g, b, a);
}

static inline void surface_pattern_set_extend(void * pattern, const char * type)
{
	surface_operate_get()->pattern_set_extend(pattern, type);
}

static inline void surface_pattern_set_filter(void * pattern, const char * type)
{
	surface_operate_get()->pattern_set_filter(pattern, type);
}

static inline void surface_pattern_set_matrix(void * pattern, struct matrix_t * m)
{
	surface_operate_get()->pattern_set_matrix(pattern, m);
}

struct surface_t * surface_alloc(int width, int height, void * priv);
struct surface_t * surface_alloc_from_xfs(struct xfs_context_t * ctx, const char * filename);
struct surface_t * surface_clone(struct surface_t * s);
void surface_free(struct surface_t * s);
void surface_clear(struct surface_t * s);

#ifdef __cplusplus
}
#endif

#endif /* __GRAPHIC_SURFACE_H__ */
