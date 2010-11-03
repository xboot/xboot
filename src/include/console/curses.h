#ifndef __CURSES_H__
#define __CURSES_H__

#include <configs.h>
#include <default.h>
#include <types.h>
#include <ctype.h>
#include <string.h>
#include <console/console.h>


x_bool console_getwh(struct console * console, x_s32 * w, x_s32 * h);
x_bool console_getxy(struct console * console, x_s32 * x, x_s32 * y);
x_bool console_gotoxy(struct console * console, x_s32 x, x_s32 y);
x_bool console_setcursor(struct console * console, x_bool on);
x_bool console_setcolor(struct console * console, enum console_color f, enum console_color b);
x_bool console_getcolor(struct console * console, enum console_color * f, enum console_color * b);
x_bool console_cls(struct console * console);
x_bool console_refresh(struct console * console);
x_bool console_getchar(struct console * console, x_u32 * c);
x_bool console_putchar(struct console * console, x_u32 c);
x_bool console_draw_hline(struct console * console, x_u32 x0, x_u32 y0, x_u32 x);
x_bool console_draw_vline(struct console * console, x_u32 x0, x_u32 y0, x_u32 y);
x_bool console_draw_rect(struct console * console, x_u32 x0, x_u32 y0, x_u32 x1, x_u32 y1);

#endif /* __CURSES_H__ */
