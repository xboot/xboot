/*
 * kernel/shell/readline.c
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
#include <mode.h>
#include <ctype.h>
#include <string.h>
#include <charset.h>
#include <xboot/scank.h>
#include <xboot/printk.h>
#include <console/console.h>
#include <shell/history.h>
#include <shell/readline.h>

struct rl_buf {
	x_u32 * buf;
	x_u32 * cut;
	x_s32 size;
	x_s32 len;
	x_s32 pos;

	x_s32 x, y;
	x_s32 w, h;
};

static x_s32 ucs4_strlen(const x_u32 * s)
{
	const x_u32 * sc;

	for(sc = s; *sc != '\0'; ++sc);
	return sc - s;
}

static void rl_gotoxy(struct rl_buf * rl)
{
	x_s32 x, y;

	x = ((rl->y * rl->w) + rl->x + rl->pos) % (rl->w);
	y = ((rl->y * rl->w) + rl->x + rl->pos) / (rl->w);

	console_gotoxy(get_stdout(), x, y);
}

static void rl_space(struct rl_buf * rl, x_u32 len)
{
	x_s32 i;

	for(i = 0; i < len; i++)
		putch(' ');
}

static void rl_print(struct rl_buf * rl, x_s32 pos)
{
	x_u8 * utf8;

	if(pos < 0)
		return;

	if(pos > rl->len)
		return;

	utf8 = ucs4_to_utf8_alloc(rl->buf + pos, rl->len - pos);
	printk((const char *)utf8);

	free(utf8);
}

static struct rl_buf * rl_buf_alloc(x_s32 size)
{
	struct rl_buf * rl;
	x_s32 x, y, w, h;

	if(size <= 0)
		return NULL;

	if(!console_getxy(get_stdout(), &x, &y))
		return NULL;

	if(!console_getwh(get_stdout(), &w, &h))
		return NULL;

	rl = malloc(sizeof(struct rl_buf));
	if(!rl)
		return NULL;

	rl->size = size;
	rl->len = 0;
	rl->pos = 0;
	rl->cut = NULL;

	rl->x = x;
	rl->y = y;
	rl->w = w;
	rl->h = h;

	rl->buf = malloc(sizeof(x_u32) * rl->size);
	if(!rl->buf)
	{
		free(rl);
		return NULL;
	}

	return rl;
}

static void rl_buf_free(struct rl_buf * rl)
{
	if(rl->cut)
		free(rl->cut);

	free(rl->buf);
	free(rl);
}

static void rl_cursor_home(struct rl_buf * rl)
{
	if(rl->pos != 0)
	{
		rl->pos = 0;
		rl_gotoxy(rl);
	}
}

static void rl_cursor_end(struct rl_buf * rl)
{
	if(rl->pos != rl->len)
	{
		rl->pos = rl->len;
		rl_gotoxy(rl);
	}
}

static void rl_cursor_left(struct rl_buf * rl)
{
	if(rl->pos > 0)
	{
		rl->pos = rl->pos - 1;
		rl_gotoxy(rl);
	}
}

static void rl_cursor_right(struct rl_buf * rl)
{
	if(rl->pos < rl->len)
	{
		rl->pos = rl->pos + 1;
		rl_gotoxy(rl);
	}
}

static void rl_insert(struct rl_buf * rl, x_u32 * str)
{
	x_u32 * p;
	x_s32 len = ucs4_strlen(str);

	if(len + rl->len >= rl->size)
	{
		p = realloc(rl->buf, sizeof(x_u32) * rl->size * 2);
		if(!p)
			return;
		rl->size = rl->size * 2;
		rl->buf = p;
	}

	if(len + rl->len < rl->size)
	{
		memmove(rl->buf + rl->pos + len, rl->buf + rl->pos, (rl->len - rl->pos + 1) * sizeof(x_u32));
		memmove (rl->buf + rl->pos, str, len * sizeof(x_u32));

		rl->len = rl->len + len;
		rl_print(rl, rl->pos);
		rl->pos = rl->pos + len;
		rl_gotoxy(rl);
	}
}

static void rl_delete(struct rl_buf * rl, x_u32 len)
{
	if(len > rl->len - rl->pos)
		len = rl->len - rl->pos;

	if(rl->pos + len <= rl->len)
	{
		if(rl->cut)
			free(rl->cut);

		rl->cut = malloc((rl->len - rl->pos + 1) * sizeof(x_u32));
		if(rl->cut)
		{
			memcpy(rl->cut, rl->buf + rl->pos, (rl->len - rl->pos + 1) * sizeof(x_u32));
			rl->cut[rl->len - rl->pos] = '\0';
		}

		memmove(rl->buf + rl->pos, rl->buf + rl->pos + len, sizeof(x_u32) * (rl->len - rl->pos + 1));
		rl->len = rl->len - len;
		rl_print(rl, rl->pos);
		rl_space(rl, len);
		rl_gotoxy(rl);
	}
}

static x_bool readline_handle(struct rl_buf * rl, x_u32 code)
{
    x_u32 tmp[2];
    x_u32 n;

	switch(code)
	{
	case 0x0:	/* null */
		break;

	case 0x1:	/* ctrl-a: to the start of the line */
		rl_cursor_home(rl);
		break;

	case 0x2:	/* ctrl-b: to the left */
		rl_cursor_left(rl);
		break;

	case 0x3:	/* ctrl-c: break the readline */
		rl_cursor_home(rl);
		rl_delete(rl, rl->len);
		return TRUE;

	case 0x4:	/* ctrl-d: delete the character underneath the cursor */
		if(rl->pos < rl->len)
			rl_delete(rl, 1);
		break;

	case 0x5:	/* ctrl-e: to the end of the line */
		rl_cursor_end(rl);
		break;

	case 0x6:	/* ctrl-f: to the right */
		rl_cursor_right(rl);
		break;

	case 0x7:	/* ctrl-g */
		break;

	case 0x8:	/* ctrl-h: backspace */
		if(rl->pos > 0)
		{
			rl_cursor_left(rl);
			rl_delete(rl, 1);
		}
		break;

	case 0x9: 	/* ctrl-i: tab */
		break;

	case 0xa:	/* ctrl-j */
		return TRUE;

	case 0xb: 	/* ctrl-k: delete everything from the cursor to the end of the line */
		if(rl->pos < rl->len)
			rl_delete(rl, rl->len - rl->pos);
		break;

	case 0xc: 	/* ctrl-l */
		break;

	case 0xd: 	/* ctrl-m */
		return TRUE;

	case 0xe:	/* ctrl-n: the next history */
		break;

	case 0xf: 	/* ctrl-o */
		break;

	case 0x10:	/* ctrl-p: the previous history */
		break;

	case 0x11: 	/* ctrl-q */
		break;

	case 0x12:	/* ctrl-r */
		break;

	case 0x13: 	/* ctrl-s */
		break;

	case 0x14: 	/* ctrl-t */
		break;

	case 0x15: 	/* ctrl-u: delete everything from the cursor back to the line start */
		if(rl->pos > 0)
		{
			n = rl->pos;
			rl_cursor_home(rl);
			rl_delete(rl, n);
		}
		break;

	case 0x16:	/* ctrl-v */
		break;

	case 0x17: 	/* ctrl-w */
		break;

	case 0x18: 	/* ctrl-x */
		break;

	case 0x19:	/* ctrl-y: paste the killed text at the cursor position */
		if(rl->cut)
			rl_insert(rl, rl->cut);
		break;

	case 0x1a: 	/* ctrl-z */
		break;

	default:
	      tmp[0] = code;
	      tmp[1] = '\0';
	      rl_insert(rl, tmp);
		break;
	}

	return FALSE;
}

/*
 * read line with utf-8 stream
 */
x_s8 * readline(const x_s8 * prompt)
{
	struct rl_buf * rl;
	x_s8 * utf8 = NULL;
	x_u32 code;

	if(prompt)
		printk((char *)prompt);

	rl = rl_buf_alloc(256);
	if(!rl)
		return utf8;

	for(;;)
	{
		if(getcode(&code))
		{
			if(readline_handle(rl, code))
				break;
		}
	}

	if(rl->len > 0)
		utf8 = (x_s8 *)(ucs4_to_utf8_alloc(rl->buf, rl->len));

	rl_buf_free(rl);
	return utf8;
}
