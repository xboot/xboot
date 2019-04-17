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

#ifdef __cplusplus
}
#endif

#endif /* __RECTANGLE_H__ */
