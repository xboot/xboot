#ifndef __FBDEF_H__
#define __FBDEF_H__

#include <configs.h>
#include <default.h>
#include <fb/fb.h>

x_u32 fb_default_map_color(struct fb * fb, x_u8 r, x_u8 g, x_u8 b, x_u8 a);
x_bool fb_default_unmap_color(struct fb * fb, x_u32 c, x_u8 * r, x_u8 * g, x_u8 * b, x_u8 * a);

#endif /* __FBDEF_H__ */
