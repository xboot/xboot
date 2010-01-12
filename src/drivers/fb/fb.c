/*
 * drivers/fb/fb.c
 *
 * Copyright (c) 2007-2009  jianjun jiang <jerryjianjun@gmail.com>
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
#include <xboot.h>
#include <malloc.h>
#include <xboot/chrdev.h>
#include <fb/logo.h>
#include <fb/graphic.h>
#include <fb/fblinear.h>
#include <fb/fb.h>


#define TERM_MODE_HIGHLIGHT		(0x01)
#define TERM_MODE_UNDERLINE		(0x02)
#define TERM_MODE_REVERSE		(0x04)
#define TERM_MODE_BLANKING		(0x08)

/*
 * defined the struct of framebuffer terminal information.
 */
struct fb_terminal_info
{
	/* the tty name. */
	char name[32+1];

	/* framebuffer driver */
	struct fb * fb;

	/* terminal width and height */
	x_s32 width, height;

	/* terminal current x,y */
	x_s32 x, y;

	/* saved cursor */
	x_s32 x_save, y_save;

	/* cursor status, on or off */
	x_bool cursor_status;

	/* terminal mode */
	x_u32 mode;

	/* terminal front color and background color */
	enum terminal_color fc, bc;

	/*
	 * below for priv data
	 */
	/* save reverse cursor postion and status */
	x_s32 rev_x, rev_y;
	x_bool rev_flag;

	/* front color and background color (true color) */
	x_u32 f, b;
};

/*
 * fb open
 */
static x_s32 fb_open(struct chrdev * dev)
{
	return 0;
}

/*
 * fb seek
 */
static x_s32 fb_seek(struct chrdev * dev, x_s32 offset)
{
	struct fb * fb = (struct fb *)(dev->driver);

	fb->info->pos = offset;

	return (0);
}

/*
 * fb read
 */
static x_s32 fb_read(struct chrdev * dev, x_u8 * buf, x_s32 count)
{
	struct fb * fb = (struct fb *)(dev->driver);
	x_u8 * p = (x_u8 *)((x_u32)(fb->info->base) + fb->info->pos);
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
	x_u8 * p = (x_u8 *)((x_u32)(fb->info->base) + fb->info->pos);
	x_s32 i;

	for(i = 0; i < count; i++)
	{
		p[i] = buf[i];
	}

	return i;
}

/*
 * fb flush
 */
static x_s32 fb_flush(struct chrdev * dev)
{
	return 0;
}

/*
 * fb ioctl
 */
static x_s32 fb_ioctl(struct chrdev * dev, x_u32 cmd, x_u32 arg)
{
	struct fb * fb = (struct fb *)(dev->driver);

	if(fb->ioctl)
		return ((fb->ioctl)(cmd, arg));

	return -1;
}

/*
 * fb release
 */
static x_s32 fb_release(struct chrdev * dev)
{
	return 0;
}

/*
 * clear cursor
 */
static void fbcon_clear_cursor(struct fb_terminal_info * info)
{
	if(info->rev_flag)
	{
		fb_reverse_text(info->fb, info->rev_x, info->rev_y);
		info->rev_flag = FALSE;
	}
}

/*
 * updata cursor
 */
static void fbcon_updata_cursor(struct fb_terminal_info * info)
{
	if(info->cursor_status)
	{
		fbcon_clear_cursor(info);
		info->rev_flag = TRUE;
		fb_reverse_text(info->fb, info->x, info->y);
		info->rev_x = info->x;
		info->rev_y = info->y;
	}
	else
		fbcon_clear_cursor(info);
}

/*
 * get terminal's width and height.
 */
static x_bool fb_term_getwh(struct terminal * term, x_s32 * w, x_s32 * h)
{
	struct fb_terminal_info * info = term->priv;

	*w = info->width;
	*h = info->height;

	return TRUE;
}

/*
 * set cursor position
 */
static x_bool fb_term_setxy(struct terminal * term, x_s32 x, x_s32 y)
{
	struct fb_terminal_info * info = term->priv;

	info->x = x;
	info->y = y;

	fbcon_updata_cursor(info);

	return TRUE;
}

/*
 * get cursor position
 */
