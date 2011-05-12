#ifndef __FBSOFT_H__
#define __FBSOFT_H__

#include <configs.h>
#include <default.h>
#include <fb/fb.h>


u32_t fb_soft_map_color(struct fb * fb, u8_t r, u8_t g, u8_t b, u8_t a);
void fb_soft_unmap_color(struct fb * fb, u32_t c, u8_t * r, u8_t * g, u8_t * b, u8_t * a);
bool_t fb_soft_fill_rect(struct fb * fb, u32_t c, u32_t x, u32_t y, u32_t w, u32_t h);
bool_t fb_soft_blit_bitmap(struct fb * fb, struct bitmap * bitmap, enum blit_mode mode, u32_t x, u32_t y, u32_t w, u32_t h, u32_t ox, u32_t oy);

#endif /* __FBSOFT_H__ */
