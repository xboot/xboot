/*
 * drivers/terminal/curses.c
 *
 *
 * Copyright (c) 2007-2009  jianjun jiang <jjjstudio@gmail.com>
 * website: http://xboot.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <configs.h>
#include <default.h>
#include <types.h>
#include <ctype.h>
#include <string.h>
#include <xboot/scank.h>
#include <xboot/printk.h>
#include <terminal/terminal.h>
#include <terminal/curses.h>


#define	TERMINAL_SERIAL_SPACE		(' ')
#define	TERMINAL_SERIAL_HLINE		('-')
#define	TERMINAL_SERIAL_VLINE		('|')
#define	TERMINAL_SERIAL_LEFT		('<')
#define	TERMINAL_SERIAL_RIGHT		('>')
#define	TERMINAL_SERIAL_UP			('^')
#define	TERMINAL_SERIAL_DOWN		('v')
#define	TERMINAL_SERIAL_LEFTUP		('+')
#define	TERMINAL_SERIAL_RIGHTUP		('+')
#define	TERMINAL_SERIAL_LEFTDOWN	('+')
#define	TERMINAL_SERIAL_RIGHTDOWN	('+')
#define	TERMINAL_SERIAL_CROSS		('+')
#define	TERMINAL_SERIAL_CUBE		('#')

#define	TERMINAL_LCD_SPACE			(' ')
#define	TERMINAL_LCD_HLINE			(196)
#define	TERMINAL_LCD_VLINE			(179)
#define	TERMINAL_LCD_LEFT			(17)
#define	TERMINAL_LCD_RIGHT			(16)
#define	TERMINAL_LCD_UP				(30)
#define	TERMINAL_LCD_DOWN			(31)
#define	TERMINAL_LCD_LEFTUP			(218)
#define	TERMINAL_LCD_RIGHTUP		(191)
#define	TERMINAL_LCD_LEFTDOWN		(192)
#define	TERMINAL_LCD_RIGHTDOWN		(217)
#define	TERMINAL_LCD_CROSS			(197)
#define	TERMINAL_LCD_CUBE			(254)

x_bool terminal_getwh(struct terminal * term, x_s32 * w, x_s32 * h)
{
	if(term)
		return term->getwh(term, w, h);
	return FALSE;
}

x_bool terminal_setxy(struct terminal * term, x_s32 x, x_s32 y)
{
	x_s32 w, h;

	if(term)
	{
		if(!term->getwh(term, &w, &h))
			return FALSE;

		if(x < 0)
			x = 0;

		if(x >= w)
			x = w - 1;

		if(y < 0)
			y = 0;

		if(y >= h)
			y = h - 1;

		return term->setxy(term, x, y);
	}
	return FALSE;
}

x_bool terminal_getxy(struct terminal * term, x_s32 * x, x_s32 * y)
{
	if(term)
		return term->getxy(term, x, y);
	return FALSE;
}

x_bool terminal_cursor_left(struct terminal * term, x_u32 n)
{
	if(term)
		return term->cursor_left(term, n);
	return FALSE;
}

x_bool terminal_cursor_right(struct terminal * term, x_u32 n)
{
	if(term)
		return term->cursor_right(term, n);
	return FALSE;
}

x_bool terminal_cursor_up(struct terminal * term, x_u32 n)
{
	if(term)
		return term->cursor_up(term, n);
	return FALSE;
}

x_bool terminal_cursor_down(struct terminal * term, x_u32 n)
{
	if(term)
		return term->cursor_down(term, n);
	return FALSE;
}

x_bool terminal_cursor_home(struct terminal * term)
{
	if(term)
		return term->cursor_home(term);
	return FALSE;
}

x_bool terminal_cursor_save(struct terminal * term)
{
	if(term)
		return term->cursor_save(term);
	return FALSE;
}

x_bool terminal_cursor_restore(struct terminal * term)
{
	if(term)
		return term->cursor_restore(term);
	return FALSE;
}

x_bool terminal_cursor_hide(struct terminal * term)
{
	if(term)
		return term->cursor_hide(term);
	return FALSE;
}

x_bool terminal_cursor_show(struct terminal * term)
{
	if(term)
		return term->cursor_show(term);
	return FALSE;
}

x_bool terminal_cursor_flash(struct terminal * term)
{
	if(term)
		return term->cursor_flash(term);
	return FALSE;
}

x_bool terminal_mode_highlight(struct terminal * term)
{
	if(term)
		return term->mode_highlight(term);
	return FALSE;
}

x_bool terminal_mode_underline(struct terminal * term)
{
	if(term)
		return term->mode_underline(term);
	return FALSE;
}

x_bool terminal_mode_reverse(struct terminal * term)
{
	if(term)
		return term->mode_reverse(term);
	return FALSE;
}

x_bool terminal_mode_blanking(struct terminal * term)
{
	if(term)
		return term->mode_blanking(term);
	return FALSE;
}

x_bool terminal_mode_closeall(struct terminal * term)
{
	if(term)
		return term->mode_closeall(term);
	return FALSE;
}

x_bool terminal_set_color(struct terminal * term, enum terminal_color f, enum terminal_color b)
{
	if(term)
		return term->set_color(term, f, b);
	return FALSE;
}

x_bool terminal_get_color(struct terminal * term, enum terminal_color * f, enum terminal_color * b)
{
	if(term)
		return term->get_color(term, f, b);
	return FALSE;
}

x_bool terminal_clear_screen(struct terminal * term)
{
	if(term)
		return term->clear_screen(term);
	return FALSE;
}

x_s32 terminal_read(struct terminal * term, x_u8 * buf, x_s32 count)
{
	if(term)
		return term->read(term, buf, count);
	return 0;
}

x_s32 terminal_write(struct terminal * term, x_u8 * buf, x_s32 count)
{
	if(term)
		return term->write(term, buf, count);
	return 0;
}

x_bool terminal_curses_putch(struct terminal * term, enum terminal_curses_char c)
{
	x_u8 ch;

	if(!term || !term->write)
		return FALSE;

	switch(term->type)
	{
	case TERMINAL_SERIAL:
		switch(c)
		{
		case TERMINAL_SPACE:
			ch = TERMINAL_SERIAL_SPACE;
			break;
		case TERMINAL_HLINE:
			ch = TERMINAL_SERIAL_HLINE;
			break;
		case TERMINAL_VLINE:
			ch = TERMINAL_SERIAL_VLINE;
			break;
		case TERMINAL_LEFT:
			ch = TERMINAL_SERIAL_LEFT;
			break;
		case TERMINAL_RIGHT:
			ch = TERMINAL_SERIAL_RIGHT;
			break;
		case TERMINAL_UP:
			ch = TERMINAL_SERIAL_UP;
			break;
		case TERMINAL_DOWN:
			ch = TERMINAL_SERIAL_DOWN;
			break;
		case TERMINAL_LEFTUP:
			ch = TERMINAL_SERIAL_LEFTUP;
			break;
		case TERMINAL_RIGHTUP:
			ch = TERMINAL_SERIAL_RIGHTUP;
			break;
		case TERMINAL_LEFTDOWN:
			ch = TERMINAL_SERIAL_LEFTDOWN;
			break;
		case TERMINAL_RIGHTDOWN:
			ch = TERMINAL_SERIAL_RIGHTDOWN;
			break;
		case TERMINAL_CROSS:
			ch = TERMINAL_SERIAL_CROSS;
			break;
		case TERMINAL_CUBE:
			ch = TERMINAL_SERIAL_CUBE;
			break;
		default:
			return FALSE;
		}
		break;

	case TERMINAL_LCD:
		switch(c)
		{
		case TERMINAL_SPACE:
			ch = TERMINAL_LCD_SPACE;
			break;
		case TERMINAL_HLINE:
			ch = TERMINAL_LCD_HLINE;
			break;
		case TERMINAL_VLINE:
			ch = TERMINAL_LCD_VLINE;
			break;
		case TERMINAL_LEFT:
			ch = TERMINAL_LCD_LEFT;
			break;
		case TERMINAL_RIGHT:
			ch = TERMINAL_LCD_RIGHT;
			break;
		case TERMINAL_UP:
			ch = TERMINAL_LCD_UP;
			break;
		case TERMINAL_DOWN:
			ch = TERMINAL_LCD_DOWN;
			break;
		case TERMINAL_LEFTUP:
			ch = TERMINAL_LCD_LEFTUP;
			break;
		case TERMINAL_RIGHTUP:
			ch = TERMINAL_LCD_RIGHTUP;
			break;
		case TERMINAL_LEFTDOWN:
			ch = TERMINAL_LCD_LEFTDOWN;
			break;
		case TERMINAL_RIGHTDOWN:
			ch = TERMINAL_LCD_RIGHTDOWN;
			break;
		case TERMINAL_CROSS:
			ch = TERMINAL_LCD_CROSS;
			break;
		case TERMINAL_CUBE:
			ch = TERMINAL_LCD_CUBE;
			break;
		default:
			return FALSE;
		}
		break;

	default:
		return FALSE;
	}

	term->write(term, &ch, 1);
	return TRUE;
}

/*
 * draw hline.
 */