static x_bool fb_term_getxy(struct terminal * term, x_s32 * x, x_s32 * y)
{
	struct fb_terminal_info * info = term->priv;

	*x = info->x;
	*y = info->y;

	return TRUE;
}

/*
 * move cursor to left with n line
 */
static x_bool fb_term_cursor_left(struct terminal * term, x_u32 n)
{
	struct fb_terminal_info * info = term->priv;

	if(n == 0)
		return TRUE;

	if(info->x - n > 0)
		info->x = info->x - n;
	else
		info->x = 0;

	fbcon_updata_cursor(info);

	return TRUE;
}

/*
 * move cursor to right with n line
 */
static x_bool fb_term_cursor_right(struct terminal * term, x_u32 n)
{
	struct fb_terminal_info * info = term->priv;

	if(n == 0)
		return TRUE;

	if(info->x + n < info->width)
		info->x = info->x + n;
	else
		info->x = info->width - 1;

	fbcon_updata_cursor(info);

	return TRUE;
}

/*
 * move cursor to up with n line
 */
static x_bool fb_term_cursor_up(struct terminal * term, x_u32 n)
{
	struct fb_terminal_info * info = term->priv;

	if(n == 0)
		return TRUE;

	if(info->y - n > 0)
		info->y = info->y - n;
	else
		info->y = 0;

	fbcon_updata_cursor(info);

	return TRUE;
}

/*
 * move cursor to down with n line
 */
static x_bool fb_term_cursor_down(struct terminal * term, x_u32 n)
{
	struct fb_terminal_info * info = term->priv;

	if(n == 0)
		return TRUE;

	if(info->y + n < info->height)
		info->y = info->y + n;
	else
		info->y = info->height - 1;

	fbcon_updata_cursor(info);

	return TRUE;
}

/*
 * move cursor to home.
 */
static x_bool fb_term_cursor_home(struct terminal * term)
{
	struct fb_terminal_info * info = term->priv;

	info->x = 0;
	info->y = 0;

	fbcon_updata_cursor(info);

	return TRUE;
}

/*
 * save cursor
 */
static x_bool fb_term_cursor_save(struct terminal * term)
{
	struct fb_terminal_info * info = term->priv;

	info->x_save = info->x;
	info->y_save = info->y;

	return TRUE;
}

/*
 * restore cursor
 */
static x_bool fb_term_cursor_restore(struct terminal * term)
{
	struct fb_terminal_info * info = term->priv;

	info->x = info->x_save;
	info->y = info->y_save;
	fbcon_updata_cursor(info);

	return TRUE;
}

/*
 * hide cursor.
 */
static x_bool fb_term_cursor_hide(struct terminal * term)
{
	struct fb_terminal_info * info = term->priv;

	info->cursor_status = FALSE;
	fbcon_updata_cursor(info);

	return TRUE;
}

/*
 * show cursor.
 */
static x_bool fb_term_cursor_show(struct terminal * term)
{
	struct fb_terminal_info * info = term->priv;

	info->cursor_status = TRUE;
	fbcon_updata_cursor(info);

	return TRUE;
}

/*
 * flash cursor.
 */
static x_bool fb_term_cursor_flash(struct terminal * term)
{
	struct fb_terminal_info * info = term->priv;

	info->cursor_status = TRUE;
	fbcon_updata_cursor(info);

	return TRUE;
}

/*
 * highlight mode.
 */
static x_bool fb_term_mode_highlight(struct terminal * term)
{
	struct fb_terminal_info * info = term->priv;

	info->mode |= TERM_MODE_HIGHLIGHT;
	return TRUE;
}

/*
 * underline mode.
 */
static x_bool fb_term_mode_underline(struct terminal * term)
{
	struct fb_terminal_info * info = term->priv;

	info->mode |= TERM_MODE_UNDERLINE;
	return TRUE;
}

/*
 * reverse mode.
 */
static x_bool fb_term_mode_reverse(struct terminal * term)
{
	struct fb_terminal_info * info = term->priv;

	info->mode |= TERM_MODE_REVERSE;
	return TRUE;
}

/*
 * blanking mode.
 */
static x_bool fb_term_mode_blanking(struct terminal * term)
{
	struct fb_terminal_info * info = term->priv;

	info->mode |= TERM_MODE_BLANKING;

	return TRUE;
}

/*
 * close all attribute
 */
