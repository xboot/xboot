/*
 * drivers/console/console-fb.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
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
#include <console/console-fb-font.h>
#include <console/console-fb.h>

enum esc_state_t {
	ESC_STATE_NORMAL,
	ESC_STATE_ESC,
	ESC_STATE_CSI,
};

enum tcolor_t {
	TCOLOR_BLACK	= 0x00,
	TCOLOR_RED		= 0x01,
	TCOLOR_GREEN	= 0x02,
	TCOLOR_YELLOW	= 0x03,
	TCOLOR_BULE		= 0x04,
	TCOLOR_MAGENTA	= 0x05,
	TCOLOR_CYAN		= 0x06,
	TCOLOR_WHITE	= 0x07,
};

struct cell_t
{
	u32_t cp;
	struct color_t fc, bc;
};

struct console_fb_data_t {
	struct fb_t * fb;

	int fw, fh;
	int w, h;
	int x, y;
	int sx, sy;
	int cursor;
	int bright;
	int sbright;
	enum tcolor_t f, b;
	enum tcolor_t sf, sb;
	struct color_t fc, bc;
	struct cell_t * cell;
	int clen;

	enum esc_state_t state;
	int cbuf[17 * 3];
	int csize;
	int abuf[17];
	int asize;
	char utf8[32];
	int usize;
};

static const u8_t tcolor_to_rgba_table[16][3] = {
	/* 0x00 */	{ 0x00, 0x00, 0x00 },
	/* 0x01 */	{ 0xcd, 0x00, 0x00 },
	/* 0x02 */	{ 0x00, 0xcd, 0x00 },
	/* 0x03 */	{ 0xcd, 0xcd, 0x00 },
	/* 0x04 */	{ 0x00, 0x00, 0xee },
	/* 0x05 */	{ 0xcd, 0x00, 0xcd },
	/* 0x06 */	{ 0x00, 0xcd, 0xcd },
	/* 0x07 */	{ 0xe5, 0xe5, 0xe5 },

	/* 0x08 */	{ 0x7f, 0x7f, 0x7f },
	/* 0x09 */	{ 0xff, 0x00, 0x00 },
	/* 0x0a */	{ 0x00, 0xff, 0x00 },
	/* 0x0b */	{ 0xff, 0xff, 0x00 },
	/* 0x0c */	{ 0x5c, 0x5c, 0xff },
	/* 0x0d */	{ 0xff, 0x00, 0xff },
	/* 0x0e */	{ 0x00, 0xff, 0xff },
	/* 0x0f */	{ 0xff, 0xff, 0xff },
};

static bool_t tcolor_to_color(enum tcolor_t c, int bright, struct color_t * col)
{
	u8_t index = c & 0x07;

	if(bright != 0)
		index += 8;

	col->r = tcolor_to_rgba_table[index][0];
	col->g = tcolor_to_rgba_table[index][1];
	col->b = tcolor_to_rgba_table[index][2];
	col->a = 0xff;
	return TRUE;
}

static void fb_helper_fill_rect(struct fb_t * fb, struct color_t * c, u32_t x, u32_t y, u32_t w, u32_t h)
{
	struct rect_t rect;

	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;

	render_clear(fb->alone, &rect, c);
}

static void fb_helper_blit(struct fb_t * fb, struct texture_t * texture, u32_t x, u32_t y, u32_t w, u32_t h, u32_t ox, u32_t oy)
{
	struct rect_t drect, srect;

	drect.x = x;
	drect.y = y;
	drect.w = w;
	drect.h = h;

	srect.x = ox;
	srect.y = oy;
	srect.w = w;
	srect.h = h;

	render_blit_texture(fb->alone, &drect, texture, &srect);
}

static void fb_helper_putcode(struct fb_t * fb, u32_t code, struct color_t * fc, struct color_t * bc, u32_t x, u32_t y)
{
	struct texture_t * face = lookup_console_font_face(fb->alone, code, fc, bc);

	if(face)
		fb_helper_blit(fb, face, x, y, face->width, face->height, 0, 0);
	render_free_texture(fb->alone, face);
}

