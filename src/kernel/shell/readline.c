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
#include <configs.h>
#include <xboot/scank.h>
#include <xboot/printk.h>
#include <time/timer.h>
#include <shell/history.h>
#include <shell/readline.h>

//TODO
#if 0
static x_s8 rl_buf[CONFIG_READLINE_BUF_SIZE];
static x_s32 rl_len = 0;
static x_s32 rl_pos = 0;

/*
 * readline cursor left
 */
static void rl_cursor_left(void)
{
	struct terminal_stdout_list * list;
	struct hlist_node * pos;

	if(rl_pos)
	{
		rl_pos--;
		hlist_for_each_entry(list,  pos, &stdout_list, node)
		{
			terminal_cursor_left(list->term, 1);
		}
	}
}

/*
 * readline cursor right
 */
static void rl_cursor_right(void)
{
	struct terminal_stdout_list * list;
	struct hlist_node * pos;

	if(rl_pos < rl_len)
	{
		rl_pos++;
		hlist_for_each_entry(list,  pos, &stdout_list, node)
		{
			terminal_cursor_right(list->term, 1);
		}
	}
}

/*
 * move readline cursor to home
 */
static void rl_cursor_home(void)
{
	struct terminal_stdout_list * list;
	struct hlist_node * pos;

	if(rl_pos)
	{
		hlist_for_each_entry(list,  pos, &stdout_list, node)
		{
			terminal_cursor_left(list->term, rl_pos);
		}
		rl_pos = 0;
	}
}

/*
 * move readline cursor to end
 */
static void rl_cursor_end(void)
{
	struct terminal_stdout_list * list;
	struct hlist_node * pos;

	if(rl_pos != rl_len)
	{
		hlist_for_each_entry(list,  pos, &stdout_list, node)
		{
			terminal_cursor_right(list->term, rl_len - rl_pos);
		}
		rl_pos = rl_len;
	}
}

/*
 * readline insert
 */
static void rl_insert(x_s8 c)
{
	struct terminal_stdout_list * list;
	struct hlist_node * pos;
	x_s32 i;

	if(rl_len + 1 < CONFIG_READLINE_BUF_SIZE)
	{
		memmove(rl_buf + rl_pos + 1, rl_buf + rl_pos, rl_len - rl_pos + 1);
		rl_buf[rl_pos++] = c;
		rl_buf[++rl_len] = '\0';

		for(i = rl_pos-1; i< rl_len; i++)
		{
			hlist_for_each_entry(list,  pos, &stdout_list, node)
			{
				terminal_write(list->term, (x_u8 *)(&rl_buf[i]), 1);
			}
		}

		if(rl_pos != rl_len)
		{
			hlist_for_each_entry(list,  pos, &stdout_list, node)
			{
				terminal_cursor_left(list->term, rl_len - rl_pos);
			}
		}
	}
}

/*
 * readline erase to eol
 */
static void rl_erase_eol(void)
{
	struct terminal_stdout_list * list;
	struct hlist_node * pos;
	x_s32 i;

	hlist_for_each_entry(list,  pos, &stdout_list, node)
	{
		for(i=rl_pos; i<rl_len; i++)
			terminal_write(list->term, (x_u8 *)(" "), 1);
		terminal_cursor_left(list->term, rl_len - rl_pos);
	}

	rl_buf[rl_pos] = '\0';
	rl_len = rl_pos;
}

/*
 * clear readline to empty
 */
static void rl_clear(void)
{
	rl_cursor_home();
	rl_erase_eol();
}

/*
 * readline delete
 */
static void rl_delete(void)
{
	struct terminal_stdout_list * list;
	struct hlist_node * pos;
	x_s32 i;

	if(rl_pos < rl_len)
	{
		memmove(rl_buf + rl_pos, rl_buf + rl_pos + 1, rl_len - rl_pos + 1);
		rl_len--;

		for(i = rl_pos; i < rl_len; i++)
		{
			hlist_for_each_entry(list,  pos, &stdout_list, node)
			{
				terminal_write(list->term, (x_u8 *)(&rl_buf[i]), 1);
			}
		}

		hlist_for_each_entry(list,  pos, &stdout_list, node)
		{
			terminal_write(list->term, (x_u8 *)(" "), 1);
			terminal_cursor_left(list->term, rl_len-rl_pos+1);
		}
	}
}

/*
 * readline backspace
 */
static void rl_backspace(void)
{
	if(rl_pos)
	{
		rl_cursor_left();
		rl_delete();
	}
}

/*
 * clear readline buffer
 */
static void clear_rl_buf(void)
{
	rl_buf[0] = '\0';
	rl_len = 0;
	rl_pos = 0;
}

/*
 * copy to readline buffer
 */
