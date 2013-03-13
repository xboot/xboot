/*
 * drivers/serial/serial.c
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
#include <types.h>
#include <string.h>
#include <malloc.h>
#include <charset.h>
#include <stdio.h>
#include <xboot/printk.h>
#include <xboot/chrdev.h>
#include <console/console.h>
#include <serial/serial.h>

enum tty_state {
	TTY_STATE_NORMAL,
	TTY_STATE_ESC,
	TTY_STATE_CSI,
};

/*
 * defined the serial console information
 */
struct serial_console_info
{
	/* the console name */
	char * name;

	/* serial driver */
	struct serial_driver * drv;

	/* console width and height */
	s32_t w, h;

	/* console current x, y */
	s32_t x, y;

	/* console front color and background color */
	enum tcolor f, b;

	/* cursor status, on or off */
	bool_t cursor;

	/* on/off status */
	bool_t onoff;

	/*
	 * below for private data
	 */
	enum tty_state state;
	s32_t params[8];
	s32_t num_params;

	s8_t utf8[32];
	s32_t size;
};

/*
 * serial open
 */
static int serial_open(struct chrdev * dev)
{
	return 0;
}

/*
 * serial read
 */
static ssize_t serial_read(struct chrdev * dev, u8_t * buf, size_t count)
{
	struct serial_driver * drv = (struct serial_driver *)(dev->driver);

	if(drv->read)
		return ((drv->read)(buf, count));

	return 0;
}

/*
 * serial write.
 */
static ssize_t serial_write(struct chrdev * dev, const u8_t * buf, size_t count)
{
	struct serial_driver * drv = (struct serial_driver *)(dev->driver);

	if(drv->write)
		return ((drv->write)(buf, count));

	return 0;
}

/*
 * serial ioctl
 */
static int serial_ioctl(struct chrdev * dev, int cmd, void * arg)
{
	struct serial_driver * drv = (struct serial_driver *)(dev->driver);

	if(drv->ioctl)
		return ((drv->ioctl)(cmd, arg));

	return -1;
}

/*
 * serial close
 */
static int serial_close(struct chrdev * dev)
{
	return 0;
}

/*
 * get console's width and height
 */
static bool_t scon_getwh(struct console * console, s32_t * w, s32_t * h)
{
	struct serial_console_info * info = console->priv;

	if(!info->onoff)
		return FALSE;

	if(w)
		*w = info->w;

	if(h)
		*h = info->h;

	return TRUE;
}

/*
 * get cursor's position
 */
static bool_t scon_getxy(struct console * console, s32_t * x, s32_t * y)
{
	struct serial_console_info * info = console->priv;

	if(!info->onoff)
		return FALSE;

	if(x)
		*x = info->x;

	if(y)
		*y = info->y;

	return TRUE;
}

/*
 * set cursor's position
 */
static bool_t scon_gotoxy(struct console * console, s32_t x, s32_t y)
{
	struct serial_console_info * info = console->priv;
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

	sprintf(buf, (const char *)"\033[%ld;%ldH", y + 1, x + 1);
	info->drv->write((const u8_t *)buf, strlen(buf));

	return TRUE;
}

/*
 * turn on/off the cursor
 */
static bool_t scon_setcursor(struct console * console, bool_t on)
{
	struct serial_console_info * info = console->priv;
	char buf[32];

	if(!info->onoff)
		return FALSE;

	info->cursor = on;

	if(on)
		sprintf(buf, (const char *)"\033[?25h");
	else
		sprintf(buf, (const char *)"\033[?25l");
	info->drv->write((const u8_t *)buf, strlen(buf));

	return TRUE;
}

/*
 * get cursor's status
 */
static bool_t scon_getcursor(struct console * console)
{
	struct serial_console_info * info = console->priv;

	if(!info->onoff)
		return FALSE;

	return info->cursor;
}

/*
 * set console's front color and background color
 */
static bool_t scon_setcolor(struct console * console, enum tcolor f, enum tcolor b)
{
	struct serial_console_info * info = console->priv;
	char buf[32];

	if(!info->onoff)
		return FALSE;

	info->f = f;
	info->b = b;

	sprintf(buf, (const char *)"\033[38;5;%ldm", (u32_t)f);
	info->drv->write((const u8_t *)buf, strlen(buf));

	sprintf(buf, (const char *)"\033[48;5;%ldm", (u32_t)b);
	info->drv->write((const u8_t *)buf, strlen(buf));

	return TRUE;
}

/*
 * get console front color and background color
 */
