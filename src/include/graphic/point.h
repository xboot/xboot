#ifndef __GRAPHIC_POINT_H__
#define __GRAPHIC_POINT_H__

#ifdef __cplusplus
extern "C" {
#endif

struct point_t {
	int x;
	int y;
};

static inline void point_init(struct point_t * p, int x, int y)
{
	p->x = x;
	p->y = y;
}

static inline void point_clone(struct point_t * p, struct point_t * o)
{
	memcpy(p, o, sizeof(struct point_t));
}

#ifdef __cplusplus
}
#endif

#endif /* __GRAPHIC_POINT_H__ */
