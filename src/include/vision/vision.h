#ifndef __VISION_VISION_H__
#define __VISION_VISION_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <graphic/surface.h>

enum vision_type_t {
	VISION_TYPE_GRAY	= 0x0110,	/* unsigned char (0 ~ 255) */
	VISION_TYPE_RGB		= 0x0311,	/* unsigned char (0 ~ 255) */
	VISION_TYPE_HSV		= 0x0342,	/* float (0.0f ~ 1.0f) */
};

struct vision_t
{
	enum vision_type_t type;
	int width;
	int height;
	int npixel;
	void * datas;
	size_t ndata;
};

static inline int vision_type_get_bytes(enum vision_type_t type)
{
	return (type >> 4) & 0xf;
}

static inline int vision_type_get_channels(enum vision_type_t type)
{
	return (type >> 8) & 0xff;
}

static inline enum vision_type_t vision_get_type(struct vision_t * v)
{
	return v->type;
}

static inline int vision_get_width(struct vision_t * v)
{
	return v->width;
}

static inline int vision_get_height(struct vision_t * v)
{
	return v->height;
}

static inline int vision_get_npixel(struct vision_t * v)
{
	return v->npixel;
}

static inline void * vision_get_datas(struct vision_t * v)
{
	return v->datas;
}

static inline size_t vision_get_ndata(struct vision_t * v)
{
	return v->ndata;
}

struct vision_t * vision_alloc(enum vision_type_t type, int width, int height);
void vision_free(struct vision_t * v);
struct vision_t * vision_clone(struct vision_t * v, int x, int y, int w, int h);
void vision_convert(struct vision_t * v, enum vision_type_t type);
void vision_clear(struct vision_t * v);

void vision_apply_surface(struct vision_t * v, struct surface_t * s);
void surface_apply_vision(struct surface_t * s, struct vision_t * v);

#ifdef __cplusplus
}
#endif

#endif /* __VISION_VISION_H__ */
