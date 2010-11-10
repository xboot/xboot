#ifndef __CURSES_H__
#define __CURSES_H__

#include <configs.h>
#include <default.h>
#include <types.h>
#include <ctype.h>
#include <string.h>
#include <console/console.h>

x_bool console_draw_hline(struct console * console, x_u32 x0, x_u32 y0, x_u32 x);
x_bool console_draw_vline(struct console * console, x_u32 x0, x_u32 y0, x_u32 y);
x_bool console_draw_rect(struct console * console, x_u32 x0, x_u32 y0, x_u32 x1, x_u32 y1);

#endif /* __CURSES_H__ */
