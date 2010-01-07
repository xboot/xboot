/*
 * drivers/serial/serial.c
 *
 *
 * Copyright (c) 2007-2008  jianjun jiang <jerryjianjun@gmail.com>
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
#include <string.h>
#include <malloc.h>
#include <vsprintf.h>
#include <xboot/major.h>
#include <xboot/printk.h>
#include <xboot/chrdev.h>
#include <terminal/terminal.h>
#include <serial/serial.h>

/*
 * defined the struct of serial terminal information.
 */
struct serial_terminal_info
{
	/* the tty name. */
	char name[32+1];

	/* serial driver */
	struct serial_driver * drv;

	/* terminal width and height */
	x_s32 width, height;

	/* terminal current x,y */
	x_s32 x, y;

	/* saved cursor */
	x_s32 x_save, y_save;

	/* terminal front color and background color */
	enum terminal_color fc, bc;
};

/*
 * serial open
 */
static x_s32 serial_open(struct chrdev * dev)
{
	return 0;
}

/*
 * serial seek
 */
static x_s32 serial_seek(struct chrdev * dev, x_s32 offset)
{
	return (-1);
}

/*
 * serial read
 */
static x_s32 serial_read(struct chrdev * dev, x_u8 * buf, x_s32 count)
{
	struct serial_driver * drv = (struct serial_driver *)(dev->ops->driver);

	if(drv->read)
		return ((drv->read)(buf, count));

	return 0;
}

/*
 * serial write.
 */
static x_s32 serial_write(struct chrdev * dev, const x_u8 * buf, x_s32 count)
{
	struct serial_driver * drv = (struct serial_driver *)(dev->ops->driver);

	if(drv->write)
		return ((drv->write)(buf, count));

	return 0;
}

/*
 * serial flush
 */
static x_s32 serial_flush(struct chrdev * dev)
{
	struct serial_driver * drv = (struct serial_driver *)(dev->ops->driver);

	if(drv->flush)
		(drv->flush)();

	return 0;
}

/*
 * serial ioctl
 */
static x_s32 serial_ioctl(struct chrdev * dev, x_u32 cmd, x_u32 arg)
{
	struct serial_driver * drv = (struct serial_driver *)(dev->ops->driver);

	if(drv->ioctl)
		return ((drv->ioctl)(cmd, arg));

	return -1;
}

/*
 * serial release
 */
static x_s32 serial_release(struct chrdev * dev)
{
	return 0;
}

/*
 * get terminal's width and height.
 */
static x_bool serial_term_getwh(struct terminal * term, x_s32 * w, x_s32 * h)
{
	struct serial_terminal_info * info = term->priv;

	*w = info->width;
	*h = info->height;

	return TRUE;
}

/*
 * set cursor position
 */
static x_bool serial_term_setxy(struct terminal * term, x_s32 x, x_s32 y)
{
	struct serial_terminal_info * info = term->priv;
	x_s8 buf[32];

	info->x = x;
	info->y = y;

	sprintf(buf, (const x_s8 *)"\033[%ld;%ldH", y+1, x+1);
	info->drv->write((const x_u8 *)buf, strlen(buf));

	return TRUE;
}

/*
 * get cursor position
 */
static x_bool serial_term_getxy(struct terminal * term, x_s32 * x, x_s32 * y)
{
	struct serial_terminal_info * info = term->priv;

	*x = info->x;
	*y = info->y;

	return TRUE;
}

/*
 * move cursor to left with n line
 */
static x_bool serial_term_cursor_left(struct terminal * term, x_u32 n)
{
	struct serial_terminal_info * info = term->priv;
	x_s8 buf[32];

	if(n == 0)
		return TRUE;

	if(info->x - n > 0)
		info->x = info->x - n;
	else
		info->x = 0;

	sprintf(buf, (const x_s8 *)"\033[%ldD", n);
	info->drv->write((const x_u8 *)buf, strlen(buf));

	return TRUE;
}

/*
 * move cursor to right with n line
 */
static x_bool serial_term_cursor_right(struct terminal * term, x_u32 n)
{
	struct serial_terminal_info * info = term->priv;
	x_s8 buf[32];

	if(n == 0)
		return TRUE;

	if(info->x + n < info->width)
		info->x = info->x + n;
	else
		info->x = info->width - 1;

	sprintf(buf, (const x_s8 *)"\033[%ldC", n);
	info->drv->write((const x_u8 *)buf, strlen(buf));

	return TRUE;
}

