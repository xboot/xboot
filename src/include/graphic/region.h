#ifndef __GRAPHIC_REGION_H__
#define __GRAPHIC_REGION_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

struct region_t {
	int x, y;
	int w, h;
};

static inline void region_init(struct region_t * r, int x, int y, int w, int h)
{
	r->x = x;
	r->y = y;
	r->w = w;
	r->h = h;
}

static inline void region_clone(struct region_t * r, struct region_t * o)
{
	r->x = o->x;
	r->y = o->y;
	r->w = o->w;
	r->h = o->h;
}

static inline int region_isempty(struct region_t * r)
{
	if((r->w > 0) && (r->h > 0))
		return 0;
	return 1;
}

static inline int region_hit(struct region_t * r, int x, int y)
{
	if((x >= r->x) && (x < r->x + r->w) && (y >= r->y) && (y < r->y + r->h))
		return 1;
	return 0;
}

static inline int region_contains(struct region_t * r, struct region_t * o)
{
	int rr = r->x + r->w;
	int rb = r->y + r->h;
	int or = o->x + o->w;
	int ob = o->y + o->h;
	if((o->x >= r->x) && (o->x < rr) && (o->y >= r->y) && (o->y < rb) && (or > r->x) && (or <= rr) && (ob > r->y) && (ob <= rb))
		return 1;
	return 0;
}

static inline int region_overlap(struct region_t * r, struct region_t * o)
{
	if((o->x + o->w >= r->x) && (o->x <= r->x + r->w) && (o->y + o->h >= r->y) && (o->y <= r->y + r->h))
		return 1;
	return 0;
}

static inline void region_expand(struct region_t * r, struct region_t * o, int n)
{
	r->x = o->x - n;
	r->y = o->y - n;
	r->w = o->w + n * 2;
	r->h = o->h + n * 2;
}

static inline int region_intersect(struct region_t * r, struct region_t * a, struct region_t * b)
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
			return 1;
		}
	}
	return 0;
}

static inline int region_union(struct region_t * r, struct region_t * a, struct region_t * b)
{
	int ar = a->x + a->w;
	int ab = a->y + a->h;
	int br = b->x + b->w;
	int bb = b->y + b->h;
	r->x = min(a->x, b->x);
	r->y = min(a->y, b->y);
	r->w = max(ar, br) - r->x;
	r->h = max(ab, bb) - r->y;
	return 1;
}

struct region_list_t {
	struct region_t * region;
	unsigned int size;
	unsigned int count;
};

struct region_list_t * region_list_alloc(unsigned int size);
void region_list_free(struct region_list_t * rl);
void region_list_clone(struct region_list_t * rl, struct region_list_t * o);
void region_list_merge(struct region_list_t * rl, struct region_list_t * o);
void region_list_add(struct region_list_t * rl, struct region_t * r);
void region_list_clear(struct region_list_t * rl);

#ifdef __cplusplus
}
#endif

#endif /* __GRAPHIC_REGION_H__ */
