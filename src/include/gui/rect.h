#ifndef __RECT_H__
#define __RECT_H__

#include <configs.h>
#include <default.h>

/*
 * the structure of 2d point
 */
struct point
{
	x_s32 x;
	x_s32 y;
};

/*
 * the structure of 2d rect
 */
struct rect {
	x_s32	left;
	x_s32	top;
	x_s32 	right;
	x_s32	bottom;
};


#endif /* __RECT_H__ */
