#ifndef __FB_HELPER_H__
#define __FB_HELPER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <fb/fb.h>
#include <fb/font.h>
#include <graphic/surface.h>

u32_t fb_map_color(struct fb * fb, struct color_t * col);
bool_t fb_fill_rect(struct fb * fb, u32_t c, u32_t x, u32_t y, u32_t w, u32_t h);
bool_t fb_blit(struct fb * fb, struct surface_t * surface, u32_t x, u32_t y, u32_t w, u32_t h, u32_t ox, u32_t oy);
bool_t fb_putcode(struct fb * fb, u32_t code, u32_t fc, u32_t bc, u32_t x, u32_t y);

#ifdef __cplusplus
}
#endif

#endif /* __FB_HELPER_H__ */
