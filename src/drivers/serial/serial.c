/*
 * drivers/serial/serial.c
 *
 * Copyright (c) 2007-2010  jianjun jiang <jerryjianjun@gmail.com>
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


#include <configs.h>
#include <default.h>
#include <types.h>
#include <string.h>
#include <malloc.h>
#include <charset.h>
#include <vsprintf.h>
#include <xboot/printk.h>
#include <xboot/chrdev.h>
#include <console/console.h>
#include <serial/serial.h>

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
	x_s32 w, h;

	/* console current x, y */
	x_s32 x, y;

	/* console front color and background color */
	enum tcolor f, b;

	/* cursor status, on or off */
	x_bool cursor;

	/*
	 * below for priv data
	 */
	x_s8 buf[6];
	x_s32 size;
};

/*
 * serial open
 */
static x_s32 serial_open(struct chrdev * dev)
{
	return 0;
}

/*
 * serial read
 */
static x_s32 serial_read(struct chrdev * dev, x_u8 * buf, x_s32 count)
{
	struct serial_driver * drv = (struct serial_driver *)(dev->driver);

	if(drv->read)
		return ((drv->read)(buf, count));

	return 0;
}

/*
 * serial write.
 */
static x_s32 serial_write(struct chrdev * dev, const x_u8 * buf, x_s32 count)
{
	struct serial_driver * drv = (struct serial_driver *)(dev->driver);

	if(drv->write)
		return ((drv->write)(buf, count));

	return 0;
}

/*
 * serial ioctl
 */
static x_s32 serial_ioctl(struct chrdev * dev, x_u32 cmd, void * arg)
{
	struct serial_driver * drv = (struct serial_driver *)(dev->driver);

	if(drv->ioctl)
		return ((drv->ioctl)(cmd, arg));

	return -1;
}

/*
 * serial close
 */
static x_s32 serial_close(struct chrdev * dev)
{
	return 0;
}

/*
 * get console's width and height
 */
static x_bool scon_getwh(struct console * console, x_s32 * w, x_s32 * h)
{
	struct serial_console_info * info = console->priv;

	if(w)
		*w = info->w;

	if(h)
		*h = info->h;

	return TRUE;
}

/*
 * get cursor's position
 */
static x_bool scon_getxy(struct console * console, x_s32 * x, x_s32 * y)
{
	struct serial_console_info * info = console->priv;

	if(x)
		*x = info->x;

	if(y)
		*y = info->y;

	return TRUE;
}

/*
 * set cursor's position
 */
static x_bool scon_gotoxy(struct console * console, x_s32 x, x_s32 y)
{
	struct serial_console_info * info = console->priv;
	x_s8 buf[32];

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

	sprintf(buf, (const x_s8 *)"\033[%ld;%ldH", y + 1, x + 1);
	info->drv->write((const x_u8 *)buf, strlen(buf));

	return TRUE;
}

/*
 * turn on/off the cursor
 */
static x_bool scon_setcursor(struct console * console, x_bool on)
{
	struct serial_console_info * info = console->priv;
	x_s8 buf[32];

	info->cursor = on;

	if(on)
		sprintf(buf, (const x_s8 *)"\033[?25h");
	else
		sprintf(buf, (const x_s8 *)"\033[?25l");
	info->drv->write((const x_u8 *)buf, strlen(buf));

	return TRUE;
}

/*
 * get cursor's status
 */
static x_bool scon_getcursor(struct console * console)
{
	struct serial_console_info * info = console->priv;

	return info->cursor;
}

/*
 * set console's front color and background color
 */
static x_bool scon_setcolor(struct console * console, enum tcolor f, enum tcolor b)
{
	struct serial_console_info * info = console->priv;
	x_s8 buf[32];

	info->f = f;
	info->b = b;

	sprintf(buf, (const x_s8 *)"\033[38;5;%ldm", (x_u32)f);
	info->drv->write((const x_u8 *)buf, strlen(buf));

	sprintf(buf, (const x_s8 *)"\033[48;5;%ldm", (x_u32)b);
	info->drv->write((const x_u8 *)buf, strlen(buf));

	return TRUE;
}

/*
 * get console front color and background color
 */
static x_bool scon_getcolor(struct console * console, enum tcolor * f, enum tcolor * b)
{
	struct serial_console_info * info = console->priv;

	*f = info->f;
	*b = info->b;

	return TRUE;
}

/*
 * clear screen
 */
static x_bool scon_cls(struct console * console)
{
	struct serial_console_info * info = console->priv;
	x_s8 buf[32];

	sprintf(buf, (const x_s8 *)"\033[%ld;%ldr", (x_s32)1, (x_s32)info->h);
	info->drv->write((const x_u8 *)buf, strlen(buf));

	sprintf(buf, (const x_s8 *)"\033[2J");
	info->drv->write((const x_u8 *)buf, strlen(buf));

	sprintf(buf, (const x_s8 *)"\033[%ld;%ldH", (x_s32)1, (x_s32)1);
	info->drv->write((const x_u8 *)buf, strlen(buf));

	info->x = 0;
	info->y = 0;

	return TRUE;
}

/*
 * get a unicode character
 */
x_bool scon_getcode(struct console * console, x_u32 * code)
{
	struct serial_console_info * info = console->priv;
	x_u32 cp;
	x_s8 * rest;
	x_s8 c;

	if(info->drv->read((x_u8 *)&c, 1))
	{
		info->buf[info->size++] = c;
		while(utf8_to_ucs4(&cp, 1, info->buf, info->size, (const x_s8 **)&rest) > 0)
		{
			info->size -= rest - info->buf;
			memmove(info->buf, rest, info->size);

			*code = cp;
			return TRUE;
		}
	}

	return FALSE;
}

/*
 * put a unicode character
 */
x_bool scon_putcode(struct console * console, x_u32 code)
{
	struct serial_console_info * info = console->priv;
	x_s8 buf[32];
	x_s32 i;

	switch(code)
	{
	case UNICODE_BS:
		if(info->x > 0)
			info->x = info->x - 1;

		ucs4_to_utf8(code, buf);
		info->drv->write((const x_u8 *)buf, strlen(buf));
		break;

	case UNICODE_TAB:
		i = info->w - info->x;
		if(i < 0)
			i = 0;
		if(i > 4)
			i = 4;

		while(i--)
		{
			info->x = info->x + 1;

			ucs4_to_utf8(UNICODE_SPACE, buf);
			info->drv->write((const x_u8 *)buf, strlen(buf));
		}
		break;

	case UNICODE_LF:
		if(info->y + 1 < info->h)
			info->y = info->y + 1;
		ucs4_to_utf8(code, buf);
		info->drv->write((const x_u8 *)buf, strlen(buf));
		break;

	case UNICODE_CR:
		info->x = 0;
		ucs4_to_utf8(code, buf);
		info->drv->write((const x_u8 *)buf, strlen(buf));
		break;

	default:
		if(info->x + 1 < info->w)
			info->x = info->x + 1;
		else
		{
			if(info->y + 1 < info->h)
				info->y = info->y + 1;
			info->x = 0;
		}

		ucs4_to_utf8(code, buf);
		info->drv->write((const x_u8 *)buf, strlen(buf));
		break;
	}

	return TRUE;
}

/*
 * register serial driver
 */
x_bool register_serial(struct serial_driver * drv)
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
x_bool unregister_serial(struct serial_driver * drv)
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
