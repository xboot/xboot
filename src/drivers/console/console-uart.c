/*
 * drivers/console/console-uart.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
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

#include <xboot.h>
#include <console/console.h>
#include <console/console-uart.h>

enum tty_state_t {
	TTY_STATE_NORMAL,
	TTY_STATE_ESC,
	TTY_STATE_CSI,
};

struct uart_console_info_t
{
	/* the console name */
	char * name;

	/* uart device */
	struct uart_t * uart;

	/* console width and height */
	s32_t w, h;

	/* console current x, y */
	s32_t x, y;

	/* console front color and background color */
	enum tcolor_t f, b;

	/* cursor status, on or off */
	bool_t cursor;

	/* on/off status */
	bool_t onoff;

	/*
	 * below for private data
	 */
	enum tty_state_t state;
	s32_t params[8];
	s32_t num_params;

	s8_t utf8[32];
	s32_t size;
};

static bool_t uart_console_getwh(struct console_t * console, s32_t * w, s32_t * h)
{
	struct uart_console_info_t * info = console->priv;

	if(!info->onoff)
		return FALSE;

	if(w)
		*w = info->w;

	if(h)
		*h = info->h;

	return TRUE;
}

static bool_t uart_console_getxy(struct console_t * console, s32_t * x, s32_t * y)
{
	struct uart_console_info_t * info = console->priv;

	if(!info->onoff)
		return FALSE;

	if(x)
		*x = info->x;

	if(y)
		*y = info->y;

	return TRUE;
}

static bool_t uart_console_gotoxy(struct console_t * console, s32_t x, s32_t y)
{
	struct uart_console_info_t * info = console->priv;
	char buf[32];

	if(!info->onoff)
		return FALSE;

	if(x < 0)
		x = 0;
	if(y < 0)
		y = 0;

	if(x > info->w - 1)
		x = info->w - 1;
	if(y > info->h - 1)
		y = info->h - 1;

	info->x = x;
	info->y = y;

	sprintf(buf, (const char *)"\033[%d;%dH", y + 1, x + 1);
	info->uart->write(info->uart, (const u8_t *)buf, strlen(buf));

	return TRUE;
}

static bool_t uart_console_setcursor(struct console_t * console, bool_t on)
{
	struct uart_console_info_t * info = console->priv;
	char buf[32];

	if(!info->onoff)
		return FALSE;

	info->cursor = on;

	if(on)
		sprintf(buf, "\033[?25h");
	else
		sprintf(buf, "\033[?25l");
	info->uart->write(info->uart, (const u8_t *)buf, strlen(buf));

	return TRUE;
}

static bool_t uart_console_getcursor(struct console_t * console)
{
	struct uart_console_info_t * info = console->priv;

	if(!info->onoff)
		return FALSE;

	return info->cursor;
}

static bool_t uart_console_setcolor(struct console_t * console, enum tcolor_t f, enum tcolor_t b)
{
	struct uart_console_info_t * info = console->priv;
	char buf[32];

	if(!info->onoff)
		return FALSE;

	info->f = f;
	info->b = b;

	sprintf(buf, "\033[38;5;%dm", (u32_t)f);
	info->uart->write(info->uart, (const u8_t *)buf, strlen(buf));

	sprintf(buf, "\033[48;5;%dm", (u32_t)b);
	info->uart->write(info->uart, (const u8_t *)buf, strlen(buf));

	return TRUE;
}

static bool_t uart_console_getcolor(struct console_t * console, enum tcolor_t * f, enum tcolor_t * b)
{
	struct uart_console_info_t * info = console->priv;

	if(!info->onoff)
		return FALSE;

	*f = info->f;
	*b = info->b;

	return TRUE;
}

static bool_t uart_console_cls(struct console_t * console)
{
	struct uart_console_info_t * info = console->priv;
	char buf[32];

	if(!info->onoff)
		return FALSE;

	sprintf(buf, "\033[%d;%dr", (s32_t)1, (s32_t)info->h);
	info->uart->write(info->uart, (const u8_t *)buf, strlen(buf));

	sprintf(buf, "\033[2J");
	info->uart->write(info->uart, (const u8_t *)buf, strlen(buf));

	sprintf(buf, "\033[%d;%dH", (s32_t)1, (s32_t)1);
	info->uart->write(info->uart, (const u8_t *)buf, strlen(buf));

	info->x = 0;
	info->y = 0;

	return TRUE;
}

