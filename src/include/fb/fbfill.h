#ifndef __FBFILL_H__
#define __FBFILL_H__

#include <configs.h>
#include <default.h>
#include <fb/fb.h>

void bitmap_fill_rect_generic(struct bitmap * bitmap, x_u32 c, x_u32 x, x_u32 y, x_u32 w, x_u32 h);
void bitmap_fill_rect_direct32(struct bitmap * bitmap, x_u32 c, x_u32 x, x_u32 y, x_u32 w, x_u32 h);
void bitmap_fill_rect_direct24(struct bitmap * bitmap, x_u32 c, x_u32 x, x_u32 y, x_u32 w, x_u32 h);
void bitmap_fill_rect_direct16(struct bitmap * bitmap, x_u32 c, x_u32 x, x_u32 y, x_u32 w, x_u32 h);
void bitmap_fill_rect_direct8(struct bitmap * bitmap, x_u32 c, x_u32 x, x_u32 y, x_u32 w, x_u32 h);

#endif /* __FBFILL_H__ */
