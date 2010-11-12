/*
 * drivers/fb/fb.c
 *
 * Copyright (c) 2007-2009  jianjun jiang <jerryjianjun@gmail.com>
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
#include <xboot.h>
#include <malloc.h>
#include <xboot/chrdev.h>
#include <xboot/ioctl.h>
#include <console/console.h>
#include <fb/logo.h>
#include <fb/graphic.h>
#include <fb/fb.h>

extern x_bool fb_putcode(struct fb * fb, x_u32 code, x_u32 fc, x_u32 bc, x_u32 x, x_u32 y);
extern x_bool fb_codewidth(x_u32 code, x_u32 * w, x_u32 * h);

struct fbcon_cell
{
	/* code pointer */
	x_u32 cp;

	/* front color and background color */
	x_u32 fc, bc;

	/* character width and height, the unit of font's width and height in pixel */
	x_u32 cw, ch;
};

/*
 * defined the framebuffer console information
 */
struct fb_console_info
{
	/* the console name */
	char * name;

	/* framebuffer driver */
	struct fb * fb;

	/* console font width and height in pixel */
	x_s32 fw, fh;

	/* console width and height */
	x_s32 w, h;

	/* console current x, y */
	x_s32 x, y;

	/* console front color and background color */
	enum console_color f, b;
	x_u32 fc, bc;

	/* cursor status, on or off */
	x_bool cursor;

	/* fb console's cell */
	struct fbcon_cell * cell;

	/* fb console cell's length */
	x_u32 clen;

	/*
	 * below for priv data
	 */
	x_u32 white, black;
};

/*
 * fb open
 */
static x_s32 fb_open(struct chrdev * dev)
{
	return 0;
}

/*
 * fb read
 */
static x_s32 fb_read(struct chrdev * dev, x_u8 * buf, x_s32 count)
{
	struct fb * fb = (struct fb *)(dev->driver);
	x_u8 * p = (x_u8 *)((x_u32)(fb->info->bitmap.data));
	x_s32 i;

	for(i = 0; i < count; i++)
	{
		buf[i] = p[i];
	}

	return i;
}

/*
 * fb write.
 */
static x_s32 fb_write(struct chrdev * dev, const x_u8 * buf, x_s32 count)
{
	struct fb * fb = (struct fb *)(dev->driver);
	x_u8 * p = (x_u8 *)((x_u32)(fb->info->bitmap.data));
	x_s32 i;

	for(i = 0; i < count; i++)
	{
		p[i] = buf[i];
	}

	return i;
}

/*
 * fb ioctl
 */
static x_s32 fb_ioctl(struct chrdev * dev, x_u32 cmd, void * arg)
{
	struct fb * fb = (struct fb *)(dev->driver);

	if(fb->ioctl)
		return ((fb->ioctl)(cmd, arg));

	return -1;
}

/*
 * fb close
 */
static x_s32 fb_close(struct chrdev * dev)
{
	return 0;
}

/*
 * console color to rgba
 */
static x_bool console_color_to_rgba(enum console_color c, x_u8 * r, x_u8 * g, x_u8 * b, x_u8 * a)
{
	switch(c)
	{
	case CONSOLE_BLACK:
		*r = 0;
		*g = 0;
		*b = 0;
		*a = 255;
		break;

	case CONSOLE_RED:
		*r = 255;
		*g = 0;
		*b = 0;
		*a = 255;
		break;

	case CONSOLE_GREEN:
		*r = 0;
		*g = 255;
		*b = 0;
		*a = 255;
		break;

	case CONSOLE_YELLOW:
		*r = 255;
		*g = 255;
		*b = 0;
		*a = 255;
		break;

	case CONSOLE_BULE:
		*r = 0;
		*g = 0;
		*b = 255;
		*a = 255;
		break;

	case CONSOLE_MAGENTA:
		*r = 255;
		*g = 0;
		*b = 255;
		*a = 255;
		break;

	case CONSOLE_CYAN:
		*r = 0;
		*g = 255;
		*b = 255;
		*a = 255;
		break;

	case CONSOLE_WHITE:
		*r = 255;
		*g = 255;
		*b = 255;
		*a = 255;
		break;

	default:
		*r = 0;
		*g = 0;
		*b = 0;
		*a = 255;
		break;
	}

	return TRUE;
}

/*
 * get console's width and height
 */
static x_bool fbcon_getwh(struct console * console, x_s32 * w, x_s32 * h)
{
	struct fb_console_info * info = console->priv;

	if(w)
		*w = info->w;

	if(h)
		*h = info->h;

	return TRUE;
}

/*
 * get cursor position
 */
