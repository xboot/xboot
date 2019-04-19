#ifndef __REGION_H__
#define __REGION_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

struct region_t {
    int x, y;
    int w, h;
    int area;
};

static inline void region_init(struct region_t * r, int x, int y, int w, int h)
{
	r->x = x;
	r->y = y;
	r->w = w;
	r->h = h;
	r->area = w * h;
}

static inline void region_clone(struct region_t * r, struct region_t * o)
{
	memcpy(r, o, sizeof(struct region_t));
}

static inline int region_isempty(struct region_t * r)
{
	if(r->area > 0)
		return 1;
	return 0;
}

static inline int region_contains(struct region_t * r, struct region_t * o)
{
	if(r->area >= o->area)
	{
		int rr = r->x + r->w;
		int rb = r->y + r->h;
		int or = o->x + o->w;
		int ob = o->y + o->h;
		if((o->x >= r->x) && (o->x < rr) && (o->y >= r->y) && (o->y < rb) && (or > r->x) && (or <= rr) && (ob > r->y) && (ob <= rb))
			return 1;
	}
	return 0;
}

int region_intersect(struct region_t * r, struct region_t * a, struct region_t * b);
int region_union(struct region_t * r, struct region_t * a, struct region_t * b);

#ifdef __cplusplus
}
#endif

#endif /* __REGION_H__ */