static x_s8 * copy_to_rl_buf(x_s8 * s)
{
	if(!s)
		clear_rl_buf();
	else
	{
		strcpy(rl_buf, s);
		rl_len = rl_pos = strlen(s);
	}
	return rl_buf;
}

/*
 * handle readline
 */
static x_bool readline_handle(x_s8 c)
{
	static enum readline_mode rl_mode = RL_NORMAL;
	static x_s32 rl_param = 0;
	x_s8 *p;

	switch (rl_mode)
	{
	case RL_NORMAL:
		switch (c)
		{
		case 1:		/* ctrl-a */
			rl_cursor_home();
			break;

		case 2:		/* ctrl-b */
			rl_cursor_left();
			break;

		case 3:		/* ctrl-c */
			rl_clear();
			return TRUE;

		case 4:		/* ctrl-d */
			rl_delete();
			break;

		case 5:		/* ctrl-e */
			rl_cursor_end();
			break;

		case 6:		/* ctrl-f */
			rl_cursor_right();
			break;

		case 9: 	/* ctrl-i or tab */
			break;

		case 11: 	/* ctrl-k */
			rl_erase_eol();
			break;

		case 18:	/* ctrl-r */
			break;

		case 24:	/* ctrl-x */
			xboot_set_mode(MODE_MENU);
			rl_clear();
			return TRUE;

		case 27:	/* esc */
			rl_mode = RL_ESC;
			break;

		case 8:		/* ctrl-h */
		case 127:	/* backspace */
			rl_backspace();
			break;

		case (x_s8)155:
			rl_mode = RL_CSI;
	    	break;

		case '\r':	/* enter */
		case '\n':
		case '\0':
			history_add(rl_buf);
			return TRUE;

		default:
			if(isprint(c))
				rl_insert(c);
			break;
		}
		break;

	case RL_ESC:
		if(c == '[')
		{
			rl_mode = RL_CSI;
        	rl_param = 0;
		}
        else
        {
        	rl_mode = RL_NORMAL;
        }
		break;

	case RL_CSI:
		switch (c)
		{
		case 'A':	/* up arrow */
		case 'F':
			p = history_get_prev_cmd();
			if(p)
			{
				rl_clear();
				copy_to_rl_buf(p);
				printk((char*)rl_buf);
			}
			break;

		case 'B':	/* down arrow */
		case 'E':
			rl_clear();
			p = history_get_next_cmd();
			copy_to_rl_buf(p);
			printk((char*)rl_buf);
			break;

		case 'D':	/* left arrow */
			rl_cursor_left();
			break;

		case 'C':	/* right arrow */
			rl_cursor_right();
			break;

        case '0' ... '9':
        	rl_param = rl_param * 10 + (c - '0');
        	goto the_end;

        case '~':
        	switch (rl_param)
        	{
        	case 1:
        		rl_cursor_home();
        		break;

        	case 2:		/* insert */
        		break;

        	case 3:		/* delete */
        		rl_delete();
        		break;

        	case 4:
        		rl_cursor_end();
        		break;

        	case 5:		/* page up */
    			p = history_get_prev_cmd();
    			if(p)
    			{
    				rl_clear();
    				copy_to_rl_buf(p);
    				printk((char*)rl_buf);
    			}
        		break;

        	case 6:		/* page down */
    			rl_clear();
    			p = history_get_next_cmd();
    			copy_to_rl_buf(p);
    			printk((char*)rl_buf);
        		break;

        	default:
        		break;
        	}

        default:
        	break;
		}
		rl_mode = RL_NORMAL;
the_end:
		break;

	default:
		break;
	}

	return FALSE;
}
#endif

static x_bool readline_handle(x_u32 code)
{
	switch(code)
	{
	case 0x1:	/* ctrl-a */
		break;

	case 0x2:	/* ctrl-b */
		break;

	case 0x3:	/* ctrl-c */
		return TRUE;

	case 0x4:	/* ctrl-d */
		break;

	case 0x5:	/* ctrl-e */
		break;

	case 0x6:	/* ctrl-f */
		break;

	case 0x9: 	/* ctrl-i */
		break;

	case 0xb: 	/* ctrl-k */
		break;

	case 0xe:	/* ctrl-n */
		break;

	case 0x10:	/* ctrl-p */
		break;

	case 0x12:	/* ctrl-r */
		break;

	case 0x15:	/* ctrl-u */
		break;

	case 0x19:	/* ctrl-y */
		break;

	default:
		break;
	}

	return FALSE;
}

/*
 * read one line from standard input console
 */
x_s8 * readline(const x_s8 * prompt)
{
	x_u32 code;

	if(prompt)
		printk((char *)prompt);

	for(;;)
	{
		if(getcode(&code))
		{
			if(readline_handle(code))
				break;
		}
	}

	printk("\r\n");

	return 0;
}
