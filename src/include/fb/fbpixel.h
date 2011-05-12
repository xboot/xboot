#ifndef __FBPIXEL_H__
#define __FBPIXEL_H__

#include <configs.h>
#include <default.h>
#include <fb/fb.h>

inline u8_t * bitmap_get_pointer(struct bitmap * bitmap, u32_t x, u32_t y);
inline u32_t bitmap_get_pixel(struct bitmap * bitmap, u32_t x, u32_t y);
inline void bitmap_set_pixel(struct bitmap * bitmap, u32_t x, u32_t y, u32_t c);

#endif /* __FBPIXEL_H__ */
