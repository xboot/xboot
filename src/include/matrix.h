#ifndef __MATRIX_H__
#define __MATRIX_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * | [xx] [yx] [0] |
 * | [xy] [yy] [0] |
 * | [x0] [y0] [1] |
 */
struct matrix_t {
    double xx; double yx;
    double xy; double yy;
    double x0; double y0;
};

void matrix_init(struct matrix_t * m, double xx, double yx, double xy, double yy, double x0, double y0);
void matrix_init_identity(struct matrix_t * m);
void matrix_init_translate(struct matrix_t * m, double tx, double ty);
void matrix_init_scale(struct matrix_t * m, double sx, double sy);
void matrix_init_rotate(struct matrix_t * m, double r);
void matrix_multiply(struct matrix_t * m, const struct matrix_t * m1, const struct matrix_t * m2);
void matrix_invert(struct matrix_t * m);
void matrix_translate(struct matrix_t * m, double tx, double ty);
void matrix_scale(struct matrix_t * m, double sx, double sy);
void matrix_rotate(struct matrix_t * m, double r);
void matrix_transform_distance(const struct matrix_t * m, double * dx, double * dy);
void matrix_transform_point(const struct matrix_t * m, double * x, double * y);
void matrix_transform_bounds(const struct matrix_t * m, double * x1, double * y1, double * x2, double * y2);

#ifdef __cplusplus
}
#endif

#endif /* __MATRIX_H__ */
