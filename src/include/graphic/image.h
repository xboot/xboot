#ifndef __IMAGE_H__
#define __IMAGE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <stdint.h>

struct image_t;
struct image_operate_t;

/*
 * Each pixel is a 32-bits, with alpha in the upper 8 bits, then red green and blue.
 * The 32-bit quantities are stored native-endian, Pre-multiplied alpha is used.
 * That is, 50% transparent red is 0x80800000 not 0x80ff0000.
 */
struct image_t
{
	int width;
	int height;
	int stride;
	void * pixels;
	struct image_operate_t * op;
	void * priv;
};

struct image_operate_t
{
	void * (*create)(struct image_t * img);
	void (*destroy)(void * priv);

	void (*blit)(struct image_t * img, struct matrix_t * m, struct image_t * src, double alpha);
	void (*mask)(struct image_t * img, struct matrix_t * m, struct image_t * src, struct image_t * mask);
	void (*fill)(struct image_t * img, struct matrix_t * m, double x, double y, double w, double h, double r, double g, double b, double a);

	void (*shape_save)(struct image_t * img);
	void (*shape_restore)(struct image_t * img);
#if 0
	void (*push_group)(struct image_t * img);
	void (*pop_group)(struct image_t * img);
	void (*pop_group_to_source)(struct image_t * img);
	void (*new_path)(struct image_t * img);
	void (*new_sub_path)(struct image_t * img);
	void (*close_path)(struct image_t * img);
	void (*set_operator)(struct image_t * img, const char * type);
	void (*set_source)(struct image_t * img); //todo
	void (*set_source_color)(struct image_t * img, double r, double g, double b, double a);
	void (*set_tolerance)(struct image_t * img, double tolerance);
	void (*set_miter_limit)(struct image_t * img, double limit);
	void (*set_antialias)(struct image_t * img, const char * type);
	void (*set_fill_rule)(struct image_t * img, const char * type);
	void (*set_line_width)(struct image_t * img, double width);
	void (*set_line_cap)(struct image_t * img, const char * type);
	void (*set_line_join)(struct image_t * img, const char * type);
	void (*set_dash)(struct image_t * img, const double * dashes, int ndashes, double offset);
	void (*move_to)(struct image_t * img, double x, double y);
	void (*rel_move_to)(struct image_t * img, double x, double y);
	void (*line_to)(struct image_t * img, double x, double y);
	void (*rel_line_to)(struct image_t * img, double x, double y);
	void (*curve_to)(struct image_t * img, double x1, double y1, double x2, double y2, double x3, double y3);
	void (*rel_curve_to)(struct image_t * img, double x1, double y1, double x2, double y2, double x3, double y3);
	void (*rectangle)(struct image_t * img, double x, double y, double w, double h);
	void (*rounded_rectangle)(struct image_t * img, double x, double y, double w, double h, double r);
	void (*arc)(struct image_t * img, double xc, double yc, double radius, double angle1, double angle2);
	void (*arc_negative)(struct image_t * img, double xc, double yc, double radius, double angle1, double angle2);
	void (*stroke)(struct image_t * img);
	void (*stroke_preserve)(struct image_t * img);
	void (*fill)(struct image_t * img);
	void (*fill_preserve)(struct image_t * img);
	void (*clip)(struct image_t * img);
	void (*clip_preserve)(struct image_t * img);
	void (*mask)(struct image_t * img); //todo
	void (*paint)(struct image_t * img);
#endif
};

static inline int image_get_width(struct image_t * img)
{
	return img->width;
}

static inline int image_get_height(struct image_t * img)
{
	return img->height;
}

static inline int image_get_stride(struct image_t * img)
{
	return img->stride;
}

static inline void * image_get_pixels(struct image_t * img)
{
	return img->pixels;
}

static inline void image_blit(struct image_t * img, struct matrix_t * m, struct image_t * src, double alpha)
{
	img->op->blit(img, m, src, alpha);
}

static inline void image_mask(struct image_t * img, struct matrix_t * m, struct image_t * src, struct image_t * mask)
{
	img->op->mask(img, m, src, mask);
}

static inline void image_fill(struct image_t * img, struct matrix_t * m, double x, double y, double w, double h, double r, double g, double b, double a)
{
	img->op->fill(img, m, x, y, w, h, r, g, b, a);
}

static inline void image_shape_save(struct image_t * img)
{
	img->op->shape_save(img);
}

static inline void image_shape_restore(struct image_t * img)
{
	img->op->shape_restore(img);
}

struct image_t * image_alloc(int width, int height);
void image_free(struct image_t * img);

#ifdef __cplusplus
}
#endif

#endif /* __IMAGE_H__ */
