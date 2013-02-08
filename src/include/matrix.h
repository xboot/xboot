#ifndef __MATRIX_H__
#define __MATRIX_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <math.h>

/*
 * | [a] [b] [x] |
 * | [c] [d] [y] |
 * | [0] [0] [1] |
 */
struct matrix_transform_t {
	float a, b, x;
	float c, d, y;
};

void matrix_transform_init(struct matrix_transform_t * m, float a, float b, float x, float c, float d, float y);
void matrix_transform_init_identity(struct matrix_transform_t * m);
void matrix_transform_init_translate(struct matrix_transform_t * m, float tx, float ty);
void matrix_transform_init_scale(struct matrix_transform_t * m, float sx, float sy);
void matrix_transform_init_rotate(struct matrix_transform_t * m, float c, float s);
void matrix_transform_init_shear(struct matrix_transform_t * m, float x, float y);
void matrix_transform_multiply(struct matrix_transform_t * m, const struct matrix_transform_t * m1, const struct matrix_transform_t * m2);
void matrix_transform_invert(struct matrix_transform_t * m, const struct matrix_transform_t * i);
void matrix_transform_translate(struct matrix_transform_t * m, float tx, float ty);
void matrix_transform_scale(struct matrix_transform_t * m, float sx, float sy);
void matrix_transform_rotate(struct matrix_transform_t * m, float c, float s);
void matrix_transform_shear(struct matrix_transform_t * m, float x, float y);
void matrix_transform_distance(const struct matrix_transform_t * m, float * dx, float * dy);
void matrix_transform_point(const struct matrix_transform_t * m, float * x, float * y);

#ifdef __cplusplus
}
#endif

#endif /* __MATRIX_H__ */