x_bool terminal_draw_hline(struct terminal * term, x_u32 x0, x_u32 y0, x_u32 x)
{
	x_s32 width, height;
	x_u32 i, l;

	if(term)
	{
		if(!term->getwh(term, &width, &height))
			return FALSE;

		if(x0 > width || y0 > height)
			return FALSE;

		if(x0 + x > width)
			l = width - x0;
		else
			l = x;

		term->setxy(term, x0, y0);
		for(i = 0; i < l; i++)
		{
			terminal_curses_putch(term, TERMINAL_HLINE);
		}

		return TRUE;
	}
	return FALSE;
}

/*
 * draw vline.
 */
x_bool terminal_draw_vline(struct terminal * term, x_u32 x0, x_u32 y0, x_u32 y)
{
	x_s32 width, height;
	x_u32 i, l;

	if(term)
	{
		if(!term->getwh(term, &width, &height))
			return FALSE;

		if(x0 > width || y0 > height)
			return FALSE;

		if(y0 + y > height)
			l = width - y0;
		else
			l = y;

		for(i = 0; i < l; i++)
		{
			term->setxy(term, x0, y0 + i);
			terminal_curses_putch(term, TERMINAL_VLINE);
		}
		return TRUE;
	}
	return FALSE;
}

/*
 * draw rect.
 */