static x_bool fbcon_getxy(struct console * console, x_s32 * x, x_s32 * y)
{
	struct fb_console_info * info = console->priv;

	if(x)
		*x = info->x;

	if(y)
		*y = info->y;

	return TRUE;
}

/*
 * set cursor position
 */
static x_bool fbcon_gotoxy(struct console * console, x_s32 x, x_s32 y)
{
	struct fb_console_info * info = console->priv;
	struct fbcon_cell * cell;
	x_s32 pos, px, py;

	if(x < 0)
		x = 0;
	if(y < 0)
		y = 0;

	if(x > info->w - 1)
		x = info->w - 1;
	if(y > info->h - 1)
		y = info->h - 1;

	if(info->cursor)
	{
		pos = info->w * info->y + info->x;
		cell = &(info->cell[pos]);
		px = (pos % info->w) * info->fw;
		py = (pos / info->w) * info->fh;
		fb_putcode(info->fb, cell->cp, cell->fc, cell->bc, px, py);

		pos = info->w * y + x;
		cell = &(info->cell[pos]);
		px = (pos % info->w) * info->fw;
		py = (pos / info->w) * info->fh;
		fb_putcode(info->fb, cell->cp, info->black, info->white, px, py);
	}

	info->x = x;
	info->y = y;

	return TRUE;
}

/*
 * turn on/off the cursor
 */
static x_bool fbcon_setcursor(struct console * console, x_bool on)
{
	struct fb_console_info * info = console->priv;
	struct fbcon_cell * cell;
	x_s32 pos, px, py;

	info->cursor = on;

	pos = info->w * info->y + info->x;
	cell = &(info->cell[pos]);
	px = (pos % info->w) * info->fw;
	py = (pos / info->w) * info->fh;

	if(info->cursor)
		fb_putcode(info->fb, cell->cp, info->black, info->white, px, py);
	else
		fb_putcode(info->fb, cell->cp, cell->fc, cell->bc, px, py);

	return TRUE;
}

/*
 * set console's front color and background color
 */
static x_bool fbcon_setcolor(struct console * console, enum console_color f, enum console_color b)
{
	struct fb_console_info * info = console->priv;
	x_u8 cr, cg, cb, ca;

	info->f = f;
	info->b = b;

	console_color_to_rgba(f, &cr, &cg, &cb, &ca);
	info->fc = fb_map_color(info->fb, cr, cg, cb, ca);

	console_color_to_rgba(b, &cr, &cg, &cb, &ca);
	info->bc = fb_map_color(info->fb, cr, cg, cb, ca);

	return TRUE;
}

/*
 * get console front color and background color
 */
static x_bool fbcon_getcolor(struct console * console, enum console_color * f, enum console_color * b)
{
	struct fb_console_info * info = console->priv;

	*f = info->f;
	*b = info->b;

	return TRUE;
}

/*
 * clear screen
 */
static x_bool fbcon_cls(struct console * console)
{
	struct fb_console_info * info = console->priv;
	struct fbcon_cell * cell = &(info->cell[0]);
	x_s32 px, py;
	x_u32 w, h;
	x_s32 i;

	if(fb_codewidth(UNICODE_SPACE, &w, &h))
	{
		w = (w + info->fw - 1) / info->fw;
		h = (h + info->fh - 1) / info->fh;
	}
	else
	{
		w = 1;
		h = 1;
	}

	for(i = 0; i < info->clen; i++)
	{
		if( (cell->cp != UNICODE_SPACE) || (cell->fc != info->black) || (cell->bc != info->black) )
		{
			cell->cp = UNICODE_SPACE;
			cell->fc = info->black;
			cell->bc = info->black;
			cell->cw = w;
			cell->ch = h;

			px = (i % info->w) * info->fw;
			py = (i / info->w) * info->fh;
			fb_putcode(info->fb, cell->cp, cell->fc, cell->bc, px, py);
		}
		cell++;
	}

	fbcon_gotoxy(console, 0, 0);
	return TRUE;
}

/*
 * scroll up
 */
