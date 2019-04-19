/*
 * libx/region.c
 */

#include <region.h>

#define min(x,y) ({			\
	typeof(x) _x = (x);		\
	typeof(y) _y = (y);		\
	(void)(&_x == &_y);		\
	_x < _y ? _x : _y; })

#define max(x,y) ({			\
	typeof(x) _x = (x);		\
	typeof(y) _y = (y);		\
	(void)(&_x == &_y);		\
	_x > _y ? _x : _y; })

int region_intersect(struct region_t * r, struct region_t * a, struct region_t * b)
{
	int x0 = max(a->x, b->x);
	int x1 = min(a->x + a->w, b->x + b->w);
	if(x0 <= x1)
	{
		int y0 = max(a->y, b->y);
		int y1 = min(a->y + a->h, b->y + b->h);
		if(y0 <= y1)
		{
			r->x = x0;
			r->y = y0;
			r->w = x1 - x0;
			r->h = y1 - y0;
			r->area = r->w * r->h;
			return 1;
		}
	}
	return 0;
}

int region_union(struct region_t * r, struct region_t * a, struct region_t * b)
{
	int ar = a->x + a->w;
	int ab = a->y + a->h;
	int br = b->x + b->w;
	int bb = b->y + b->h;
	r->x = min(a->x, b->x);
	r->y = min(a->y, b->y);
	r->w = max(ar, br) - r->x;
	r->h = max(ab, bb) - r->y;
	r->area = r->w * r->h;
	return 1;
}
