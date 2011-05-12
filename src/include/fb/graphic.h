#ifndef __GRAPHIC_H__
#define __GRAPHIC_H__

#include <configs.h>
#include <default.h>
#include <rect.h>
#include <fb/fb.h>
#include <fb/bitmap.h>
#include <fb/font.h>


bool_t fb_set_viewport(struct fb * fb, struct rect * rect);
bool_t fb_get_viewport(struct fb * fb, struct rect * rect);
u32_t fb_map_color(struct fb * fb, u8_t r, u8_t g, u8_t b, u8_t a);
void fb_unmap_color(struct fb * fb, u32_t c, u8_t * r, u8_t * g, u8_t * b, u8_t * a);
bool_t fb_fill_rect(struct fb * fb, u32_t c, u32_t x, u32_t y, u32_t w, u32_t h);
bool_t fb_blit_bitmap(struct fb * fb, struct bitmap * bitmap, enum blit_mode mode, u32_t x, u32_t y, u32_t w, u32_t h, u32_t ox, u32_t oy);

bool_t bitmap_fill_rect(struct bitmap * bitmap, u32_t c, u32_t x, u32_t y, u32_t w, u32_t h);
bool_t bitmap_blit(struct bitmap * dst, struct bitmap * src, enum blit_mode mode, u32_t x, u32_t y, u32_t w, u32_t h, u32_t ox, u32_t oy);

#endif /* __GRAPHIC_H__ */