static void console_fb_cursor_gotoxy(struct console_fb_data_t * dat, int x, int y)
{
	struct cell_t * cell;
	int pos, px, py;

	if(x < 0)
		x = 0;
	if(y < 0)
		y = 0;

	if(x > dat->w - 1)
		x = dat->w - 1;
	if(y > dat->h - 1)
		y = dat->h - 1;

	if(dat->cursor != 0)
	{
		pos = dat->w * dat->y + dat->x;
		cell = &(dat->cell[pos]);
		px = (pos % dat->w) * dat->fw;
		py = (pos / dat->w) * dat->fh;
		fb_helper_putcode(dat->fb, cell->cp, &(cell->fc), &(cell->bc), px, py);

		pos = dat->w * y + x;
		cell = &(dat->cell[pos]);
		px = (pos % dat->w) * dat->fw;
		py = (pos / dat->w) * dat->fh;
		fb_helper_putcode(dat->fb, cell->cp, &(dat->bc), &(dat->fc), px, py);
	}

	dat->x = x;
	dat->y = y;
}

static void console_fb_save_cursor(struct console_fb_data_t * dat)
{
	dat->sx = dat->x;
	dat->sy = dat->y;
}

static void console_fb_restore_cursor(struct console_fb_data_t * dat)
{
	console_fb_cursor_gotoxy(dat, dat->sx, dat->sy);
}

static void console_fb_show_cursor(struct console_fb_data_t * dat, int show)
{
	struct cell_t * cell;
	int pos, px, py;

	dat->cursor = (show != 0) ? 1 : 0;

	pos = dat->w * dat->y + dat->x;
	cell = &(dat->cell[pos]);
	px = (pos % dat->w) * dat->fw;
	py = (pos / dat->w) * dat->fh;

	if(dat->cursor != 0)
		fb_helper_putcode(dat->fb, cell->cp, &(dat->bc), &(dat->fc), px, py);
	else
		fb_helper_putcode(dat->fb, cell->cp, &(cell->fc), &(cell->bc), px, py);
}

static void console_fb_set_color(struct console_fb_data_t * dat, enum tcolor_t f, enum tcolor_t b)
{
	dat->f = f;
	dat->b = b;
	tcolor_to_color(f, dat->bright, &(dat->fc));
	tcolor_to_color(b, dat->bright, &(dat->bc));
}

static void console_fb_set_color_bright(struct console_fb_data_t * dat, int bright)
{
	dat->bright = (bright != 0) ? 1 : 0;
}

static void console_fb_save_color(struct console_fb_data_t * dat)
{
	dat->sf = dat->f;
	dat->sb = dat->b;
	dat->sbright = dat->bright;
}

static void console_fb_restore_color(struct console_fb_data_t * dat)
{
	console_fb_set_color_bright(dat, dat->sbright);
	console_fb_set_color(dat, dat->sf, dat->sb);
}

static void console_fb_clear_screen(struct console_fb_data_t * dat)
{
	struct cell_t * cell = &(dat->cell[0]);
	int i;

	for(i = 0; i < dat->clen; i++)
	{
		cell->cp = ' ';
		memcpy(&(cell->fc), &(dat->fc), sizeof(struct color_t));
		memcpy(&(cell->bc), &(dat->bc), sizeof(struct color_t));
		cell++;
	}

	fb_helper_fill_rect(dat->fb, &(dat->bc), 0, 0, (dat->w * dat->fw), (dat->h * dat->fh));
	console_fb_cursor_gotoxy(dat, 0, 0);
}