static x_bool fbcon_scrollup(struct console * console)
{
	struct fb_console_info * info = console->priv;
	struct fbcon_cell * p, * q;
	x_s32 i = 0, px, py;
	x_u32 m, l;
	x_u32 w, h;

	l = info->w;
	m = info->clen - l;
	p = &(info->cell[0]);
	q = &(info->cell[l]);

	if(fb_codewidth(UNICODE_SPACE, &w, &h))
	{
		w = (w + info->fw - 1) / info->fw;
		h = (h + info->fh - 1) / info->fh;
	}
	else
	{
		w = 1;
		h = 1;
	}

	while(m--)
	{
		if( (p->cp != q->cp) || (p->fc != q->fc) || (p->bc != q->bc) )
		{
			p->cp = q->cp;
			p->fc = q->fc;
			p->bc = q->bc;
			p->cw = q->cw;
			p->ch = q->ch;

			px = (i % info->w) * info->fw;
			py = (i / info->w) * info->fh;
			fb_putcode(info->fb, p->cp, p->fc, p->bc, px, py);
		}

		p++;
		q++;
		i++;
	}

	while(l--)
	{
		if( (p->cp != UNICODE_SPACE) || (p->fc != info->black) || (p->bc != info->black) )
		{
			p->cp = UNICODE_SPACE;
			p->fc = info->black;
			p->bc = info->black;
			p->cw = w;
			p->ch = h;

			px = (i % info->w) * info->fw;
			py = (i / info->w) * info->fh;
			fb_putcode(info->fb, p->cp, p->fc, p->bc, px, py);
		}

		p++;
		i++;
	}

	fbcon_gotoxy(console, info->x, info->y - 1);
	return TRUE;
}

/*
 * put a unicode character
 */
x_bool fbcon_putcode(struct console * console, x_u32 code)
{
	struct fb_console_info * info = console->priv;
	struct fbcon_cell * cell;
	x_s32 pos, px, py;
	x_u32 w, h;
	x_s32 i;

	switch(code)
	{
	case UNICODE_BS:
		if(info->x > 0)
		{
			if(fb_codewidth(UNICODE_SPACE, &w, &h))
			{
				w = (w + info->fw - 1) / info->fw;
				h = (h + info->fh - 1) / info->fh;
			}
			else
			{
				w = 1;
				h = 1;
			}

			fbcon_gotoxy(console, info->x - 1, info->y);

			pos = info->w * info->y + info->x;
			cell = &(info->cell[pos]);

			cell->cp = UNICODE_SPACE;
			cell->fc = info->black;
			cell->bc = info->black;
			cell->cw = w;
			cell->ch = h;

			px = (pos % info->w) * info->fw;
			py = (pos / info->w) * info->fh;
			fb_putcode(info->fb, cell->cp, cell->fc, cell->bc, px, py);

			fbcon_setcursor(console, info->cursor);
		}
		break;

	case UNICODE_TAB:
		i = info->w - info->x;
		if(i < 0)
			i = 0;
		if(i > 4)
			i = 4;

		if(fb_codewidth(UNICODE_SPACE, &w, &h))
		{
			w = (w + info->fw - 1) / info->fw;
			h = (h + info->fh - 1) / info->fh;
		}
		else
		{
			w = 1;
			h = 1;
		}

		while(i--)
		{
			pos = info->w * info->y + info->x;
			cell = &(info->cell[pos]);

			cell->cp = UNICODE_SPACE;
			cell->fc = info->black;
			cell->bc = info->black;
			cell->cw = w;
			cell->ch = h;

			px = (pos % info->w) * info->fw;
			py = (pos / info->w) * info->fh;
			fb_putcode(info->fb, cell->cp, cell->fc, cell->bc, px, py);

			if(info->x + 1 >= info->w)
			{
				if(info->y + 1 >= info->h)
				{
					fbcon_scrollup(console);
				}
				fbcon_gotoxy(console, 0, info->y + 1);
			}
			else
			{
				fbcon_gotoxy(console, info->x + 1, info->y);
			}
		}
		break;

	case UNICODE_LF:
		if(info->y + 1 >= info->h)
		{
			fbcon_scrollup(console);
		}
		fbcon_gotoxy(console, info->x, info->y + 1);
		break;

	case UNICODE_CR:
		fbcon_gotoxy(console, 0, info->y);
		break;

	default:
		if(fb_codewidth(code, &w, &h))
		{
			w = (w + info->fw - 1) / info->fw;
			h = (h + info->fh - 1) / info->fh;
		}
		else
		{
			w = 1;
			h = 1;
		}

		pos = info->w * info->y + info->x;
		cell = &(info->cell[pos]);

		cell->cp = code;
		cell->fc = info->fc;
		cell->bc = info->bc;
		cell->cw = w;
		cell->ch = h;

		px = (pos % info->w) * info->fw;
		py = (pos / info->w) * info->fh;
		fb_putcode(info->fb, cell->cp, cell->fc, cell->bc, px, py);

		if(info->x + 1 >= info->w)
		{
			if(info->y + 1 >= info->h)
			{
				fbcon_scrollup(console);
			}
			fbcon_gotoxy(console, 0, info->y + 1);
		}
		else
		{
			fbcon_gotoxy(console, info->x + 1, info->y);
		}

		break;
	}

	return TRUE;
}

/*
 * search framebuffer by name.
 */
struct fb * search_framebuffer(const char * name)
{
	struct fb * fb;
	struct chrdev * dev;

