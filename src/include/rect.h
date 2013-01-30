#ifndef __RECT_H__
#define __RECT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

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

/*
 * Align method
 */
enum align_t {
	ALIGN_LEFT					= 0,
	ALIGN_TOP					= 1,
	ALIGN_RIGHT					= 2,
	ALIGN_BOTTOM				= 3,
	ALIGN_LEFT_TOP				= 4,
	ALIGN_RIGHT_TOP				= 5,
	ALIGN_LEFT_BOTTOM			= 6,
	ALIGN_RIGHT_BOTTOM			= 7,
	ALIGN_LEFT_CENTER			= 8,
	ALIGN_TOP_CENTER			= 9,
	ALIGN_RIGHT_CENTER			= 10,
	ALIGN_BOTTOM_CENTER			= 11,
	ALIGN_CENTER_HORIZONTAL		= 12,
	ALIGN_CENTER_VERTICAL		= 13,
	ALIGN_CENTER				= 14,
};

bool_t rect_is_empty(const struct rect_t * r);
bool_t rect_is_equal(const struct rect_t * a, const struct rect_t * b);
bool_t rect_has_intersection(const struct rect_t * a, const struct rect_t * b);
bool_t rect_intersect(const struct rect_t * a, const struct rect_t * b, struct rect_t * r);
void rect_union(const struct rect_t * a, const struct rect_t * b, struct rect_t * r);
bool_t rect_enclose_points(const struct point_t * p, int cnt, const struct rect_t * clip, struct rect_t * r);
bool_t rect_intersect_with_line(const struct rect_t * r, s32_t * x1, s32_t * y1, s32_t * x2, s32_t * y2);
bool_t rect_align(const struct rect_t * a, const struct rect_t * b, struct rect_t * r, enum align_t align);

#ifdef __cplusplus
}
#endif

#endif /* __RECT_H__ */