static x_bool fb_term_mode_closeall(struct terminal * term)
{
	struct fb_terminal_info * info = term->priv;

	info->mode = 0;
	return TRUE;
}

/*
 * set front color and background color
 */
static x_bool fb_term_set_color(struct terminal * term, enum terminal_color f, enum terminal_color b)
{
	struct fb_terminal_info * info = term->priv;

	info->fc = f;
	info->bc = b;

	switch(f)
	{
	case TERMINAL_NONE:
		info->f = 0x00000000;
		break;
	case TERMINAL_BLACK:
		info->f = 0x00000000;
		break;
	case TERMINAL_RED:
		info->f = 0x00ff0000;
		break;
	case TERMINAL_GREEN:
		info->f = 0x0000ff00;
		break;
	case TERMINAL_YELLOW:
		info->f = 0x00ffff00;
		break;
	case TERMINAL_BULE:
		info->f = 0x000000ff;
		break;
	case TERMINAL_MAGENTA:
		info->f = 0x008b008b;
		break;
	case TERMINAL_CYAN:
		info->f = 0x0000ffff;
		break;
	case TERMINAL_WHITE:
		info->f = 0xffffffff;
		break;
	default:
		info->f = 0x00000000;
		break;
	}

	switch(b)
	{
	case TERMINAL_NONE:
		info->b = 0x00000000;
		break;
	case TERMINAL_BLACK:
		info->b = 0x00000000;
		break;
	case TERMINAL_RED:
		info->b = 0x00ff0000;
		break;
	case TERMINAL_GREEN:
		info->b = 0x0000ff00;
		break;
	case TERMINAL_YELLOW:
		info->b = 0x00ffff00;
		break;
	case TERMINAL_BULE:
		info->b = 0x000000ff;
		break;
	case TERMINAL_MAGENTA:
		info->b = 0x008b008b;
		break;
	case TERMINAL_CYAN:
		info->b = 0x0000ffff;
		break;
	case TERMINAL_WHITE:
		info->b = 0xffffffff;
		break;
	default:
		info->b = 0x00000000;
		break;
	}

	return TRUE;
}

/*
 * set front color and background color
 */
static x_bool fb_term_get_color(struct terminal * term, enum terminal_color * f, enum terminal_color * b)
{
	struct fb_terminal_info * info = term->priv;

	*f = info->fc;
	*b = info->bc;

	return TRUE;
}

/*
 * clear screen
 */
static x_bool fb_term_clear_screen(struct terminal * term)
{
	struct fb_terminal_info * info = term->priv;

	fb_clear_screen(info->fb);
	info->rev_flag = FALSE;
	fb_term_cursor_home(term);

	return TRUE;
}

/*
 * terminal write
 */
