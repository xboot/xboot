#ifndef __FBBLIT_H__
#define __FBBLIT_H__

#include <configs.h>
#include <default.h>
#include <fb/fb.h>


void common_bitmap_blit(struct bitmap * dst, struct bitmap * src, enum blit_mode mode, x_u32 x, x_u32 y, x_u32 w, x_u32 h, x_u32 ox, x_u32 oy);

#endif /* __FBBLIT_H__ */
