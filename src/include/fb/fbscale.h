#ifndef __FBSCALE_H__
#define __FBSCALE_H__

#include <configs.h>
#include <default.h>
#include <fb/fb.h>

enum bitmap_scale_method
{
	BITMAP_SCALE_METHOD_FASTEST,
	BITMAP_SCALE_METHOD_BEST,
};

x_bool bitmap_create_scaled(struct bitmap ** dst, x_u32 w, x_u32 h, struct bitmap * src, enum bitmap_scale_method method);

#endif /* __FBSCALE_H__ */