static x_s32 fb_term_write(struct terminal * term, const x_u8 * buf, x_s32 count)
{
	struct fb_terminal_info * info = term->priv;

	while(count--)
	{
		switch(*buf)
		{
		case '\r':
			info->x = 0;
			break;

		case '\n':
			info->y++;
			if(info->y >= info->height)
			{
			#if	defined(CONFIG_FB_SCROLL_UP) && (CONFIG_FB_SCROLL_UP > 0)
				fbcon_clear_cursor(info);
				fb_scrollup(info->fb);
				info->y--;
			#else
				fb_term_clear_screen(term);
			#endif
			}
			break;

		default:
			if(info->mode & TERM_MODE_REVERSE)
				fb_draw_text(info->fb, info->x, info->y, info->b, info->f, (x_u8 *)buf, 1);
			else
				fb_draw_text(info->fb, info->x, info->y, info->f, info->b, (x_u8 *)buf, 1);

			info->x++;
			if(info->x >= info->width)
			{
				info->x = 0;
				info->y++;
				if(info->y >= info->height)
				{
				#if	defined(CONFIG_FB_SCROLL_UP) && (CONFIG_FB_SCROLL_UP > 0)
					fbcon_clear_cursor(info);
					fb_scrollup(info->fb);
					info->y--;
				#else
					fb_term_clear_screen(term);
				#endif
				}
			}
			info->rev_flag = FALSE;
			break;
		}
		buf++;
		fbcon_updata_cursor(info);
	}

	return 0;
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
 * register framebuffer driver, return true is successed.
 */
x_bool register_framebuffer(struct fb * fb)
{
	struct chrdev * dev;
	struct terminal * term;
	struct fb_terminal_info * info;

	if(!fb || !fb->info || !fb->info->name || !(fb->set_pixel || fb->get_pixel))
		return FALSE;

	dev = malloc(sizeof(struct chrdev));
	if(!dev)
		return FALSE;

	dev->name		= fb->info->name;
	dev->type		= CHR_DEV_FRAMEBUFFER;
	dev->open 		= fb_open;
	dev->seek 		= fb_seek;
	dev->read 		= fb_read;
	dev->write 		= fb_write;
	dev->flush 		= fb_flush;
	dev->ioctl 		= fb_ioctl;
	dev->release 	= fb_release;
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

	/* display logo */
	display_logo(fb);

	if(fb->bl)
		(fb->bl)(0xff);

	/*
	 * register a terminal
	 */
	term = malloc(sizeof(struct terminal));
	info = malloc(sizeof(struct fb_terminal_info));
	if(!term || !info)
	{
		unregister_chrdev(dev->name);
		free(dev);
		free(term);
		free(info);
		return FALSE;
	}

	strcpy((x_s8*)info->name, (x_s8*)"tty-");
	strlcat((x_s8*)info->name, (x_s8*)fb->info->name, 32+1);
	info->fb = fb;
	info->width = fb->info->width/8;
	info->height = fb->info->height/16;
	info->x = 0;
	info->y = 0;
	info->x_save = 0;
	info->y_save = 0;
	info->cursor_status = TRUE;
	info->mode = 0;
	info->fc = TERMINAL_WHITE;
	info->bc = TERMINAL_BLACK;

	info->rev_x = 0;
	info->rev_y = 0;
	info->rev_flag = FALSE;
	info->f = 0xffffffff;
	info->b = 0x00000000;

	term->name = info->name;
	term->priv = info;
	term->type = TERMINAL_LCD;
	term->getwh = fb_term_getwh;
	term->setxy = fb_term_setxy;
	term->getxy = fb_term_getxy;
	term->cursor_left = fb_term_cursor_left;
	term->cursor_right = fb_term_cursor_right;
	term->cursor_up = fb_term_cursor_up;
	term->cursor_down = fb_term_cursor_down;
	term->cursor_home = fb_term_cursor_home;
	term->cursor_save = fb_term_cursor_save;
	term->cursor_restore = fb_term_cursor_restore;
	term->cursor_hide = fb_term_cursor_hide;
	term->cursor_show = fb_term_cursor_show;
	term->cursor_flash = fb_term_cursor_flash;
	term->mode_highlight = fb_term_mode_highlight;
	term->mode_underline = fb_term_mode_underline;
	term->mode_reverse = fb_term_mode_reverse;
	term->mode_blanking = fb_term_mode_blanking;
	term->mode_closeall = fb_term_mode_closeall;
	term->set_color = fb_term_set_color;
	term->get_color = fb_term_get_color;
	term->clear_screen = fb_term_clear_screen;
	term->read = NULL;
	term->write = fb_term_write;

	if(!register_terminal(term))
	{
		unregister_chrdev(dev->name);
		free(dev);
		free(term);
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
	struct fb * driver;
	x_s8 term_name[32+1];
	struct terminal * term;
	struct fb_terminal_info * info;

	if(!fb || !fb->info || !fb->info->name)
		return FALSE;

	dev = search_chrdev_with_type(fb->info->name, CHR_DEV_FRAMEBUFFER);
	if(!dev)
		return FALSE;

	strcpy(term_name, (x_s8*)"tty-");
	strlcat(term_name, (x_s8*)fb->info->name, 32+1);

	term = search_terminal((char *)term_name);
	if(term)
		info = (struct fb_terminal_info *)term->priv;
	else
		return FALSE;

	driver = (struct fb *)(dev->driver);
	if(driver)
	{
		if(driver->bl)
			(driver->bl)(0x00);

		if(driver->exit)
			(driver->exit)();
	}

	if(!unregister_terminal(term))
		return FALSE;

	if(!unregister_chrdev(dev->name))
		return FALSE;

	free(info);
	free(term);
	free(dev);

	return TRUE;
}
