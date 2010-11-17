#ifndef __GRAPHIC_H__
#define __GRAPHIC_H__

#include <configs.h>
#include <default.h>
#include <rect.h>
#include <fb/fb.h>
#include <fb/bitmap.h>
#include <fb/font.h>


x_bool fb_set_viewport(struct fb * fb, struct rect * rect);
x_bool fb_get_viewport(struct fb * fb, struct rect * rect);
x_u32 fb_map_color(struct fb * fb, x_u8 r, x_u8 g, x_u8 b, x_u8 a);
void fb_unmap_color(struct fb * fb, x_u32 c, x_u8 * r, x_u8 * g, x_u8 * b, x_u8 * a);
x_bool fb_fill_rect(struct fb * fb, x_u32 c, x_u32 x, x_u32 y, x_u32 w, x_u32 h);
x_bool fb_blit_bitmap(struct fb * fb, struct bitmap * bitmap, enum blit_mode mode, x_u32 x, x_u32 y, x_u32 w, x_u32 h, x_u32 ox, x_u32 oy);

x_bool bitmap_fill_rect(struct bitmap * bitmap, x_u32 c, x_u32 x, x_u32 y, x_u32 w, x_u32 h);
x_bool bitmap_blit(struct bitmap * dst, struct bitmap * src, enum blit_mode mode, x_u32 x, x_u32 y, x_u32 w, x_u32 h, x_u32 ox, x_u32 oy);

#endif /* __GRAPHIC_H__ */