static void console_fb_scrollup(struct console_fb_data_t * dat)
{
	struct cell_t * p, * q;
	int m, l;
	int i;

	l = dat->w;
	m = dat->clen - l;
	p = &(dat->cell[0]);
	q = &(dat->cell[l]);

	for(i = 0; i < m; i++)
	{
		p->cp = q->cp;
		p->fc = q->fc;
		p->bc = q->bc;

		p++;
		q++;
	}

	while( (l--) > 0 )
	{
		p->cp = ' ';
		p->fc = dat->fc;
		p->bc = dat->bc;
		p++;
	}

	struct texture_t * t = render_snapshot(dat->fb->alone);
	fb_helper_blit(dat->fb, t, 0, 0, (dat->w * dat->fw), ((dat->h - 1) * dat->fh), 0, dat->fh);
	render_free_texture(dat->fb->alone, t);
	fb_helper_fill_rect(dat->fb, &(dat->bc), 0, ((dat->h - 1) * dat->fh), (dat->w * dat->fw), dat->fh);
	console_fb_cursor_gotoxy(dat, dat->x, dat->y - 1);
}

static void console_fb_putcode(struct console_fb_data_t * dat, u32_t code)
{
	struct cell_t * cell;
	int pos, px, py;
	int w, i;

	switch(code)
	{
	case '\b':
		return;

	case '\t':
		i = 8 - (dat->x % 8);
		if(i + dat->x >= dat->w)
			i = dat->w - dat->x - 1;

		while(i--)
		{
			pos = dat->w * dat->y + dat->x;
			cell = &(dat->cell[pos]);

			cell->cp = ' ';
			memcpy(&(cell->fc), &(dat->fc), sizeof(struct color_t));
			memcpy(&(cell->bc), &(dat->bc), sizeof(struct color_t));

			px = (pos % dat->w) * dat->fw;
			py = (pos / dat->w) * dat->fh;
			fb_helper_putcode(dat->fb, cell->cp, &(cell->fc), &(cell->bc), px, py);
			dat->x = dat->x + 1;
		}
		console_fb_cursor_gotoxy(dat, dat->x, dat->y);
		break;

	case '\r':
		console_fb_cursor_gotoxy(dat, 0, dat->y);
		break;

	case '\n':
		if(dat->y + 1 >= dat->h)
			console_fb_scrollup(dat);
		console_fb_cursor_gotoxy(dat, 0, dat->y + 1);
		break;

	default:
		w = ucs4_width(code);
		if(w <= 0)
			return;

		pos = dat->w * dat->y + dat->x;
		cell = &(dat->cell[pos]);

		cell->cp = code;
		memcpy(&(cell->fc), &(dat->fc), sizeof(struct color_t));
		memcpy(&(cell->bc), &(dat->bc), sizeof(struct color_t));

		for(i = 1; i < w; i++)
		{
			((struct cell_t *)(cell + i))->cp = ' ';
			((struct cell_t *)(cell + i))->fc = dat->fc;
			((struct cell_t *)(cell + i))->bc = dat->bc;
		}

		px = (pos % dat->w) * dat->fw;
		py = (pos / dat->w) * dat->fh;
		fb_helper_putcode(dat->fb, cell->cp, &(cell->fc), &(cell->bc), px, py);

		if(dat->x + w < dat->w)
			console_fb_cursor_gotoxy(dat, dat->x + w, dat->y);
		else
		{
			if(dat->y + 1 >= dat->h)
				console_fb_scrollup(dat);
			console_fb_cursor_gotoxy(dat, 0, dat->y + 1);
		}
		break;
	}
}

