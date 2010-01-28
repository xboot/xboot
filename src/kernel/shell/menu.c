/*
 * kernel/shell/menu.c
 *
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
#include <ctype.h>
#include <mode.h>
#include <string.h>
#include <malloc.h>
#include <version.h>
#include <vsprintf.h>
#include <xboot/log.h>
#include <xboot/list.h>
#include <xboot/scank.h>
#include <xboot/printk.h>
#include <xboot/machine.h>
#include <terminal/curses.h>
#include <terminal/terminal.h>
#include <time/tick.h>
#include <time/timer.h>
#include <shell/command.h>
#include <shell/readline.h>
#include <shell/env.h>
#include <shell/parser.h>
#include <shell/shell.h>
#include <shell/menu.h>


extern struct hlist_head stdout_list;
extern struct hlist_head stdin_list;

static struct menu_item * menu_entry;
static x_u32 menu_item_num = 0;
static x_u32 menu_item_posion = 0;
static x_s32 menu_item_autorun_delay = CONFIG_MENU_MODE_AUTORUN_DELAY;

/*
 * default menu, must place NULL at the end.
 */
static struct menu_item default_menu[] = {
	{
		.name		= "Shell Command Line",
		.context	= "clear; version; exit -s;"
	}, {
		.name		= "Memory Test",
		.context	= "memtest;"
	}, {
		.name		= NULL,
		.context	= NULL
	}
};

/*
 * update menu.
 */
static x_bool update_menu(struct terminal * term)
{
	x_s8 line_buf[256];
	x_u32 i, len;
	x_s32 width, height;
	x_u32 x0, y0, x1, y1;
	x_u32 window1 = 0, window2 = 0;

	if(!term || !term->getwh(term, &width, &height))
		return FALSE;

	terminal_mode_closeall(term);

	x0 = 1;
	y0 = 2;
	x1 = width - 2;
	if(width >= 44 && height > 12)
	{
		xboot_char_logo(term, (width-44)/2, height-7);
		y1 = height - 10;
	}
	else
		y1 = height - 2;

	terminal_setxy(term, 1, 0);
	xboot_banner(term);

	terminal_draw_rect(term, x0, y0, x1, y1);

	if(menu_item_autorun_delay > 0)
	{
		terminal_setxy(term, 0, y1+1);
		sprintf(line_buf, (x_s8*)" press any key to stop, the selected will be booted in %2ld s.", menu_item_autorun_delay);
		terminal_write(term, (x_u8 *)line_buf, strlen(line_buf));
	}
	else
	{
		terminal_setxy(term, 0, y1+1);
		sprintf(line_buf, (x_s8*)"%*s", (int)width, "");
		terminal_write(term, (x_u8 *)line_buf, strlen(line_buf));
	}

	if(menu_item_posion+1 > menu_item_num)
		menu_item_posion = menu_item_num -1;

	if(window2 - window1 + 1 != y1 -y0 -1)
	{
		window1 = 0;
		window2 = y1 -y0 -2;
	}

	if(menu_item_posion > window2)
	{
		window2 = menu_item_posion;
		window1 = menu_item_posion +2 - (y1 -y0);
	}

	if(menu_item_posion < window1)
	{
		window1 = menu_item_posion;
		window2 = menu_item_posion + y1 -y0 -2;
	}

	terminal_setxy(term, x1 - 1, y0 + 1);
	if(window1 > 0)
		terminal_curses_putch(term, TERMINAL_UP);
	else
		terminal_curses_putch(term, TERMINAL_SPACE);

	terminal_setxy(term, x1 - 1, y1 - 1);
	if(window2 + 1 < menu_item_num)
		terminal_curses_putch(term, TERMINAL_DOWN);
	else
		terminal_curses_putch(term, TERMINAL_SPACE);

	for(i = window1; i <= window2; i++)
	{
		if(i+1 > menu_item_num)
			break;

		if(i == menu_item_posion)
			terminal_mode_reverse(term);
		else
			terminal_mode_closeall(term);

		terminal_setxy(term, x0 + 2, y0 + i + 1 - window1);
		len = strlen((const x_s8 *)menu_entry[i].name);

		if( len < (x1 - x0) )
			sprintf(line_buf, (x_s8*)"%s%*s", menu_entry[i].name, (int)((x1 - x0) - len - 3), " ");
		else
		{
			strncpy(line_buf, (const x_s8 *)menu_entry[i].name, (x1 - x0) -3 );
			line_buf[(x1 - x0) - 3] = 0;
		}
		terminal_write(term, (x_u8 *)line_buf, strlen(line_buf));
	}
	terminal_mode_closeall(term);

	return TRUE;
}

/*
 * initial menu
 */
static void initial_menu(void)
{
	struct machine * mach = get_machine();
	struct menu_item * item, * p;
	struct terminal_stdout_list * list;
	struct hlist_node * pos;
	x_u32 i;

	hlist_for_each_entry(list,  pos, &stdout_list, node)
	{
		terminal_mode_closeall(list->term);
		terminal_clear_screen(list->term);
		terminal_cursor_home(list->term);
		terminal_cursor_hide(list->term);
	}

	if(mach && mach->cfg.menu)
		p = item = mach->cfg.menu;
	else
		p = item = default_menu;

	menu_item_num = 0;
	while(p->name)
	{
		p++;
		menu_item_num++;
	}
	if(menu_item_num == 0)
		menu_item_num = 1;

	menu_entry = malloc(sizeof(struct menu_item) * menu_item_num);
	if(!menu_entry)
		return;

	for(i=0, p=item; p->name; i++, p++)
	{
		menu_entry[i].name = p->name;
		menu_entry[i].context = p->context;
	}

	if(mach && (mach->cfg.mode != MODE_MENU))
		menu_item_autorun_delay = 0;

	hlist_for_each_entry(list,  pos, &stdout_list, node)
	{
		update_menu(list->term);
	}
}

