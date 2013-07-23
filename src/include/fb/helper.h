#ifndef __FB_HELPER_H__
#define __FB_HELPER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <fb/fb.h>
#include <fb/font.h>
#include <fb/color.h>
#include <fb/render.h>

void fb_fill_rect(struct fb_t * fb, struct color_t * c, u32_t x, u32_t y, u32_t w, u32_t h);
void fb_blit(struct fb_t * fb, struct texture_t * texture, u32_t x, u32_t y, u32_t w, u32_t h, u32_t ox, u32_t oy);
void fb_putcode(struct fb_t * fb, u32_t code, struct color_t * fc, struct color_t * bc, u32_t x, u32_t y);

#ifdef __cplusplus
}
#endif

#endif /* __FB_HELPER_H__ */
