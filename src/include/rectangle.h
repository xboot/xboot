#ifndef __RECTANGLE_H__
#define __RECTANGLE_H__

#ifdef __cplusplus
extern "C" {
#endif

struct rectangle_t {
    double x, y;
    double w, h;
};

void rectangle_init(struct rectangle_t * r, double x, double y, double w, double h);
int rectangle_isempty(struct rectangle_t * r);
int rectangle_contains(struct rectangle_t * r, struct rectangle_t * o);
int rectangle_intersect(struct rectangle_t * r, struct rectangle_t * a, struct rectangle_t * b);
int rectangle_union(struct rectangle_t * r, struct rectangle_t * a, struct rectangle_t * b);

#ifdef __cplusplus
}
#endif

#endif /* __RECTANGLE_H__ */
