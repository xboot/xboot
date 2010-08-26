#ifndef __GRAPHIC_H__
#define __GRAPHIC_H__

#include <configs.h>
#include <default.h>
#include <fb/fb.h>


x_u32 map_bitmap_color(struct bitmap * bitmap, x_u8 r, x_u8 g, x_u8 b, x_u8 a);
void unmap_bitmap_color(struct bitmap * bitmap, x_u32 c, x_u8 * r, x_u8 * g, x_u8 * b, x_u8 * a);
x_u8 * get_bitmap_pointer(struct bitmap * bitmap, x_u32 x, x_u32 y);
x_u32 get_bitmap_pixel(struct bitmap * bitmap, x_u32 x, x_u32 y);
void set_bitmap_pixel(struct bitmap * bitmap, x_u32 x, x_u32 y, x_u32 c);
void bitmap_fill_rect(struct bitmap * bitmap, x_u32 c, x_u32 x, x_u32 y, x_u32 w, x_u32 h);

#endif /* __GRAPHIC_H__ */
