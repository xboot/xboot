#ifndef __GRAPHIC_H__
#define __GRAPHIC_H__

#include <configs.h>
#include <default.h>
#include <fb/fb.h>


x_bool draw_line(struct fb * fb, x_u32 x1, x_u32 y1, x_u32 x2, x_u32 y2, x_u32 c);
x_bool draw_line_ex(struct fb * fb, x_u32 x1, x_u32 y1, x_u32 x2, x_u32 y2, x_u32 w, x_u32 c);
x_bool draw_rect(struct fb * fb, struct rect * rect, x_u32 c);

#endif /* __GRAPHIC_H__ */