/*
 * move cursor to up with n line
 */
static x_bool serial_term_cursor_up(struct terminal * term, x_u32 n)
{
	struct serial_terminal_info * info = term->priv;
	x_s8 buf[32];

	if(n == 0)
		return TRUE;

	if(info->y - n > 0)
		info->y = info->y - n;
	else
		info->y = 0;

	sprintf(buf, (const x_s8 *)"\033[%ldA", n);
	info->drv->write((const x_u8 *)buf, strlen(buf));

	return TRUE;
}

/*
 * move cursor to down with n line
 */
static x_bool serial_term_cursor_down(struct terminal * term, x_u32 n)
{
	struct serial_terminal_info * info = term->priv;
	x_s8 buf[32];

	if(n == 0)
		return TRUE;

	if(info->y + n < info->height)
		info->y = info->y + n;
	else
		info->y = info->height - 1;

	sprintf(buf, (const x_s8 *)"\033[%ldB", n);
	info->drv->write((const x_u8 *)buf, strlen(buf));

	return TRUE;
}

/*
 * move cursor to home.
 */
static x_bool serial_term_cursor_home(struct terminal * term)
{
	struct serial_terminal_info * info = term->priv;
	x_s8 buf[32];

	info->x = 0;
	info->y = 0;

	sprintf(buf, (const x_s8 *)"\033[H");
	info->drv->write((const x_u8 *)buf, strlen(buf));

	return TRUE;
}

/*
 * save cursor
 */
static x_bool serial_term_cursor_save(struct terminal * term)
{
	struct serial_terminal_info * info = term->priv;
	x_s8 buf[32];

	info->x_save = info->x;
	info->y_save = info->y;

	sprintf(buf, (const x_s8 *)"\033[s");
	info->drv->write((const x_u8 *)buf, strlen(buf));

	return TRUE;
}

/*
 * restore cursor
 */
static x_bool serial_term_cursor_restore(struct terminal * term)
{
	struct serial_terminal_info * info = term->priv;
	x_s8 buf[32];

	info->x = info->x_save;
	info->y = info->y_save;

	sprintf(buf, (const x_s8 *)"\033[u");
	info->drv->write((const x_u8 *)buf, strlen(buf));

	return TRUE;
}

/*
 * hide cursor.
 */
static x_bool serial_term_cursor_hide(struct terminal * term)
{
	struct serial_terminal_info * info = term->priv;
	x_s8 buf[32];

	sprintf(buf, (const x_s8 *)"\033[?25l");
	info->drv->write((const x_u8 *)buf, strlen(buf));

	return TRUE;
}

/*
 * show cursor.
 */
static x_bool serial_term_cursor_show(struct terminal * term)
{
	struct serial_terminal_info * info = term->priv;
	x_s8 buf[32];

	sprintf(buf, (const x_s8 *)"\033[?25h");
	info->drv->write((const x_u8 *)buf, strlen(buf));

	return TRUE;
}

/*
 * flash cursor.
 */
static x_bool serial_term_cursor_flash(struct terminal * term)
{
	struct serial_terminal_info * info = term->priv;
	x_s8 buf[32];

	sprintf(buf, (const x_s8 *)"\033[5m");
	info->drv->write((const x_u8 *)buf, strlen(buf));

	return TRUE;
}

/*
 * highlight mode.
 */
static x_bool serial_term_mode_highlight(struct terminal * term)
{
	struct serial_terminal_info * info = term->priv;
	x_s8 buf[32];

	sprintf(buf, (const x_s8 *)"\033[1m");
	info->drv->write((const x_u8 *)buf, strlen(buf));

	return TRUE;
}

/*
 * underline mode.
 */
static x_bool serial_term_mode_underline(struct terminal * term)
{
	struct serial_terminal_info * info = term->priv;
	x_s8 buf[32];

	sprintf(buf, (const x_s8 *)"\033[4m");
	info->drv->write((const x_u8 *)buf, strlen(buf));

	return TRUE;
}

/*
 * reverse mode.
 */
