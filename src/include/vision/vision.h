#ifndef __VISION_VISION_H__
#define __VISION_VISION_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <graphic/surface.h>

enum vision_type_t {
	VISION_TYPE_GRAY		= 0x0100,
	VISION_TYPE_RGB			= 0x0301,
	VISION_TYPE_HSV			= 0x0302,
};

struct vision_t
{
	enum vision_type_t type;
	int width;
	int height;
	int npixel;
	float * datas;
	size_t ndata;
};

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

static inline int vision_get_channel(struct vision_t * v)
{
	return (v->type >> 8) & 0xff;
}

static inline int vision_get_npixel(struct vision_t * v)
{
	return v->npixel;
}

static inline float * vision_get_datas(struct vision_t * v)
{
	return v->datas;
}

static inline size_t vision_get_ndata(struct vision_t * v)
{
	return v->ndata;
}

struct vision_t * vision_alloc(enum vision_type_t type, int width, int height);
void vision_free(struct vision_t * v);
void vision_convert(struct vision_t * v, enum vision_type_t type);
void vision_clear(struct vision_t * v);
void vision_set_pixel(struct vision_t * v, int x, int y, int c, float px);
float vision_get_pixel(struct vision_t * v, int x, int y, int c);

struct vision_t * vision_alloc_from_surface(struct surface_t * s);
void vision_copyto_surface(struct vision_t * v, struct surface_t * s);

#ifdef __cplusplus
}
#endif

#endif /* __VISION_VISION_H__ */