static void console_fb_write_byte(struct console_fb_data_t * dat, unsigned char c)
{
	u32_t cp;
	char * rest;
	int t;

	switch(dat->state)
	{
	case ESC_STATE_NORMAL:
		switch(c)
		{
		case '\e':	/* ESC state */
			dat->state = ESC_STATE_ESC;
			break;

		default:
			dat->utf8[dat->usize++] = c;
			if(utf8_to_ucs4(&cp, 1, (const char *)dat->utf8, dat->usize, (const char **)&rest) > 0)
			{
				dat->usize -= rest - dat->utf8;
				memmove(dat->utf8, rest, dat->usize);
				console_fb_putcode(dat, cp);
			}
			break;
		}
		break;

	case ESC_STATE_ESC:
		switch(c)
		{
		case 'c':	/* Reset */
			console_fb_show_cursor(dat, 1);
			console_fb_set_color_bright(dat, 0);
			console_fb_set_color(dat, TCOLOR_WHITE, TCOLOR_BLACK);
			dat->state = ESC_STATE_NORMAL;
			break;

		case 'D':	/* Scroll display down one line */
			dat->state = ESC_STATE_NORMAL;
			break;

		case 'M':	/* Scroll display up one line */
			dat->state = ESC_STATE_NORMAL;
			break;

		case '7':	/* Save cursor position and attrs */
			console_fb_save_cursor(dat);
			console_fb_save_color(dat);
			dat->state = ESC_STATE_NORMAL;
			break;

		case '8':	/* Restore cursor position and attrs */
			console_fb_restore_cursor(dat);
			console_fb_restore_color(dat);
			dat->state = ESC_STATE_NORMAL;
			break;

		case '[':	/* CSI codes */
			dat->csize = 0;
			dat->abuf[0] = 0;
			dat->asize = 0;
			dat->state = ESC_STATE_CSI;
			break;

		default:
			dat->state = ESC_STATE_NORMAL;
			break;
		}
		break;

	case ESC_STATE_CSI:
		dat->cbuf[dat->csize++] = c;

		switch(dat->cbuf[dat->csize - 1])
		{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			dat->abuf[dat->asize] *= 10;
			dat->abuf[dat->asize] += (dat->cbuf[dat->csize - 1] - '0');
			break;

		case ';':
			dat->asize++;
			dat->abuf[dat->asize] = 0;
			break;

		case 'A':	/* Move the cursor up */
			t = dat->abuf[0];
			t = (t) ? t : 1;
			console_fb_cursor_gotoxy(dat, dat->x, dat->y - t);
			dat->state = ESC_STATE_NORMAL;
			break;

		case 'B':	/* Move the cursor down */
			t = dat->abuf[0];
			t = (t) ? t : 1;
			console_fb_cursor_gotoxy(dat, dat->x, dat->y + t);
			dat->state = ESC_STATE_NORMAL;
			break;

		case 'C':	/* Move the cursor right */
			t = dat->abuf[0];
			t = (t) ? t : 1;
			console_fb_cursor_gotoxy(dat, dat->x + t, dat->y);
			dat->state = ESC_STATE_NORMAL;
			break;

		case 'D':	/* Move the cursor left */
			t = dat->abuf[0];
			t = (t) ? t : 1;
			console_fb_cursor_gotoxy(dat, dat->x - t, dat->y);
			dat->state = ESC_STATE_NORMAL;
			break;

		case 's':	/* Save cursor position */
			console_fb_save_cursor(dat);
			dat->state = ESC_STATE_NORMAL;
			break;

		case 'u':	/* Restore cursor position */
			console_fb_restore_cursor(dat);
			dat->state = ESC_STATE_NORMAL;
			break;

		case 'J':	/* Clear the screen */
			console_fb_clear_screen(dat);
			dat->state = ESC_STATE_NORMAL;
			break;

		case 'H':	/* Cursor home */
		case 'f':	/* Force cursor position */
			if(dat->asize == 0)
				console_fb_cursor_gotoxy(dat, 0, dat->y);
			else
				console_fb_cursor_gotoxy(dat, dat->abuf[1], dat->abuf[0]);
			dat->state = ESC_STATE_NORMAL;
			break;

		case 'c':		/* Request terminal Type */
			dat->state = ESC_STATE_NORMAL;
			break;

		case 'n':
			switch(dat->abuf[0])
			{
			case 5:		/* Request terminal status */
				break;
			case 6:		/* Request cursor position */
				break;
			};
			dat->state = ESC_STATE_NORMAL;
			break;

		case 'm':		/* Set Display Attributes */
			for(t = 0; t <= dat->asize; t++)
			{
				switch(dat->abuf[t])
				{
				case 0:		/* Reset all attrs */
					console_fb_set_color_bright(dat, 0);
					console_fb_set_color(dat, TCOLOR_WHITE, TCOLOR_BLACK);
					break;
				case 1:		/* Bright */
					console_fb_set_color_bright(dat, 1);
					console_fb_set_color(dat, dat->f, dat->b);
					break;
				case 2:		/* Dim */
					break;
				case 4:		/* Underscore */
					break;
				case 5:		/* Blink */
					break;
				case 7:		/* Reverse */
					console_fb_set_color(dat, dat->b, dat->f);
					break;
				case 8:		/* Hidden */
					break;

				case 30:	/* Set foreground color */
				case 31:
				case 32:
				case 33:
				case 34:
				case 35:
				case 36:
				case 37:
					console_fb_set_color(dat, dat->abuf[t] - 30, dat->b);
					break;

				case 40:	/* Set background color */
				case 41:
				case 42:
				case 43:
				case 44:
				case 45:
				case 46:
				case 47:
					console_fb_set_color(dat, dat->f, dat->abuf[t] - 40);
					break;

				default:
					break;
				}
			}
			dat->state = ESC_STATE_NORMAL;
			break;

		default:
			dat->state = ESC_STATE_NORMAL;
			break;
		}
		break;

	default:
		dat->state = ESC_STATE_NORMAL;
		break;
	}
}

