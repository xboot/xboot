/*
 * kernel/shell/readline.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xboot.h>
#include <shell/readline.h>

enum esc_state_t {
	ESC_STATE_NORMAL,
	ESC_STATE_ESC,
	ESC_STATE_CSI,
};

struct rl_buf_t {
	u32_t * buf;
	u32_t * cut;
	int bsize;
	int pos;
	int len;

	enum esc_state_t state;
	int params[8];
	int num_params;
	char utf8[32];
	int usize;
};

struct history_list_t
{
	u32_t * history;
	struct list_head entry;
};

static struct history_list_t __history_list = {
	.entry = {
		.next	= &(__history_list.entry),
		.prev	= &(__history_list.entry),
	},
};
static struct history_list_t * history_list = &__history_list;
static struct history_list_t * history_current = &__history_list;

static int ucs4_strlen(const u32_t * s)
{
	const u32_t * sc;

	if(!s)
		return 0;
	for(sc = s; *sc != '\0'; ++sc);
	return sc - s;
}

static int history_numberof(void)
{
	struct list_head * pos = (&history_list->entry)->next;
	int i = 0;

	while(!list_is_last(pos, (&history_list->entry)->next))
	{
		pos = pos->next;
		i++;
	}
	return i;
}

static void history_remove(void)
{
	struct history_list_t * list, * list_prev;
	struct list_head * pos = (&history_list->entry)->prev;

	if(!list_empty(&history_list->entry))
	{
		list = list_entry(pos, struct history_list_t, entry);
		if(history_current == list)
		{
			list_prev = list_entry((&history_current->entry)->prev, struct history_list_t, entry);
			if(list_prev != history_list)
				history_current = list_prev;
			else
				history_current = NULL;
		}
		list_del(pos);
		free(list->history);
		free(list);
	}
}

static void history_add(u32_t * history, int len)
{
	u32_t * s;
	struct history_list_t * list;
	struct list_head * pos;

	history_current = history_list;
	if(!history)
		return;

	pos = (&history_list->entry)->next;
	list = list_entry(pos, struct history_list_t, entry);
	if(ucs4_strlen(list->history) == len)
	{
		if(memcmp(list->history, history, len * sizeof(u32_t)) == 0)
			return;
	}

	list = malloc(sizeof(struct history_list_t));
	if(!list)
	{
		free(list);
		return;
	}

	s = malloc((len + 1) * sizeof(u32_t));
	if(!s)
	{
		free(s);
		free(list);
		return;
	}

	if(history_numberof() >= 32)
		history_remove();

	memcpy(s, history, len * sizeof(u32_t));
	s[len] = '\0';

	list->history = s;
	list_add(&list->entry, &history_list->entry);
}

static u32_t * history_next(void)
{
	struct history_list_t * list;

	if(list_empty(&history_list->entry))
	{
		history_current = history_list;
		return NULL;
	}

	if(history_current == history_list)
		return NULL;

	list = list_entry((&history_current->entry)->prev, struct history_list_t, entry);
	history_current = list;
	if(list != history_list)
		return history_current->history;
	else
		return NULL;
}

static u32_t * history_prev(void)
{
	struct history_list_t * list;

	if(list_empty(&history_list->entry))
	{
		history_current = history_list;
		return NULL;
	}

	list = list_entry((&history_current->entry)->next, struct history_list_t, entry);
	if(list != history_list)
	{
		history_current = list;
		return history_current->history;
	}
	else
		return NULL;
}

static void rl_space(struct rl_buf_t * rl, int len)
{
	int i;

	for(i = 0; i < len; i++)
		printf(" ");
}

static void rl_print(struct rl_buf_t * rl, int from, int len)
{
	char * utf8;

	from = (from < 0) ? 0 : from;
	len = (len > rl->len) ? rl->len : len;

	utf8 = ucs4_to_utf8_alloc(rl->buf + from, len);
	printf("%s", utf8);
	free(utf8);
}

static void rl_cursor_save(struct rl_buf_t * rl)
{
	printf("\033[s");
}

static void rl_cursor_restore(struct rl_buf_t * rl)
{
	printf("\033[u");
}

static void rl_cursor_left(struct rl_buf_t * rl)
{
	if(rl->pos > 0)
	{
		rl->pos = rl->pos - 1;
		printf("\033[D");
	}
}

static void rl_cursor_right(struct rl_buf_t * rl)
{
	if(rl->pos < rl->len)
	{
		rl->pos = rl->pos + 1;
		printf("\033[C");
	}
}

static void rl_cursor_home(struct rl_buf_t * rl)
{
	while(rl->pos > 0)
	{
		rl_cursor_left(rl);
	}
}

static void rl_cursor_end(struct rl_buf_t * rl)
{
	while(rl->pos < rl->len)
	{
		rl_cursor_right(rl);
	}
}

static void rl_insert(struct rl_buf_t * rl, u32_t * str)
{
	int len = ucs4_strlen(str);

	if(len <= 0)
		return;

	if(len + rl->len >= rl->bsize)
	{
		rl->bsize = rl->bsize * 2;
		u32_t * p = realloc(rl->buf, sizeof(u32_t) * rl->bsize);
		if(!p)
			return;
		rl->buf = p;
	}

	if(len + rl->len < rl->bsize)
	{
		memmove(rl->buf + rl->pos + len, rl->buf + rl->pos, (rl->len - rl->pos + 1) * sizeof(u32_t));
		memmove (rl->buf + rl->pos, str, len * sizeof(u32_t));

		rl->pos = rl->pos + len;
		rl->len = rl->len + len;
		rl_print(rl, rl->pos - len, len);
		rl_cursor_save(rl);
		rl_print(rl, rl->pos, rl->len - rl->pos);
		rl_cursor_restore(rl);
	}
}

static void rl_delete(struct rl_buf_t * rl, u32_t len)
{
	int n, i, w;

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
		rl_cursor_save(rl);
		rl_print(rl, rl->pos, rl->len - rl->pos);
		rl_space(rl, n);
		rl_cursor_restore(rl);
	}
}

static struct rl_buf_t * rl_alloc(const char * prompt)
{
	struct rl_buf_t * rl;

	if(prompt)
		printf("%s", prompt);

	rl = malloc(sizeof(struct rl_buf_t));
	if(!rl)
		return NULL;

	rl->bsize = 128;
	rl->buf = malloc(sizeof(u32_t) * rl->bsize);
	rl->cut = NULL;
	rl->pos = 0;
	rl->len = 0;

	rl->state = ESC_STATE_NORMAL;
	rl->num_params = 0;
	rl->usize = 0;

	if(!rl->buf)
	{
		free(rl);
		return NULL;
	}

	return rl;
}

static void rl_free(struct rl_buf_t * rl)
{
	if(rl->cut)
		free(rl->cut);
	free(rl->buf);
	free(rl);
}

static bool_t rl_getcode(struct rl_buf_t * rl, u32_t * code)
{
	unsigned char c;
	char * rest;
	u32_t cp;
	int ch;
	int i;

	if((ch = getchar()) == EOF)
		return FALSE;
	c = ch;

	switch(rl->state)
	{
	case ESC_STATE_NORMAL:
		switch(c)
		{
		case 27:
			rl->state = ESC_STATE_ESC;
			break;

		case 127:				/* backspace */
			*code = 0x8;		/* ctrl-h */
			return TRUE;

		default:
			rl->utf8[rl->usize++] = c;
			if(utf8_to_ucs4(&cp, 1, (const char *)rl->utf8, rl->usize, (const char **)&rest) > 0)
			{
				rl->usize -= rest - rl->utf8;
				memmove(rl->utf8, rest, rl->usize);

				*code = cp;
				return TRUE;
			}
			break;
		}
		break;

	case ESC_STATE_ESC:
		if(c == '[')
		{
			for(i = 0; i < ARRAY_SIZE(rl->params); i++)
				rl->params[i] = 0;
			rl->num_params = 0;
			rl->state = ESC_STATE_CSI;
		}
		else
		{
			rl->state = ESC_STATE_NORMAL;
		}
		break;

	case ESC_STATE_CSI:
		if(c >= '0' && c <= '9')
		{
			if(rl->num_params < ARRAY_SIZE(rl->params))
			{
				rl->params[rl->num_params] = rl->params[rl->num_params] * 10 + c - '0';
			}
		}
		else
		{
			rl->num_params++;
			if(c == ';')
				break;

			rl->state = ESC_STATE_NORMAL;
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
				if(rl->num_params != 1)
					break;

				switch(rl->params[0])
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
		rl->state = ESC_STATE_NORMAL;
		break;
	}

	return FALSE;
}

static bool_t readline_handle(struct rl_buf_t * rl, u32_t code)
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

char * readline(const char * prompt)
{
	struct rl_buf_t * rl;
	char * utf8 = NULL;
	u32_t code;

	rl = rl_alloc(prompt);
	if(!rl)
		return NULL;

	for(;;)
	{
		if(rl_getcode(rl, &code))
		{
			if(readline_handle(rl, code))
			{
				printf("\r\n");
				break;
			}
		}
	}

	if(rl->len > 0)
		utf8 = ucs4_to_utf8_alloc(rl->buf, rl->len);
	rl_free(rl);
	return utf8;
}