/*
 * free menu
 */
static void free_menu(void)
{
	struct terminal_stdout_list * list;
	struct hlist_node * pos;

	hlist_for_each_entry(list,  pos, &stdout_list, node)
	{
		terminal_mode_closeall(list->term);
		terminal_clear_screen(list->term);
		terminal_cursor_home(list->term);
		terminal_cursor_show(list->term);
	}

	free(menu_entry);
}

/*
 * running the menu mode
 */
void run_menu_mode(void)
{
	struct terminal_stdout_list * list;
	struct hlist_node * pos;
	x_s8 c;
	x_s8 * cmdline;
	enum readline_mode rl_mode = RL_NORMAL;
	x_s32 rl_param = 0;
	x_u32 jiffies_saved = jiffies;

	/* initial menu */
	initial_menu();

	do {
		if(getch((char*)&c))
		{
			/*
			 * press key and stop auto run
			 */
			if(menu_item_autorun_delay > 0)
			{
				menu_item_autorun_delay = 0;
				hlist_for_each_entry(list,  pos, &stdout_list, node)
				{
					update_menu(list->term);
				}
			}

			switch (rl_mode)
			{
			case RL_NORMAL:
				switch (c)
				{
				case 3:		/* ctrl-c */
				case 'c':
				case 'C':
					break;

				case 27:	/* esc */
					rl_mode = RL_ESC;
					break;

				case 32:	/* space */
					break;

				case (x_s8)155:
					rl_mode = RL_CSI;
			    	break;

				case '\r':	/* enter */
				case '\n':
				case '\0':
					if(menu_item_autorun_delay > 0)
						menu_item_autorun_delay = 0;
					cmdline = (x_s8 *)menu_entry[menu_item_posion].context;
					if(cmdline)
						xboot_set_mode(MODE_SHELL);
					break;

				default:
					break;
				}
				break;

			case RL_ESC:
				if (c == '[')
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
					if(menu_item_posion > 0)
						menu_item_posion--;
					else
						menu_item_posion = menu_item_num;
					hlist_for_each_entry(list,  pos, &stdout_list, node)
					{
						update_menu(list->term);
					}
					break;

				case 'B':	/* down arrow */
				case 'E':
					if(menu_item_posion + 1 < menu_item_num)
						menu_item_posion++;
					else
						menu_item_posion = 0;

					hlist_for_each_entry(list,  pos, &stdout_list, node)
					{
						update_menu(list->term);
					}
					break;

				case 'D':	/* left arrow */
					break;

				case 'C':	/* right arrow */
					break;

		        case '0' ... '9':
		        	rl_param = rl_param * 10 + (c - '0');
		        	goto the_end;

		        case '~':
		        	switch (rl_param)
		        	{
		        	case 5:		/* page up */
						if(menu_item_posion > 0)
							menu_item_posion--;
						hlist_for_each_entry(list,  pos, &stdout_list, node)
						{
							update_menu(list->term);
						}
		        		break;

		        	case 6:		/* page down */
						if(menu_item_posion + 1 < menu_item_num)
							menu_item_posion++;
						hlist_for_each_entry(list,  pos, &stdout_list, node)
						{
							update_menu(list->term);
						}
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
		}

		if(menu_item_autorun_delay > 0)
		{
			if(get_system_hz() > 0)
			{
				if(jiffies > jiffies_saved + get_system_hz())
				{
					jiffies_saved = jiffies;
					menu_item_autorun_delay--;

					hlist_for_each_entry(list,  pos, &stdout_list, node)
					{
						update_menu(list->term);
					}
				}
			}
			else
			{
				/* not exact at all, depend on cpu's speed */
				jiffies_saved = (jiffies_saved + 1) & 0x3ff;
				if(jiffies_saved == 0)
				{
					menu_item_autorun_delay--;

					hlist_for_each_entry(list,  pos, &stdout_list, node)
					{
						update_menu(list->term);
					}
				}
			}

			if(menu_item_autorun_delay <= 0)
			{
				cmdline = (x_s8 *)menu_entry[menu_item_posion].context;
				if(cmdline)
					xboot_set_mode(MODE_SHELL);
			}
		}
	}while(xboot_get_mode() == MODE_MENU);

	/* free menu */
	free_menu();

	/* run command line */
	run_cmdline(cmdline);
}

/*
 * running the normal mode
 */
void run_normal_mode(void)
{
	struct machine * mach = get_machine();
	struct menu_item * item;

	set_stdout_status(FALSE);

	if(mach && mach->cfg.menu)
		item = mach->cfg.menu;
	else
		item = default_menu;

	xboot_set_mode(MODE_SHELL);

	if(item->name && item->context)
		run_cmdline((const x_s8 *)item->context);

	set_stdout_status(TRUE);
}
