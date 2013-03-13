/*
 * libc/readline/readline.c
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
#include <xboot/module.h>
#include <console/console.h>
#include <readline.h>

struct rl_buf {
	struct console * in;
	struct console * out;
	struct console * err;
	u32_t * buf;
	u32_t * cut;
	size_t size;
	size_t len;
	size_t pos;
	s32_t x, y;
	s32_t w, h;
};

struct history_list
{
	u32_t * history;
	struct list_head entry;
};

static struct history_list __history_list = {
	.entry = {
		.next	= &(__history_list.entry),
		.prev	= &(__history_list.entry),
	},
};
static struct history_list * history_list = &__history_list;
static struct history_list * history_current = &__history_list;

static s32_t ucs4_strlen(const u32_t * s)
{
	const u32_t * sc;

	if(!s)
		return 0;

	for(sc = s; *sc != '\0'; ++sc);
	return sc - s;
}

static s32_t history_numberof(void)
{
	s32_t i = 0;
	struct list_head * pos = (&history_list->entry)->next;

	while(!list_is_last(pos, (&history_list->entry)->next))
	{
		pos = pos->next;
		i++;
	}

	return i;
}

static bool_t history_remove(void)
{
	struct history_list * list, * list_prev;
	struct list_head * pos = (&history_list->entry)->prev;

	if(!list_empty(&history_list->entry))
	{
		list = list_entry(pos, struct history_list, entry);
		if(history_current == list)
		{
			list_prev = list_entry((&history_current->entry)->prev, struct history_list, entry);
			if(list_prev != history_list)
				history_current = list_prev;
			else
				history_current = NULL;
		}
		list_del(pos);
		free(list->history);
		free(list);
		return TRUE;
	}

	return FALSE;
}

static bool_t history_add(u32_t * history, u32_t len)
{
	u32_t * s;
	struct history_list * list;
	struct list_head * pos;

	history_current = history_list;
	if(!history)
		return FALSE;

	pos = (&history_list->entry)->next;
	list = list_entry(pos, struct history_list, entry);
	if(ucs4_strlen(list->history) == len)
	{
		if(memcmp(list->history, history, len * sizeof(u32_t)) == 0)
			return TRUE;
	}

	list = malloc(sizeof(struct history_list));
	if(!list)
	{
		free(list);
		return FALSE;
	}

	s = malloc((len + 1) * sizeof(u32_t));
	if(!s)
	{
		free(s);
		free(list);
		return FALSE;
	}

	if(history_numberof() >= 32)
		history_remove();

	memcpy(s, history, len * sizeof(u32_t));
	s[len] = '\0';

	list->history = s;
	list_add(&list->entry, &history_list->entry);

	return TRUE;
}

static u32_t * history_next(void)
{
	struct history_list * list;

	if(list_empty(&history_list->entry))
	{
		history_current = history_list;
		return NULL;
	}

	if(history_current == history_list)
		return NULL;

	list = list_entry((&history_current->entry)->prev, struct history_list, entry);
	history_current = list;
	if(list != history_list)
		return history_current->history;
	else
		return NULL;
}

static u32_t * history_prev(void)
{
	struct history_list * list;

	if(list_empty(&history_list->entry))
	{
		history_current = history_list;
		return NULL;
	}

	list = list_entry((&history_current->entry)->next, struct history_list, entry);
	if(list != history_list)
	{
		history_current = list;
		return history_current->history;
	}
	else
		return NULL;
}

static void rl_gotoxy(struct rl_buf * rl)
{
	s32_t x, y;
	s32_t pos, i, w;

	for(i = 0, pos = 0; i < rl->pos; i++)
	{
		w = ucs4_width(rl->buf[i]);
		if(w < 0)
			w = 0;
		pos += w;
	}

	x = ((rl->y * rl->w) + rl->x + pos) % (rl->w);
	y = ((rl->y * rl->w) + rl->x + pos) / (rl->w);

	console_gotoxy(rl->out, x, y);
}

static void rl_space(struct rl_buf * rl, u32_t len)
{
	s32_t i;

	for(i = 0; i < len; i++)
		console_print(rl->out, " ");
}

static void rl_print(struct rl_buf * rl, s32_t pos)
{
	char * utf8;

	if(pos < 0)
		return;

	if(pos > rl->len)
		return;

	utf8 = ucs4_to_utf8_alloc(rl->buf + pos, rl->len - pos);
	console_print(rl->out, utf8);

	free(utf8);
}

static struct rl_buf * rl_buf_alloc(const char * prompt)
{
	struct console * cin = get_console_stdin();
	struct console * cout = get_console_stdout();
	struct console * cerr = get_console_stderr();
	struct rl_buf * rl;
	s32_t x, y, w, h;

	if(!cin || !cout || !cerr)
		return NULL;

	if(prompt)
		console_print(cout, prompt);

	if(!console_getxy(cout, &x, &y))
		return NULL;

	if(!console_getwh(cout, &w, &h))
		return NULL;

	rl = malloc(sizeof(struct rl_buf));
	if(!rl)
		return NULL;

	rl->in = cin;
	rl->out = cout;
	rl->err = cerr;

	rl->size = 256;
	rl->len = 0;
	rl->pos = 0;
	rl->cut = NULL;

	rl->x = x;
	rl->y = y;
	rl->w = w;
	rl->h = h;

	rl->buf = malloc(sizeof(u32_t) * rl->size);
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

static void rl_insert(struct rl_buf * rl, u32_t * str)
{
	u32_t * p;
	s32_t len = ucs4_strlen(str);

	if(len <= 0)
		return;

	if(len + rl->len >= rl->size)
	{
		p = realloc(rl->buf, sizeof(u32_t) * rl->size * 2);
		if(!p)
			return;
		rl->size = rl->size * 2;
		rl->buf = p;
	}

	if(len + rl->len < rl->size)
	{
		memmove(rl->buf + rl->pos + len, rl->buf + rl->pos, (rl->len - rl->pos + 1) * sizeof(u32_t));
		memmove (rl->buf + rl->pos, str, len * sizeof(u32_t));

		rl->len = rl->len + len;
		rl_print(rl, rl->pos);
		rl->pos = rl->pos + len;
		rl_gotoxy(rl);
	}
}

static void rl_delete(struct rl_buf * rl, u32_t len)
{
	s32_t n, i, w;

	if(len > rl->len - rl->pos)
		len = rl->len - rl->pos;

	if(rl->pos + len <= rl->len)
	{
		for(i = 0, n = 0; i < len; i++)
		{
			w = ucs4_width(rl->buf[rl->pos + i]);
			if(w < 0)
				w = 0;
			n += w;
		}

		if(rl->cut)
			free(rl->cut);

		rl->cut = malloc((rl->len - rl->pos + 1) * sizeof(u32_t));
		if(rl->cut)
		{
			memcpy(rl->cut, rl->buf + rl->pos, (rl->len - rl->pos + 1) * sizeof(u32_t));
			rl->cut[rl->len - rl->pos] = '\0';
		}

		memmove(rl->buf + rl->pos, rl->buf + rl->pos + len, sizeof(u32_t) * (rl->len - rl->pos + 1));
		rl->len = rl->len - len;
		rl_print(rl, rl->pos);
		rl_space(rl, n);
		rl_gotoxy(rl);
	}
}

static bool_t readline_handle(struct rl_buf * rl, u32_t code)
{
	u32_t * p;
    u32_t tmp[2];
    u32_t n;

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

	case 0xb: 	/* ctrl-k: delete everything from the cursor to the end of the line */
		if(rl->pos < rl->len)
			rl_delete(rl, rl->len - rl->pos);
		break;

	case 0xc: 	/* ctrl-l */
		break;

	case 0xa:	/* ctrl-j: lf */
	case 0xd: 	/* ctrl-m: cr */
		if(rl->len > 0)
			history_add(rl->buf, rl->len);
		return TRUE;

	case 0xe:	/* ctrl-n: the next history */
		rl_cursor_home(rl);
		rl_delete(rl, rl->len);
		rl_insert(rl, history_next());
		break;

	case 0xf: 	/* ctrl-o */
		break;

	case 0x10:	/* ctrl-p: the previous history */
		p = history_prev();
		if(p)
		{
			rl_cursor_home(rl);
			rl_delete(rl, rl->len);
			rl_insert(rl, p);
		}
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
char * readline(const char * prompt)
{
	struct rl_buf * rl;
	char * utf8 = NULL;
	u32_t code;

	rl = rl_buf_alloc(prompt);
	if(!rl)
		return NULL;

	for(;;)
	{
		if(rl->in->getcode(rl->in, &code))
		{
			if(readline_handle(rl, code))
			{
				console_print(rl->out, "\r\n");
				break;
			}
		}
	}

	if(rl->len > 0)
		utf8 = ucs4_to_utf8_alloc(rl->buf, rl->len);

	rl_buf_free(rl);
	return utf8;
}
EXPORT_SYMBOL(readline);
