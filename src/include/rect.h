#ifndef __RECT_H__
#define __RECT_H__

#include <xboot.h>

/*
 * the structure of 2d point
 */
struct point
{
	s32_t x;
	s32_t y;
};

/*
 * the structure of 2d rect
 */
struct rect {
	s32_t	left;
	s32_t	top;
	s32_t 	right;
	s32_t	bottom;
};

/*
 * defined align method
 */
enum align {
	ALIGN_LEFT,
	ALIGN_TOP,
	ALIGN_RIGHT,
	ALIGN_BOTTOM,
	ALIGN_LEFT_TOP,
	ALIGN_RIGHT_TOP,
	ALIGN_LEFT_BOTTOM,
	ALIGN_RIGHT_BOTTOM,
	ALIGN_LEFT_CENTER,
	ALIGN_TOP_CENTER,
	ALIGN_RIGHT_CENTER,
	ALIGN_BOTTOM_CENTER,
	ALIGN_CENTER_HORIZONTAL,
	ALIGN_CENTER_VERTICAL,
	ALIGN_CENTER,
};

bool_t rect_set(struct rect * rect, s32_t left, s32_t top, s32_t right, s32_t bottom);
bool_t rect_set_empty(struct rect * rect);
bool_t rect_copy(struct rect * dst, struct rect * src);
bool_t rect_is_empty(struct rect * rect);
bool_t rect_is_equal(struct rect * rect1, struct rect * rect2);
bool_t rect_intersect(struct rect * rect, struct rect * src1, struct rect * src2);
bool_t rect_union(struct rect * rect, struct rect * src1, struct rect * src2);
bool_t rect_subtract(struct rect * rect, struct rect * src1, struct rect * src2);
bool_t rect_offset(struct rect * rect, s32_t dx, s32_t dy);
bool_t rect_inflate(struct rect * rect, s32_t dx, s32_t dy);
bool_t rect_have_point(struct rect * rect, s32_t x, s32_t y);
bool_t rect_align(struct rect * rect, struct rect * to, enum align flag);

#endif /* __RECT_H__ */
