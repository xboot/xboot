#ifndef __FBBLIT_H__
#define __FBBLIT_H__

#include <configs.h>
#include <default.h>
#include <fb/fb.h>


void common_bitmap_blit(struct bitmap * dst, struct bitmap * src, enum blit_mode mode, u32_t x, u32_t y, u32_t w, u32_t h, u32_t ox, u32_t oy);

#endif /* __FBBLIT_H__ */