static bool_t scon_getcolor(struct console * console, enum tcolor * f, enum tcolor * b)
{
	struct serial_console_info * info = console->priv;

	if(!info->onoff)
		return FALSE;

	*f = info->f;
	*b = info->b;

	return TRUE;
}

/*
 * clear screen
 */
static bool_t scon_cls(struct console * console)
{
	struct serial_console_info * info = console->priv;
	char buf[32];

	if(!info->onoff)
		return FALSE;

	sprintf(buf, (const char *)"\033[%ld;%ldr", (s32_t)1, (s32_t)info->h);
	info->drv->write((const u8_t *)buf, strlen(buf));

	sprintf(buf, (const char *)"\033[2J");
	info->drv->write((const u8_t *)buf, strlen(buf));

	sprintf(buf, (const char *)"\033[%ld;%ldH", (s32_t)1, (s32_t)1);
	info->drv->write((const u8_t *)buf, strlen(buf));

	info->x = 0;
	info->y = 0;

	return TRUE;
}

/*
 * get a unicode character
 */
bool_t scon_getcode(struct console * console, u32_t * code)
{
	struct serial_console_info * info = console->priv;
	s8_t c;
	s32_t i;
	u32_t cp;
	s8_t * rest;

	if(!info->onoff)
		return FALSE;

	if(info->drv->read((u8_t *)&c, 1) != 1)
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

/*
 * put a unicode character
 */
bool_t scon_putcode(struct console * console, u32_t code)
{
	struct serial_console_info * info = console->priv;
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
	info->drv->write((const u8_t *)buf, strlen(buf));

	return TRUE;
}

/*
 * turn no / off console
 */
bool_t scon_onoff(struct console * console, bool_t flag)
{
	struct serial_console_info * info = console->priv;

	info->onoff = flag;
	return TRUE;
}

/*
 * register serial driver
 */
bool_t register_serial(struct serial_driver * drv)
{
	struct chrdev * dev;
	struct console * console;
	struct serial_console_info * info;

	if(!drv || !drv->info || !drv->info->name || !(drv->read || drv->write))
		return FALSE;

	dev = malloc(sizeof(struct chrdev));
	if(!dev)
		return FALSE;

	dev->name		= drv->info->name;
	dev->type		= CHR_DEV_SERIAL;
	dev->open 		= serial_open;
	dev->read 		= serial_read;
	dev->write 		= serial_write;
	dev->ioctl 		= serial_ioctl;
	dev->close		= serial_close;
	dev->driver 	= drv;

	if(!register_chrdev(dev))
	{
		free(dev);
		return FALSE;
	}

	if(search_chrdev_with_type(dev->name, CHR_DEV_SERIAL) == NULL)
	{
		unregister_chrdev(dev->name);
		free(dev);
		return FALSE;
	}

	if(drv->init)
		(drv->init)();

	/*
	 * register a console
	 */
	console = malloc(sizeof(struct console));
	info = malloc(sizeof(struct serial_console_info));
	if(!console || !info)
	{
		unregister_chrdev(dev->name);
		free(dev);
		free(console);
		free(info);
		return FALSE;
	}

	info->name = (char *)drv->info->name;
	info->drv = drv;
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

	console->name = info->name;
	console->getwh = scon_getwh;
	console->getxy = scon_getxy;
	console->gotoxy = scon_gotoxy;
	console->setcursor = scon_setcursor;
	console->getcursor = scon_getcursor;
	console->setcolor = scon_setcolor;
	console->getcolor = scon_getcolor;
	console->cls = scon_cls;
	console->getcode = scon_getcode;
	console->putcode = scon_putcode;
	console->onoff = scon_onoff;
	console->priv = info;

	if(!register_console(console))
	{
		unregister_chrdev(dev->name);
		free(dev);
		free(console);
		free(info);
		return FALSE;
	}

	return TRUE;
}

/*
 * unregister serial driver
 */
bool_t unregister_serial(struct serial_driver * drv)
{
	struct chrdev * dev;
	struct console * console;
	struct serial_console_info * info;
	struct serial_driver * driver;

	if(!drv || !drv->info || !drv->info->name)
		return FALSE;

	dev = search_chrdev_with_type(drv->info->name, CHR_DEV_SERIAL);
	if(!dev)
		return FALSE;

	console = search_console((char *)drv->info->name);
	if(console)
		info = (struct serial_console_info *)console->priv;
	else
		return FALSE;

	driver = (struct serial_driver *)(dev->driver);
	if(driver && driver->exit)
		(driver->exit)();

	if(!unregister_console(console))
		return FALSE;

	if(!unregister_chrdev(dev->name))
		return FALSE;

	free(info);
	free(console);
	free(dev);

	return TRUE;
}