	dev = search_chrdev(name);
	if(!dev)
		return NULL;

	if(dev->type != CHR_DEV_FRAMEBUFFER)
		return NULL;

	fb = (struct fb *)dev->driver;

	return fb;
}

/*
 * register framebuffer driver.
 */
x_bool register_framebuffer(struct fb * fb)
{
	struct chrdev * dev;
	struct console * console;
	struct fb_console_info * info;
	x_u32 fw, fh;
	x_u8 brightness;

	if(!fb || !fb->info || !fb->info->name)
		return FALSE;

	dev = malloc(sizeof(struct chrdev));
	if(!dev)
		return FALSE;

	dev->name		= fb->info->name;
	dev->type		= CHR_DEV_FRAMEBUFFER;
	dev->open 		= fb_open;
	dev->read 		= fb_read;
	dev->write 		= fb_write;
	dev->ioctl 		= fb_ioctl;
	dev->close		= fb_close;
	dev->driver 	= fb;

	if(!register_chrdev(dev))
	{
		free(dev);
		return FALSE;
	}

	if(search_chrdev_with_type(dev->name, CHR_DEV_FRAMEBUFFER) == NULL)
	{
		unregister_chrdev(dev->name);
		free(dev);
		return FALSE;
	}

	if(fb->init)
		(fb->init)();

	display_logo(fb);

	if(fb->ioctl)
	{
		brightness = 0xff;
		(fb->ioctl)(IOCTL_SET_FB_BACKLIGHT, &brightness);
	}

	/*
	 * register a console
	 */
	console = malloc(sizeof(struct console));
	info = malloc(sizeof(struct fb_console_info));
	if(!console || !info)
	{
		unregister_chrdev(dev->name);
		free(dev);
		free(console);
		free(info);
		return FALSE;
	}

	if(!fb_codewidth(UNICODE_SPACE, &fw, &fh))
	{
		fw = 8;
		fh = 16;
	}

	info->name = (char *)fb->info->name;
	info->fb = fb;
	info->fw = fw;
	info->fh = fh;
	info->w = fb->info->bitmap.info.width / info->fw;
	info->h = fb->info->bitmap.info.height / info->fh;
	info->x = 0;
	info->y = 0;
	info->f = CONSOLE_WHITE;
	info->b = CONSOLE_BLACK;
	info->white = fb_map_color(fb, 255, 255, 255, 255);
	info->black = fb_map_color(fb, 0, 0, 0, 255);
	info->fc = info->white;
	info->bc = info->black;
	info->cursor = TRUE;
	info->clen = info->w * info->h;
	info->cell = malloc(info->clen * sizeof(struct fbcon_cell));
	if(!info->cell)
	{
		unregister_chrdev(dev->name);
		free(dev);
		free(console);
		free(info);
		return FALSE;
	}
	memset(info->cell, 0, info->clen * sizeof(struct fbcon_cell));

	console->name = info->name;
	console->getwh = fbcon_getwh;
	console->getxy = fbcon_getxy;
	console->gotoxy = fbcon_gotoxy;
	console->setcursor = fbcon_setcursor;
	console->setcolor = fbcon_setcolor;
	console->getcolor = fbcon_getcolor;
	console->cls = fbcon_cls;
	console->getcode = NULL;
	console->putcode = fbcon_putcode;
	console->priv = info;

	if(!register_console(console))
	{
		unregister_chrdev(dev->name);
		free(dev);
		free(console);
		free(info->cell);
		free(info);
		return FALSE;
	}

	return TRUE;
}

/*
 * unregister framebuffer driver
 */
x_bool unregister_framebuffer(struct fb * fb)
{
	struct chrdev * dev;
	struct console * console;
	struct fb_console_info * info;
	struct fb * driver;
	x_u8 brightness;

	if(!fb || !fb->info || !fb->info->name)
		return FALSE;

	dev = search_chrdev_with_type(fb->info->name, CHR_DEV_FRAMEBUFFER);
	if(!dev)
		return FALSE;

	console = search_console((char *)fb->info->name);
	if(console)
		info = (struct fb_console_info *)console->priv;
	else
		return FALSE;

	driver = (struct fb *)(dev->driver);
	if(driver)
	{
		if(driver->ioctl)
		{
			brightness = 0x00;
			(fb->ioctl)(IOCTL_SET_FB_BACKLIGHT, &brightness);
		}

		if(driver->exit)
			(driver->exit)();
	}

	if(!unregister_console(console))
		return FALSE;

	if(!unregister_chrdev(dev->name))
		return FALSE;

	free(info->cell);
	free(info);
	free(console);
	free(dev);

	return TRUE;
}