static bool_t uart_console_getcode(struct console_t * console, u32_t * code)
{
	struct uart_console_info_t * info = console->priv;
	s8_t c;
	s32_t i;
	u32_t cp;
	s8_t * rest;

	if(!info->onoff)
		return FALSE;

	if(info->uart->read(info->uart, (u8_t *)&c, 1) != 1)
		return FALSE;

	switch(info->state)
	{
	case TTY_STATE_NORMAL:
		switch(c)
		{
		case 27:
			info->state = TTY_STATE_ESC;
			break;

		case 127:				/* backspace */
			*code = 0x8;		/* ctrl-h */
			return TRUE;

		default:
			info->utf8[info->size++] = c;
			if(utf8_to_ucs4(&cp, 1, (const char *)info->utf8, info->size, (const char **)&rest) > 0)
			{
				info->size -= rest - info->utf8;
				memmove(info->utf8, rest, info->size);

				*code = cp;
				return TRUE;
			}
			break;
		}
		break;

	case TTY_STATE_ESC:
		if(c == '[')
		{
			for(i = 0; i < ARRAY_SIZE(info->params); i++)
				info->params[i] = 0;
			info->num_params = 0;
			info->state = TTY_STATE_CSI;
		}
		else
		{
			info->state = TTY_STATE_NORMAL;
		}
		break;

	case TTY_STATE_CSI:
		if(c >= '0' && c <= '9')
		{
			if(info->num_params < ARRAY_SIZE(info->params))
			{
				info->params[info->num_params] = info->params[info->num_params] * 10 + c - '0';
			}
		}
		else
		{
			info->num_params++;
			if(c == ';')
				break;

			info->state = TTY_STATE_NORMAL;
			switch(c)
			{
			case 'A':				/* arrow up */
				*code = 0x10;		/* ctrl-p */
				return TRUE;

			case 'B':				/* arrow down */
				*code = 0xe			/* ctrl-n */;
				return TRUE;

			case 'C':				/* arrow right */
				*code = 0x6;		/* ctrl-f */
				return TRUE;

			case 'D':				/* arrow left */
				*code = 0x2;		/* ctrl-b */
				return TRUE;

			case '~':
				if(info->num_params != 1)
					break;

				switch(info->params[0])
				{
				case 1:				/* home */
					*code = 0x1;	/* ctrl-a */
					return TRUE;

				case 2:				/* insert */
					break;

				case 3:				/* delete */
					*code = 0x8;	/* ctrl-h */
					return TRUE;

				case 4:				/* end */
					*code = 0x5;	/* ctrl-e */
					return TRUE;

				case 5:				/* page up*/
					*code = 0x10;	/* ctrl-p */
					return TRUE;

				case 6:				/* page down*/
					*code = 0xe;	/* ctrl-n */
					return TRUE;

				default:
					break;
				}
				break;

			default:
				break;
			}
		}
		break;

	default:
		info->state = TTY_STATE_NORMAL;
		break;
	}

	return FALSE;
}

static bool_t uart_console_putcode(struct console_t * console, u32_t code)
{
	struct uart_console_info_t * info = console->priv;
	char buf[32];
	s32_t w, i;

	if(!info->onoff)
		return FALSE;

	w = ucs4_width(code);
	if(w < 0)
		w = 0;

	switch(code)
	{
	case UNICODE_BS:
		return TRUE;

	case UNICODE_TAB:
		i = 8 - (info->x % 8);
		if(i + info->x >= info->w)
			i = info->w - info->x - 1;
		info->x = info->x + i;
		break;

	case UNICODE_LF:
		if(info->y + 1 < info->h)
			info->y = info->y + 1;
		break;

	case UNICODE_CR:
		info->x = 0;
		break;

	default:
		if(info->x + w < info->w)
			info->x = info->x + w;
		else
		{
			if(info->y + 1 < info->h)
				info->y = info->y + 1;
			info->x = 0;
		}
		break;
	}

	ucs4_to_utf8(&code, 1, buf, sizeof(buf));
	info->uart->write(info->uart, (const u8_t *)buf, strlen(buf));

	return TRUE;
}

static bool_t uart_console_onoff(struct console_t * console, bool_t flag)
{
	struct uart_console_info_t * info = console->priv;

	info->onoff = flag;
	return TRUE;
}

bool_t register_console_uart(struct uart_t * uart)
{
	struct console_t * console;
	struct uart_console_info_t * info;

	if(!uart || !uart->name)
		return FALSE;

	console = malloc(sizeof(struct console_t));
	info = malloc(sizeof(struct uart_console_info_t));
	if(!console || !info)
	{
		free(console);
		free(info);
		return FALSE;
	}

	info->name = (char *)uart->name;
	info->uart = uart;
	info->w = 80;
	info->h = 24;
	info->x = 0;
	info->y = 0;
	info->f = TCOLOR_WHITE;
	info->b = TCOLOR_BLACK;
	info->cursor = TRUE;
	info->onoff = TRUE;
	info->state = TTY_STATE_NORMAL;
	info->num_params = 0;
	info->size = 0;

	console->name = strdup(info->name);
	console->getwh = uart_console_getwh;
	console->getxy = uart_console_getxy;
	console->gotoxy = uart_console_gotoxy;
	console->setcursor = uart_console_setcursor;
	console->getcursor = uart_console_getcursor;
	console->setcolor = uart_console_setcolor;
	console->getcolor = uart_console_getcolor;
	console->cls = uart_console_cls;
	console->getcode = uart_console_getcode;
	console->putcode = uart_console_putcode;
	console->onoff = uart_console_onoff;
	console->priv = info;

	if(!register_console(console))
	{
		free(console->name);
		free(console);
		free(info);
		return FALSE;
	}

	return TRUE;
}

bool_t unregister_console_uart(struct uart_t * uart)
{
	struct console_t * console;
	struct uart_console_info_t * info;

	if(!uart || !uart->name)
		return FALSE;

	console = search_console(uart->name);
	if(console)
		info = (struct uart_console_info_t *)console->priv;
	else
		return FALSE;

	if(!unregister_console(console))
		return FALSE;

	free(console->name);
	free(console);
	free(info);

	return TRUE;
}
