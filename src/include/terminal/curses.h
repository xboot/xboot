#ifndef __TERMINAL_CURSES_H__
#define __TERMINAL_CURSES_H__


#include <configs.h>
#include <default.h>
#include <types.h>
#include <ctype.h>
#include <string.h>
#include <terminal/terminal.h>

enum terminal_curses_char {
	TERMINAL_SPACE,
	TERMINAL_HLINE,
	TERMINAL_VLINE,
	TERMINAL_LEFT,
	TERMINAL_RIGHT,
	TERMINAL_UP,
	TERMINAL_DOWN,
	TERMINAL_LEFTUP,
	TERMINAL_RIGHTUP,
	TERMINAL_LEFTDOWN,
	TERMINAL_RIGHTDOWN,
	TERMINAL_CROSS,
	TERMINAL_CUBE,
};

x_bool terminal_getwh(struct terminal * term, x_s32 * w, x_s32 * h);
x_bool terminal_setxy(struct terminal * term, x_s32 x, x_s32 y);
x_bool terminal_getxy(struct terminal * term, x_s32 * x, x_s32 * y);
x_bool terminal_cursor_left(struct terminal * term, x_u32 n);
x_bool terminal_cursor_right(struct terminal * term, x_u32 n);
x_bool terminal_cursor_up(struct terminal * term, x_u32 n);
x_bool terminal_cursor_down(struct terminal * term, x_u32 n);
x_bool terminal_cursor_home(struct terminal * term);
x_bool terminal_cursor_save(struct terminal * term);
x_bool terminal_cursor_restore(struct terminal * term);
x_bool terminal_cursor_hide(struct terminal * term);
x_bool terminal_cursor_show(struct terminal * term);
x_bool terminal_cursor_flash(struct terminal * term);
x_bool terminal_mode_highlight(struct terminal * term);
x_bool terminal_mode_underline(struct terminal * term);
x_bool terminal_mode_reverse(struct terminal * term);
x_bool terminal_mode_blanking(struct terminal * term);
x_bool terminal_mode_closeall(struct terminal * term);
x_bool terminal_set_color(struct terminal * term, enum terminal_color f, enum terminal_color b);
x_bool terminal_get_color(struct terminal * term, enum terminal_color * f, enum terminal_color * b);
x_bool terminal_clear_screen(struct terminal * term);
x_s32 terminal_read(struct terminal * term, x_u8 * buf, x_s32 count);
x_s32 terminal_write(struct terminal * term, x_u8 * buf, x_s32 count);
x_bool terminal_curses_putch(struct terminal * term, enum terminal_curses_char c);
x_bool terminal_draw_hline(struct terminal * term, x_u32 x0, x_u32 y0, x_u32 x);
x_bool terminal_draw_vline(struct terminal * term, x_u32 x0, x_u32 y0, x_u32 y);
x_bool terminal_draw_rect(struct terminal * term, x_u32 x0, x_u32 y0, x_u32 x1, x_u32 y1);

#endif /* __TERMINAL_CURSES_H__ */