static ssize_t console_fb_write(struct console_t * console, const unsigned char * buf, size_t count)
{
	struct console_fb_data_t * dat = (struct console_fb_data_t *)console->priv;
	size_t i;

	for(i = 0; i < count; i++)
		console_fb_write_byte(dat, buf[i]);
	return count;
}

static void console_fb_suspend(struct console_t * console)
{
}

static void console_fb_resume(struct console_t * console)
{
}

bool_t register_console_framebuffer(struct fb_t * fb)
{
	struct console_fb_data_t * dat;
	struct console_t * console;

	if(!fb || !fb->name)
		return FALSE;

	dat = malloc(sizeof(struct console_fb_data_t));
	if(!dat)
		return FALSE;

	console = malloc(sizeof(struct console_t));
	if(!console)
	{
		free(dat);
		return FALSE;
	}

	dat->fb = fb;
	dat->fw = 8;
	dat->fh = 16;
	dat->w = fb->alone->width / dat->fw;
	dat->h = fb->alone->height / dat->fh;
	dat->x = 0;
	dat->y = 0;
	dat->sx = dat->x;
	dat->sy = dat->y;
	dat->cursor = 1;
	dat->bright = 0;
	dat->sbright = dat->bright;
	dat->f = TCOLOR_WHITE;
	dat->b = TCOLOR_BLACK;
	dat->sf = dat->f;
	dat->sb = dat->b;
	tcolor_to_color(dat->f, dat->bright, &(dat->fc));
	tcolor_to_color(dat->b, dat->bright, &(dat->bc));
	dat->clen = dat->w * dat->h;
	dat->cell = malloc(dat->clen * sizeof(struct cell_t));
	if(!dat->cell)
	{
		free(console);
		free(dat);
		return FALSE;
	}
	memset(dat->cell, 0, dat->clen * sizeof(struct cell_t));
	dat->state = ESC_STATE_NORMAL;
	dat->csize = 0;
	dat->asize = 0;
	dat->usize = 0;

	console->name = strdup(fb->name);
	console->read = NULL,
	console->write = console_fb_write,
	console->suspend = console_fb_suspend,
	console->resume	= console_fb_resume,
	console->priv = dat;

	if(register_console(console))
		return TRUE;

	free(dat->cell);
	free(console->priv);
	free(console->name);
	free(console);
	return FALSE;
}

bool_t unregister_console_framebuffer(struct fb_t * fb)
{
	struct console_t * console;
	struct console_fb_data_t * dat;

	console = search_console(fb->name);
	if(!console)
		return FALSE;
	dat = (struct console_fb_data_t *)console->priv;

	if(!unregister_console(console))
		return FALSE;

	free(dat->cell);
	free(dat);
	free(console->name);
	free(console);
	return TRUE;
}
