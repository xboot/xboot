#ifndef __GRAPHIC_SURFACE_H__
#define __GRAPHIC_SURFACE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <stdint.h>

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
	void * pixels;
	struct surface_operate_t * op;
	void * priv;
};

struct surface_operate_t
{
	void * (*create)(struct surface_t * s);
	void (*destroy)(void * priv);

	void (*blit)(struct surface_t * s, struct matrix_t * m, struct surface_t * src, double alpha);
	void (*mask)(struct surface_t * s, struct matrix_t * m, struct surface_t * src, struct surface_t * mask);
	void (*fill)(struct surface_t * s, struct matrix_t * m, double x, double y, double w, double h, double r, double g, double b, double a);

	void (*filter_grayscale)(struct surface_t * s);
	void (*filter_sepia)(struct surface_t * s);
	void (*filter_invert)(struct surface_t * s);
	void (*filter_threshold)(struct surface_t * s, const char * type, unsigned char threshold, unsigned char value);
	void (*filter_colorize)(struct surface_t * s, const char * type);
	void (*filter_gamma)(struct surface_t * s, double gamma);
	void (*filter_hue)(struct surface_t * s, int angle);
	void (*filter_saturate)(struct surface_t * s, int saturate);
	void (*filter_brightness)(struct surface_t * s, int brightness);
	void (*filter_contrast)(struct surface_t * s, int contrast);
	void (*filter_blur)(struct surface_t * s, int radius);

	void (*shape_save)(struct surface_t * s);
	void (*shape_restore)(struct surface_t * s);
#if 0
	void (*push_group)(struct surface_t * s);
	void (*pop_group)(struct surface_t * s);
	void (*pop_group_to_source)(struct surface_t * s);
	void (*new_path)(struct surface_t * s);
	void (*new_sub_path)(struct surface_t * s);
	void (*close_path)(struct surface_t * s);
	void (*set_operator)(struct surface_t * s, const char * type);
	void (*set_source)(struct surface_t * s); //todo
	void (*set_source_color)(struct surface_t * s, double r, double g, double b, double a);
	void (*set_tolerance)(struct surface_t * s, double tolerance);
	void (*set_miter_limit)(struct surface_t * s, double limit);
	void (*set_antialias)(struct surface_t * s, const char * type);
	void (*set_fill_rule)(struct surface_t * s, const char * type);
	void (*set_line_width)(struct surface_t * s, double width);
	void (*set_line_cap)(struct surface_t * s, const char * type);
	void (*set_line_join)(struct surface_t * s, const char * type);
	void (*set_dash)(struct surface_t * s, const double * dashes, int ndashes, double offset);
	void (*move_to)(struct surface_t * s, double x, double y);
	void (*rel_move_to)(struct surface_t * s, double x, double y);
	void (*line_to)(struct surface_t * s, double x, double y);
	void (*rel_line_to)(struct surface_t * s, double x, double y);
	void (*curve_to)(struct surface_t * s, double x1, double y1, double x2, double y2, double x3, double y3);
	void (*rel_curve_to)(struct surface_t * s, double x1, double y1, double x2, double y2, double x3, double y3);
	void (*rectangle)(struct surface_t * s, double x, double y, double w, double h);
	void (*rounded_rectangle)(struct surface_t * s, double x, double y, double w, double h, double r);
	void (*arc)(struct surface_t * s, double xc, double yc, double radius, double angle1, double angle2);
	void (*arc_negative)(struct surface_t * s, double xc, double yc, double radius, double angle1, double angle2);
	void (*stroke)(struct surface_t * s);
	void (*stroke_preserve)(struct surface_t * s);
	void (*fill)(struct surface_t * s);
	void (*fill_preserve)(struct surface_t * s);
	void (*clip)(struct surface_t * s);
	void (*clip_preserve)(struct surface_t * s);
	void (*mask)(struct surface_t * s); //todo
	void (*paint)(struct surface_t * s);
#endif
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

static inline void surface_blit(struct surface_t * s, struct matrix_t * m, struct surface_t * src, double alpha)
{
	s->op->blit(s, m, src, alpha);
}

static inline void surface_mask(struct surface_t * s, struct matrix_t * m, struct surface_t * src, struct surface_t * mask)
{
	s->op->mask(s, m, src, mask);
}

static inline void surface_fill(struct surface_t * s, struct matrix_t * m, double x, double y, double w, double h, double r, double g, double b, double a)
{
	s->op->fill(s, m, x, y, w, h, r, g, b, a);
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

static inline void surface_filter_threshold(struct surface_t * s, const char * type, unsigned char threshold, unsigned char value)
{
	s->op->filter_threshold(s, type, threshold, value);
}

static inline void surface_filter_colorize(struct surface_t * s, const char * type)
{
	s->op->filter_colorize(s, type);
}

static inline void surface_filter_gamma(struct surface_t * s, double gamma)
{
	s->op->filter_gamma(s, gamma);
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

struct surface_t * surface_alloc(int width, int height);
struct surface_t * surface_alloc_from_jpeg(const char * filename);
struct surface_t * surface_alloc_from_jpeg_data(void * buffer, int length);
void surface_free(struct surface_t * s);

#ifdef __cplusplus
}
#endif

#endif /* __GRAPHIC_SURFACE_H__ */
