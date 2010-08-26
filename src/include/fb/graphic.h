#ifndef __GRAPHIC_H__
#define __GRAPHIC_H__

#include <configs.h>
#include <default.h>
#include <fb/fb.h>

/*
 * the structure of 2d point
 */
struct point
{
	x_u32 x;
	x_u32 y;
};

/*
 * the structure of 2d rect
 */
struct rect {
	x_u32	x;
	x_u32	y;
	x_u32 	w;
	x_u32	h;
};

void save_bitmap_viewport(struct bitmap * bitmap, struct rect * rect);
void restore_bitmap_viewport(struct bitmap * bitmap, struct rect * rect);
x_u32 map_bitmap_color(struct bitmap * bitmap, x_u8 r, x_u8 g, x_u8 b, x_u8 a);
void unmap_bitmap_color(struct bitmap * bitmap, x_u32 c, x_u8 * r, x_u8 * g, x_u8 * b, x_u8 * a);
x_u8 * get_bitmap_pointer(struct bitmap * bitmap, x_u32 x, x_u32 y);
x_u32 get_bitmap_pixel(struct bitmap * bitmap, x_u32 x, x_u32 y);
void set_bitmap_pixel(struct bitmap * bitmap, x_u32 x, x_u32 y, x_u32 c);
void bitmap_fill_rect(struct bitmap * bitmap, x_u32 c, x_u32 x, x_u32 y, x_u32 w, x_u32 h);
void bitmap_blit(struct bitmap * dst, struct bitmap * src, enum blit_mode mode, x_u32 x, x_u32 y, x_u32 w, x_u32 h, x_u32 ox, x_u32 oy);

#endif /* __GRAPHIC_H__ */
