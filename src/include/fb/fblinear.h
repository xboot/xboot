#ifndef __FBLINEAR_H__
#define __FBLINEAR_H__

#include <configs.h>
#include <default.h>
#include <fb/fb.h>


x_bool fb_draw_text(struct fb * fb, x_u32 x, x_u32 y, x_u32 fc, x_u32 bc, x_u8 * buf, x_u32 count);
x_bool fb_reverse_text(struct fb * fb, x_u32 x, x_u32 y);
x_bool fb_clear_screen(struct fb * fb);
x_bool fb_scrollup(struct fb * fb);


#endif /* __FBLINEAR_H__ */
