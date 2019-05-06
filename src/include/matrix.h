#ifndef __MATRIX_H__
#define __MATRIX_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * | [a]  [b]  [0] |
 * | [c]  [d]  [0] |
 * | [tx] [ty] [1] |
 */
struct matrix_t {
    double a; double b;
    double c; double d;
    double tx; double ty;
};

void matrix_init(struct matrix_t * m, double a, double b, double c, double d, double tx, double ty);
void matrix_init_identity(struct matrix_t * m);
void matrix_init_translate(struct matrix_t * m, double tx, double ty);
void matrix_init_scale(struct matrix_t * m, double sx, double sy);
void matrix_init_rotate(struct matrix_t * m, double r);
void matrix_multiply(struct matrix_t * m, struct matrix_t * m1, struct matrix_t * m2);
void matrix_invert(struct matrix_t * m);
void matrix_translate(struct matrix_t * m, double tx, double ty);
void matrix_scale(struct matrix_t * m, double sx, double sy);
void matrix_rotate(struct matrix_t * m, double r);
void matrix_transform_distance(struct matrix_t * m, double * dx, double * dy);
void matrix_transform_point(struct matrix_t * m, double * x, double * y);
void matrix_transform_bounds(struct matrix_t * m, double * x1, double * y1, double * x2, double * y2);

#ifdef __cplusplus
}
#endif

#endif /* __MATRIX_H__ */
