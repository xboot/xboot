#ifndef __FBCOLOR_H__
#define __FBCOLOR_H__

#include <configs.h>
#include <default.h>
#include <fb/fb.h>


x_u32 bitmap_map_color(struct bitmap * bitmap, x_u8 r, x_u8 g, x_u8 b, x_u8 a);
void bitmap_unmap_color(struct bitmap * bitmap, x_u32 c, x_u8 * r, x_u8 * g, x_u8 * b, x_u8 * a);

#endif /* __FBCOLOR_H__ */
