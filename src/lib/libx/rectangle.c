/*
 * libx/matrix.c
 */

#include <math.h>
#include <string.h>
#include <rectangle.h>

#ifndef MIN
#define MIN(a, b)	((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b)	((a) > (b) ? (a) : (b))
#endif

void rectangle_init(struct rectangle_t * r, double x, double y, double w, double h)
{
	r->x = x;
	r->y = y;
	r->w = w;
	r->h = h;
}

int rectangle_isempty(struct rectangle_t * r)
{
	if((r->w > 0.0) && (r->h > 0.0))
		return 1;
	return 0;
}

int rectangle_contains(struct rectangle_t * r, struct rectangle_t * o)
{
	double rr = r->x + r->w;
	double rb = r->y + r->h;
	double or = o->x + o->w;
	double ob = o->y + o->h;
	if((o->x >= r->x) && (o->x < rr) && (o->y >= r->y) && (o->y < rb) && (or > r->x) && (or <= rr) && (ob > r->y) && (ob <= rb))
		return 1;
	return 0;
}

int rectangle_intersect(struct rectangle_t * r, struct rectangle_t * a, struct rectangle_t * b)
{
	double x0 = MAX(a->x, b->x);
	double x1 = MIN(a->x + a->w, b->x + b->w);
	if(x0 <= x1)
	{
		double y0 = MAX(a->y, b->y);
		double y1 = MIN(a->y + a->h, b->y + b->h);
		if(y0 <= y1)
		{
			r->x = x0;
			r->y = y0;
			r->w = x1 - x0;
			r->h = y1 - y0;
			return 1;
		}
	}
	return 0;
}

int rectangle_union(struct rectangle_t * r, struct rectangle_t * a, struct rectangle_t * b)
{
	double ar = a->x + a->w;
	double ab = a->y + a->h;
	double br = b->x + b->w;
	double bb = b->y + b->h;
	r->x = MIN(a->x, b->x);
	r->y = MIN(a->y, b->y);
	r->w = MAX(ar, br) - r->x;
	r->h = MAX(ab, bb) - r->y;
	return 1;
}