x_bool terminal_draw_rect(struct terminal * term, x_u32 x0, x_u32 y0, x_u32 x1, x_u32 y1)
{
	x_s32 width, height;
	x_u32 x_min, y_min, x_max, y_max;

	if(!term || !term->getwh(term, &width, &height))
		return FALSE;

	x_min = MIN(x0, x1);
	x_max = MAX(x0, x1);

	y_min = MIN(y0, y1);
	y_max = MAX(y0, y1);

	if(x_min < width && y_min < height)
	{
		term->setxy(term, x_min, y_min);
		terminal_curses_putch(term, TERMINAL_LEFTUP);
		terminal_draw_hline(term, x_min+1, y_min, x_max - x_min - 1);
		terminal_draw_vline(term, x_min, y_min+1, y_max - y_min - 1);
		if(x_max < width)
		{
			term->setxy(term, x_max, y_min);
			terminal_curses_putch(term, TERMINAL_RIGHTUP);
			terminal_draw_vline(term, x_max, y_min+1, y_max - y_min - 1);
		}
		if(y_max < height)
		{
			term->setxy(term, x_min, y_max);
			terminal_curses_putch(term, TERMINAL_LEFTDOWN);
			terminal_draw_hline(term, x_min+1, y_max, x_max - x_min - 1);
		}
		if(x_max < width && y_max < height)
		{
			term->setxy(term, x_max, y_max);
			terminal_curses_putch(term, TERMINAL_RIGHTDOWN);
		}
		return TRUE;
	}
	return FALSE;
}
