#ifndef __FBDEF_H__
#define __FBDEF_H__

#include <configs.h>
#include <default.h>
#include <fb/fb.h>

x_u32 fb_default_map_color(struct fb * fb, x_u8 r, x_u8 g, x_u8 b, x_u8 a);
void fb_default_unmap_color(struct fb * fb, x_u32 c, x_u8 * r, x_u8 * g, x_u8 * b, x_u8 * a);
void fb_default_fill_rect(struct fb * fb, x_u32 c, x_u32 x, x_u32 y, x_u32 w, x_u32 h);
void fb_default_blit_bitmap(struct fb * fb, struct bitmap * bitmap, enum blit_mode mode, x_u32 x, x_u32 y, x_u32 w, x_u32 h, x_u32 ox, x_u32 oy);

#endif /* __FBDEF_H__ */
