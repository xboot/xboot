#ifndef __RECT_H__
#define __RECT_H__

#include <xboot.h>

/*
 *  The structure that defines a point
 */
struct point_t {
	s32_t x;
	s32_t y;
};

/*
 * A rectangle, with the origin at the upper left
 */
struct rect_t {
	s32_t x, y;
    s32_t w, h;
};

bool_t rect_is_empty(const struct rect_t * r);
bool_t rect_is_equal(const struct rect_t * a, const struct rect_t * b);
bool_t rect_has_intersection(const struct rect_t * a, const struct rect_t * b);
bool_t rect_intersect(const struct rect_t * a, const struct rect_t * b, struct rect_t * r);
void rect_union(const struct rect_t * a, const struct rect_t * b, struct rect_t * r);
bool_t rect_enclose_points(const struct point_t * p, int cnt, const struct rect_t * clip, struct rect_t * r);
bool_t rect_intersect_with_line(const struct rect_t * r, s32_t * x1, s32_t * y1, s32_t * x2, s32_t * y2);

#endif /* __RECT_H__ */
