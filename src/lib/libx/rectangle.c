/*
 * libx/matrix.c
 */

#include <math.h>
#include <string.h>
#include <rectangle.h>

void rectangle_init(struct rectangle_t * r, double x, double y, double w, double h)
{
	r->x = x;
	r->y = y;
	r->w = w;
	r->h = h;
}
