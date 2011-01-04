/*
 * init/mode/run_menu.c
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
#include <xboot/scank.h>
#include <xboot/list.h>
#include <xboot/menu.h>
#include <console/console.h>
#include <shell/exec.h>
#include <mode/mode.h>


struct menu_ctx
{
	struct menu_list * list;
	struct menu_item * item;

	x_s32 width, height;
	enum tcolor fg, bg;

	x_u32 x0, y0, x1, y1;

	/*
	 * saved information
	 */
	x_bool cursor;
	enum tcolor f, b;
};

static struct menu_ctx * menu_ctx_alloc(enum tcolor fg, enum tcolor bg)
{
	struct menu_ctx * ctx;
	x_s32 w, h;

	if(!get_stdout())
		return NULL;

	if(!console_getwh(get_stdout(), &w, &h))
		return NULL;

	ctx = malloc(sizeof(struct menu_ctx));
	if(!ctx)
		return NULL;

	ctx->list = get_menu_list();
	if(!ctx->list)
	{
		free(ctx);
		return NULL;
	}

	ctx->item = get_menu_indexof_item(0);

	ctx->cursor = console_getcursor(get_stdout());
	if(!console_getcolor(get_stdout(), &ctx->f, &ctx->b))
	{
		free(ctx);
		return NULL;
	}

	ctx->width = w;
	ctx->height = h;

	ctx->fg = fg;
	ctx->bg = bg;

	ctx->x0 = 1;
	ctx->y0 = 2;
	ctx->x1 = ctx->width - 2;

	if(ctx->width >= 44 && ctx->height > 12)
	{
		xboot_char_logo((ctx->width - 44) / 2, ctx->height - 7);
		ctx->y1 = ctx->height - 10;
	}
	else
	{
		ctx->y1 = ctx->height - 2;
	}

	console_setcolor(get_stdout(), ctx->fg, ctx->bg);
	console_setcursor(get_stdout(), FALSE);
	console_cls(get_stdout());

	console_gotoxy(get_stdout(), 1, 0);
	xboot_banner();

	console_rect(get_stdout(),
					UNICODE_HLINE, UNICODE_VLINE,
					UNICODE_LEFTTOP, UNICODE_RIGHTTOP,
					UNICODE_LEFTBOTTOM, UNICODE_RIGHTBOTTOM,
					ctx->x0, ctx->y0, ctx->x1, ctx->y1);

	return ctx;
}

static void menu_ctx_free(struct menu_ctx * ctx)
{
	if(ctx)
	{
		console_setcursor(get_stdout(), ctx->cursor);
		console_setcolor(get_stdout(), ctx->f, ctx->b);

		free(ctx);
	}
}

static void menu_ctx_paint(struct menu_ctx * ctx)
{
	struct console * stdout = get_stdout();

}

static void menu_ctx_move_up(struct menu_ctx * ctx)
{
	menu_ctx_paint(ctx);
}

static void menu_ctx_move_down(struct menu_ctx * ctx)
{
	menu_ctx_paint(ctx);
}

/*
 * running the menu mode
 */
void run_menu_mode(void)
{
	struct menu_ctx * ctx;
	x_u32 code;

	ctx = menu_ctx_alloc(TCOLOR_WHITE, TCOLOR_BLACK);
	if(!ctx)
		return;

	do {
		 if(getcode(&code))
		 {
			switch(code)
			{
				/* up arrow */
				case 0x10:
					menu_ctx_move_up(ctx);
					break;

				/* down arrow */
				case 0xe:
					menu_ctx_move_down(ctx);
					break;

				/* lf or cr */
				case 0xa:
				case 0xd:
					if(ctx->item && ctx->item->command)
						xboot_set_mode(MODE_SHELL);
					break;

				default:
					break;
			}
		 }
	} while(xboot_get_mode() == MODE_MENU);

	if(ctx->item && ctx->item->command)
		exec_cmdline((const x_s8 *)ctx->item->command);

	menu_ctx_free(ctx);
}
