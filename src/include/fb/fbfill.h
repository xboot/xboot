#ifndef __FBFILL_H__
#define __FBFILL_H__

#include <configs.h>
#include <default.h>
#include <fb/fb.h>


void common_bitmap_fill_rect(struct bitmap * bitmap, u32_t c, u32_t x, u32_t y, u32_t w, u32_t h);

#endif /* __FBFILL_H__ */