static x_bool serial_term_mode_reverse(struct terminal * term)
{
	struct serial_terminal_info * info = term->priv;
	x_s8 buf[32];

	sprintf(buf, (const x_s8 *)"\033[7m");
	info->drv->write((const x_u8 *)buf, strlen(buf));

	return TRUE;
}

/*
 * blanking mode.
 */
static x_bool serial_term_mode_blanking(struct terminal * term)
{
	struct serial_terminal_info * info = term->priv;
	x_s8 buf[32];

	sprintf(buf, (const x_s8 *)"\033[8m");
	info->drv->write((const x_u8 *)buf, strlen(buf));

	return TRUE;
}

/*
 * close all attribute
 */
static x_bool serial_term_mode_closeall(struct terminal * term)
{
	struct serial_terminal_info * info = term->priv;
	x_s8 buf[32];

	sprintf(buf, (const x_s8 *)"\033[0m");
	info->drv->write((const x_u8 *)buf, strlen(buf));

	return TRUE;
}

/*
 * set front color and background color
 */
static x_bool serial_term_set_color(struct terminal * term, enum terminal_color f, enum terminal_color b)
{
	struct serial_terminal_info * info = term->priv;
	x_s8 buf[32];
	x_u32 fc, bc;

	info->fc = f;
	info->bc = b;

	switch(f)
	{
	case TERMINAL_NONE:
		fc = 0;
		break;
	case TERMINAL_BLACK:
		fc = 30;
		break;
	case TERMINAL_RED:
		fc = 31;
		break;
	case TERMINAL_GREEN:
		fc = 32;
		break;
	case TERMINAL_YELLOW:
		fc = 33;
		break;
	case TERMINAL_BULE:
		fc = 34;
		break;
	case TERMINAL_MAGENTA:
		fc = 35;
		break;
	case TERMINAL_CYAN:
		fc = 36;
		break;
	case TERMINAL_WHITE:
		fc = 37;
		break;
	default:
		fc = 0;
		break;
	}

	switch(b)
	{
	case TERMINAL_NONE:
		bc = 0;
		break;
	case TERMINAL_BLACK:
		bc = 40;
		break;
	case TERMINAL_RED:
		bc = 41;
		break;
	case TERMINAL_GREEN:
		bc = 42;
		break;
	case TERMINAL_YELLOW:
		bc = 43;
		break;
	case TERMINAL_BULE:
		bc = 44;
		break;
	case TERMINAL_MAGENTA:
		bc = 45;
		break;
	case TERMINAL_CYAN:
		bc = 46;
		break;
	case TERMINAL_WHITE:
		bc = 47;
		break;
	default:
		bc = 0;
		break;
	}

	if(fc && bc)
		sprintf(buf, (const x_s8 *)"\033[%ld;%ldm", bc, fc);
	else
	{
		if(fc)
			sprintf(buf, (const x_s8 *)"\033[%ldm", fc);
		if(bc)
			sprintf(buf, (const x_s8 *)"\033[%ldm", bc);
	}

	info->drv->write((const x_u8 *)buf, strlen(buf));

	return TRUE;
}

/*
 * set front color and background color
 */
static x_bool serial_term_get_color(struct terminal * term, enum terminal_color * f, enum terminal_color * b)
{
	struct serial_terminal_info * info = term->priv;

	*f = info->fc;
	*b = info->bc;

	return TRUE;
}

/*
 * clear screen
 */
static x_bool serial_term_clear_screen(struct terminal * term)
{
	struct serial_terminal_info * info = term->priv;
	x_s8 buf[32];

	sprintf(buf, (const x_s8 *)"\033[2J");
	info->drv->write((const x_u8 *)buf, strlen(buf));

	return TRUE;
}

/*
 * terminal read
 */
static x_s32 serial_term_read(struct terminal * term, x_u8 * buf, x_s32 count)
{
	struct serial_terminal_info * info = term->priv;

	return info->drv->read(buf, count);
}

/*
 * terminal write
 */
static x_s32 serial_term_write(struct terminal * term, const x_u8 * buf, x_s32 count)
{
	struct serial_terminal_info * info = term->priv;
	x_s32 x,y;

	x = (x_u32)(count + info->x) % (x_u32)info->width;
	y = info->y + (x_u32)(count + info->x) / (x_u32)info->width;

	if(y > info->height - 1)
		y = info->height -1;

	info->x = x;
	info->y = y;

	return info->drv->write(buf, count);
}

