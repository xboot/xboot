#ifndef __IMAGE_H__
#define __IMAGE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <stdint.h>

struct image_t;
struct image_backend_t;

struct image_t
{
	int width;
	int height;
	int stride;
	void * pixels;

	struct image_backend_t * backend;
	void * priv;
};

struct image_backend_t
{
	struct image_t * (*create)(struct image_t * img, int width, int height);
	void (*destroy)(struct image_t * img);

	void (*clear)(struct image_t * img, uint32_t color);
};

static inline int image_get_width(struct image_t * img)
{
	return img->width;
}

static inline int image_get_height(struct image_t * img)
{
	return img->height;
}

static inline int image_get_stride(struct image_t * img)
{
	return img->stride;
}

static inline void * image_get_pixels(struct image_t * img)
{
	return img->pixels;
}

#ifdef __cplusplus
}
#endif

#endif /* __IMAGE_H__ */
