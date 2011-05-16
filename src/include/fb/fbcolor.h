#ifndef __FBCOLOR_H__
#define __FBCOLOR_H__

#include <xboot.h>
#include <fb/fb.h>


u32_t bitmap_map_color(struct bitmap * bitmap, u8_t r, u8_t g, u8_t b, u8_t a);
void bitmap_unmap_color(struct bitmap * bitmap, u32_t c, u8_t * r, u8_t * g, u8_t * b, u8_t * a);

#endif /* __FBCOLOR_H__ */