/*
 * register serial driver, return true is successed.
 */
x_bool register_serial(struct serial_driver * drv)
{
	struct char_operations * ops;
	struct chrdev * dev;
	struct terminal * term;
	struct serial_terminal_info * info;

	if(!drv || !drv->info || !(drv->read || drv->write))
		return FALSE;

	ops = malloc(sizeof(struct char_operations));
	if(!ops)
		return FALSE;

	ops->open 		= serial_open;
	ops->seek 		= serial_seek;
	ops->read 		= serial_read;
	ops->write 		= serial_write;
	ops->flush 		= serial_flush;
	ops->ioctl 		= serial_ioctl;
	ops->release 	= serial_release;
	ops->driver 	= drv;

	if(!register_chrdev(MAJOR_SERIAL, drv->info->name, ops))
	{
		free(ops);
		return FALSE;
	}

	dev = search_chrdev_by_major_name(MAJOR_SERIAL, drv->info->name);
	if(!dev)
	{
		unregister_chrdev(MAJOR_SERIAL, drv->info->name);
		free(ops);
		return FALSE;
	}

	drv->device = dev;

	if(drv->init)
		(drv->init)();

	if(drv->flush)
		(drv->flush)();

	/* register a terminal */
	term = malloc(sizeof(struct terminal));
	info = malloc(sizeof(struct serial_terminal_info));
	if(!term || !info)
	{
		unregister_chrdev(MAJOR_SERIAL, drv->info->name);
		free(ops);
		free(term);
		free(info);
		return FALSE;
	}

	strcpy((x_s8*)info->name, (x_s8*)"tty-");
	strlcat((x_s8*)info->name, (x_s8*)drv->info->name, 32+1);
	info->drv = drv;
	info->width = 80;
	info->height = 24;
	info->x = 0;
	info->y = 0;
	info->x_save = 0;
	info->y_save = 0;
	info->fc = TERMINAL_WHITE;
	info->bc = TERMINAL_BLACK;

	term->name = info->name;
	term->priv = info;
	term->type = TERMINAL_SERIAL;
	term->getwh = serial_term_getwh;
	term->setxy = serial_term_setxy;
	term->getxy = serial_term_getxy;
	term->cursor_left = serial_term_cursor_left;
	term->cursor_right = serial_term_cursor_right;
	term->cursor_up = serial_term_cursor_up;
	term->cursor_down = serial_term_cursor_down;
	term->cursor_home = serial_term_cursor_home;
	term->cursor_save = serial_term_cursor_save;
	term->cursor_restore = serial_term_cursor_restore;
	term->cursor_hide = serial_term_cursor_hide;
	term->cursor_show = serial_term_cursor_show;
	term->cursor_flash = serial_term_cursor_flash;
	term->mode_highlight = serial_term_mode_highlight;
	term->mode_underline = serial_term_mode_underline;
	term->mode_reverse = serial_term_mode_reverse;
	term->mode_blanking = serial_term_mode_blanking;
	term->mode_closeall = serial_term_mode_closeall;
	term->set_color = serial_term_set_color;
	term->get_color = serial_term_get_color;
	term->clear_screen = serial_term_clear_screen;
	term->read = serial_term_read;
	term->write = serial_term_write;

	if(!register_terminal(term))
	{
		unregister_chrdev(MAJOR_SERIAL, drv->info->name);
		free(ops);
		free(term);
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
	struct char_operations * ops;
	x_s8 term_name[32+1];
	struct terminal * term;
	struct serial_terminal_info * info;

	if(!drv || !drv->device)
		return FALSE;

	dev = drv->device;
	ops = dev->ops;

	strcpy(term_name, (x_s8*)"tty-");
	strlcat(term_name, (x_s8*)drv->info->name, 32+1);

	term = search_terminal((char *)term_name);
	if(term)
		info = (struct serial_terminal_info *)term->priv;
	else
		return FALSE;

	if(drv->exit)
		(drv->exit)();

	if(!unregister_terminal(term))
		return FALSE;

	if(!unregister_chrdev(dev->major, dev->name))
		return FALSE;

	free(info);
	free(term);
	free(ops);

	return TRUE;
}
