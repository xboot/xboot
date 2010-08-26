#ifndef __FBPIXEL_H__
#define __FBPIXEL_H__

#include <configs.h>
#include <default.h>
#include <fb/fb.h>


x_u8 * bitmap_get_pointer(struct bitmap * bitmap, x_u32 x, x_u32 y);
x_u32 bitmap_get_pixel(struct bitmap * bitmap, x_u32 x, x_u32 y);
void bitmap_set_pixel(struct bitmap * bitmap, x_u32 x, x_u32 y, x_u32 c);

#endif /* __FBPIXEL_H__ */
